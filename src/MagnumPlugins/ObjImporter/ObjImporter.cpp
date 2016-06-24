/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017, 2018
              Vladimír Vondruš <mosra@centrum.cz>

    Permission is hereby granted, free of charge, to any person obtaining a
    copy of this software and associated documentation files (the "Software"),
    to deal in the Software without restriction, including without limitation
    the rights to use, copy, modify, merge, publish, distribute, sublicense,
    and/or sell copies of the Software, and to permit persons to whom the
    Software is furnished to do so, subject to the following conditions:

    The above copyright notice and this permission notice shall be included
    in all copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
    THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
    LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
    FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
    DEALINGS IN THE SOFTWARE.
*/

#include "ObjImporter.h"

#include <fstream>
#include <limits>
#include <sstream>
#include <tuple>
#include <unordered_map>
#include <Corrade/Containers/ArrayView.h>
#include <Corrade/Utility/String.h>
#include <Corrade/Utility/Directory.h>

#include "Magnum/Mesh.h"
#include "Magnum/MeshTools/CombineIndexedArrays.h"
#include "Magnum/MeshTools/Duplicate.h"
#include "Magnum/Math/Vector3.h"
#include "Magnum/Math/Color.h"
#include "Magnum/Trade/MeshData3D.h"
#include "Magnum/Trade/ImageData.h"
#include "Magnum/Trade/PhongMaterialData.h"

#include "MagnumPlugins/TgaImporter/TgaImporter.h"

#if defined(CORRADE_TARGET_NACL_NEWLIB) || defined(CORRADE_TARGET_ANDROID)
#include <sstream>
#endif

using namespace Corrade::Containers;

namespace Magnum { namespace Trade {


// Not using PhongMaterialData, since we may want to set color and texture to
// later decide which flags we set. We do not know whether we have a texture
// or color beforehand.
struct ObjMaterial {
    std::string name;
    Color3 ambient;
    Color3 diffuse;
    Color3 specular;
    Float specularity;
    UnsignedInt ambientTexture = -1;
    UnsignedInt diffuseTexture = -1;
    UnsignedInt specularTexture = -1;
};

// Semantic object for a set of indices belonging to a mesh.
struct ObjMesh {
    std::vector<std::array<int, 3>> indices;
};

// Kinda misleading: *Not* similar to the OpenGexImporter Importer State.
// This just holds all the members of the importer to 1. avoid huge includes
// in the header and 2. not have to switch file when adding new ones (I needed
// to be done quickly ;) )
struct ImporterState {
    std::vector<Vector3> positions;
    std::vector<Vector2> textureCoordinates;
    std::vector<Vector3> normals;
    std::vector<ObjMaterial> materials;
    std::vector<Int> meshMaterials;
    std::unordered_map<std::string, int> materialIndices;
    std::vector<std::unique_ptr<ObjMesh>> meshes;
    std::vector<std::string> textures;
    std::unordered_map<std::string, int> textureIndices;
};

namespace {

// entire contents of data in a std::string
std::string arrayToString(const Containers::ArrayView<const char> data) {
    /* Propagate errors */
    if(!data) return "";

    if(data.empty() ) {
        return "";
    }

    std::string out;

    for(const char* i = data; i != data.end(); ++i) {
        const char c = *i;
        out += c;
    }

    return out;
}

// find first index of c in pos, cancel search at newline or whitespace if flag set.
int findNext(const ArrayView<char>& pos, char c, bool termByNewline=false, bool termByWhitespace=false) {
    for(int i = 0; i < pos.size(); ++i) {
        if(pos[i] == c) {
            return i;
        }

        if((pos[i] == '\n' && termByNewline) || (pos[i] == ' ' && termByWhitespace)) {
            return -1;
        }
    }
    return -1;
}

// find next non-whitespace char and return suffix at that point.
// param strict if "true", '\n' and '\r' are not considered whitespace
ArrayView<char> skipWhitespaces(const ArrayView<char>& pos, bool strict=false) {
    for(int i = 0; i < pos.size(); ++i) {
        const char c = pos[i];
        if(c != ' ' && c != '\t' && (strict || (c != '\n'  && c != '\r'))) {
            return pos.suffix(i);
        }
    }
    return ArrayView<char>{};
}

// returns suffix from after next '\n'
ArrayView<char> ignoreLine(const ArrayView<char>& pos) {
    return pos.suffix(findNext(pos, '\n') + 1);
}

// same as ignoreLine, but also returns the content "ignored".
// Different description: Get string until next '\n' and return ArrayView of data
// after it.
std::tuple<std::string, ArrayView<char>> nextLine(const ArrayView<char>& pos) {
    int i = Math::min(findNext(pos, '\n'), findNext(pos, '\r'));
    if(i == -1) {
        i = pos.size();
        std::string str = arrayToString(pos.prefix(i));
        return std::make_tuple(str, pos.suffix(i));
    }
    std::string str = arrayToString(pos.prefix(i));
    return std::make_tuple(str, pos.suffix(i+1));
}

// Parse a "v/n/t" string to indices
// Warning: I'm not handing cases like "v/n", where the normal is not terminated by '/', but ' '!
// Same for "v", where even the normal is omitted.
std::tuple<std::array<int, 3>, ArrayView<char>> parseVertex(const ArrayView<char>& pos) {
    std::array<int, 3> indices{-1, -1, -1};

    int i = findNext(pos, '/');
    indices[0] = stoi(arrayToString(pos.prefix(i)));
    auto endpos = pos.suffix(i+1);

    i = findNext(endpos, '/', true, true);
    if(i != -1) { // there may not be a normal! Eg. "1//2"
        auto prefix = endpos.prefix(i);
        if(!prefix.empty()) {
            indices[1] = stoi(arrayToString(prefix));
            endpos = endpos.suffix(i+1);
        }
    }

    i = findNext(endpos, ' ', true, true); // texture coordinates are not terminated by '/', but ' ' or newline
    if(i == -1)
        i = findNext(endpos, '\r', true, true);
    if(i == -1)
        i = findNext(endpos, '\n', true, true);

    if(i != -1) { // there may not be a texCoord! Eg. "1//"
        auto prefix = endpos.prefix(i);
        if(!prefix.empty()) {
            indices[2] = stoi(arrayToString(prefix));
            endpos = endpos.suffix(i+1);
        }
    }

    return std::make_tuple(indices, endpos);
}

// get string of content until next ' ' and also return ArrayView for data after the word.
std::tuple<std::string, ArrayView<char>> nextWord(const ArrayView<char>& pos) {
    int i = 0;
    for(; i < pos.size(); ++i) {
        if(pos[i] == ' ' || pos[i] == '\r' || pos[i] == '\n') {
            break;
        }
    }
    std::string str = arrayToString(pos.prefix(i));
    return std::make_tuple(str, pos.suffix(i+1));
}

}

ObjImporter::ObjImporter() = default;

ObjImporter::ObjImporter(PluginManager::AbstractManager& manager, const std::string& plugin): AbstractImporter{manager, plugin} {}

ObjImporter::~ObjImporter() = default;

auto ObjImporter::doFeatures() const -> Features { return Feature::OpenData; }

bool ObjImporter::doIsOpened() const { return _in; }

void ObjImporter::doClose() { _in = nullptr; }

void ObjImporter::doOpenFile(const std::string& filename) {
    _fileRoot = filename.substr(0, filename.find_last_of('/')+1);
    AbstractImporter::doOpenFile(filename);
}

void ObjImporter::doOpenData(Containers::ArrayView<const char> data) {
    _in = Containers::Array<char>{data.size()};
    std::copy(data.begin(), data.end(), _in.begin());
    _state.reset(new ImporterState);
    parse();
}

void ObjImporter::parse() {
    ArrayView<char> pos = _in;
    ObjMesh* mesh = new ObjMesh;

    int currentMaterialIndex = -1;

    while(!pos.empty()) {

        /* Comment line */
        if(pos[0] == '#') {
            pos = ignoreLine(pos);
            pos = skipWhitespaces(pos);
            continue;
        }

        /* Parse the keyword */
        std::string keyword;
        std::tie(keyword, pos) = nextWord(pos);

        pos = skipWhitespaces(pos);

        /* Vertex position */
        if(keyword == "v") {
            std::string word;

            // All of this code should be abstracted away in a function.
            // Currently copy pasted to vt, vn, Ka, Ks, Kd...
            std::tie(word, pos) = nextWord(pos);
            const float x = std::stof(word);

            std::tie(word, pos) = nextWord(pos);
            const float y = std::stof(word);

            std::tie(word, pos) = nextWord(pos);
            const float z = std::stof(word);

            _state->positions.push_back(Vector3{x, y, z});
        /* Texture coordinate */
        } else if(keyword == "vt") {
            std::string word;

            std::tie(word, pos) = nextWord(pos);
            const float x = std::stof(word);

            std::tie(word, pos) = nextWord(pos);
            const float y = std::stof(word);

            _state->textureCoordinates.push_back(Vector2{x, y});

        /* Normal */
        } else if(keyword == "vn") {
            std::string word;

            std::tie(word, pos) = nextWord(pos);
            const float x = std::stof(word);

            std::tie(word, pos) = nextWord(pos);
            const float y = std::stof(word);

            std::tie(word, pos) = nextWord(pos);
            const float z = std::stof(word);

            _state->normals.push_back(Vector3{x, y, z});

        /* Indices */
        } else if(keyword == "f") {
            // Not handling case that there are more than three vertices!
            for(int i = 0; i < 3; ++i) {
                std::array<int, 3> indices;
                std::tie(indices, pos) = parseVertex(pos);
                mesh->indices.push_back(indices);
                pos = skipWhitespaces(pos, true);
            }
        /* Load a material library */
        } else if(keyword == "mtllib") {
            std::string word;
            pos = skipWhitespaces(pos);
            std::tie(word, pos) = nextWord(pos);

            parseMaterialLibrary(word);

        /* Set current material and add a new mesh for it */
        } else if(keyword == "usemtl") {
            std::string word;
            pos = skipWhitespaces(pos);
            std::tie(word, pos) = nextWord(pos);

            int materialIndex = _state->materialIndices[word];
            if(materialIndex != currentMaterialIndex) {
                currentMaterialIndex = materialIndex;
                /* switching the material here, need to create a new mesh */
                _state->meshMaterials.push_back(materialIndex);

                if(!mesh->indices.empty()) {
                    _state->meshes.push_back(std::unique_ptr<ObjMesh>(mesh));
                } /* empty meshes are skipped */
                mesh = new ObjMesh;
            } // else: usemtl did not result in material switch, no need to create new mesh

        /* Ignore unsupported keywords, error out on unknown keywords */
        } else {
            Warning() << "Trade::ObjImporter::parse(): unknown keyword:" << keyword;
        }

        /* Ignore the rest of the line */
        pos = skipWhitespaces(ignoreLine(pos));
    }

    if(!mesh->indices.empty()) {
        _state->meshes.push_back(std::unique_ptr<ObjMesh>(mesh));
    }
}

void ObjImporter::parseMaterialLibrary(std::string libname) {
    std::string filename = _fileRoot + libname;

    /* Open file */
    if(!Utility::Directory::fileExists(filename)) {
        Error() << "Trade::AbstractImporter::parseMaterialLibrary(): cannot open file" << filename;
        return;
    }

    Containers::Array<char> contents = Utility::Directory::read(filename);

    ArrayView<char> pos = contents;
    ObjMaterial* mat = nullptr;

    while(!pos.empty()) {

        /* Comment line */
        if(pos[0] == '#') {
            pos = ignoreLine(pos);
            pos = skipWhitespaces(pos);
            continue;
        }

        /* Parse the keyword */
        std::string keyword;
        std::tie(keyword, pos) = nextWord(pos);

        if(keyword.empty()) {
            continue;
        }

        pos = skipWhitespaces(pos);

        if(keyword == "newmtl") {
            if(mat != nullptr) {
                _state->materials.push_back(*mat);
                _state->materialIndices.insert(
                            std::make_pair(mat->name, _state->materials.size()-1));
                delete mat;
            }
            mat = new ObjMaterial;
            std::tie(mat->name, pos) = nextWord(pos);
            continue;
        } else if (mat == nullptr) {
            Error() << "Expected newmtl keyword, got" << keyword;
        }

        /* Ambient color */
        if(keyword == "Ka") {
            std::string word;

            // again, this code is duplicated alot.
            std::tie(word, pos) = nextWord(pos);
            const float r = std::stof(word);

            std::tie(word, pos) = nextWord(pos);
            const float g = std::stof(word);

            std::tie(word, pos) = nextWord(pos);
            const float b = std::stof(word);

            mat->ambient = Color3(r, g, b);
        /* Diffuse color */
        } else if(keyword == "Kd") {
            std::string word;

            std::tie(word, pos) = nextWord(pos);
            const float r = std::stof(word);

            std::tie(word, pos) = nextWord(pos);
            const float g = std::stof(word);

            std::tie(word, pos) = nextWord(pos);
            const float b = std::stof(word);

            mat->diffuse = Color3(r, g, b);

        /* Specular color */
        } else if(keyword == "Ks") {
            std::string word;

            std::tie(word, pos) = nextWord(pos);
            const float r = std::stof(word);

            std::tie(word, pos) = nextWord(pos);
            const float g = std::stof(word);

            std::tie(word, pos) = nextWord(pos);
            const float b = std::stof(word);

            mat->specular = Color3(r, g, b);

        /* Specularity */
        } else if(keyword == "Ns") {
            std::string word;

            std::tie(word, pos) = nextWord(pos);
            const float f = std::stof(word);

            mat->specularity = f;
        /* Ambient texture */
        } else if(keyword == "map_Ka") {
            std::string texture;
            std::tie(texture, pos) = nextLine(pos);

            // This is also very similar code for every "map_*"
            if(_state->textureIndices.find(texture) == _state->textureIndices.end()) {
                /* new texture, create it */
                int index = _state->textures.size();
                _state->textures.push_back(texture);

                _state->textureIndices[texture] = index;
                mat->ambientTexture = index;
            } else {
                mat->ambientTexture = _state->textureIndices[texture];
            }

        /* Diffuse texture */
        } else if(keyword == "map_Kd") {
            std::string texture;
            std::tie(texture, pos) = nextLine(pos);

            if(_state->textureIndices.find(texture) == _state->textureIndices.end()) {
                /* new texture, create it */
                int index = _state->textures.size();
                _state->textures.push_back(texture);

                _state->textureIndices[texture] = index;
                mat->diffuseTexture = index;
            } else {
                mat->diffuseTexture = _state->textureIndices[texture];
            }

        /* Specular texture */
        } else if(keyword == "map_Ks") {
            std::string texture;
            std::tie(texture, pos) = nextLine(pos);

            if(_state->textureIndices.find(texture) == _state->textureIndices.end()) {
                /* new texture, create it */
                int index = _state->textures.size();
                _state->textures.push_back(texture);

                _state->textureIndices[texture] = index;
                mat->specularTexture = index;
            } else {
                mat->specularTexture = _state->textureIndices[texture];
            }

        /* Ignore unsupported keywords, error out on unknown keywords */
        } else {
            Warning() << "Trade::ObjImporter::parseMaterialLibrary(): unknown keyword:" << keyword;
        }

        /* Ignore the rest of the line */
        pos = skipWhitespaces(ignoreLine(pos));
    }

    // add last currently active material to materials vector. Usually added in "newmtl" keyword handling
    if(mat != nullptr) {
        _state->materials.push_back(*mat);
        _state->materialIndices.insert(
                    std::make_pair(mat->name, _state->materials.size()-1));
        delete mat;
    }
}

UnsignedInt ObjImporter::doMesh3DCount() const { return _state->meshes.size(); }

UnsignedInt ObjImporter::doMaterialCount() const { return _state->meshMaterials.size(); }


UnsignedInt ObjImporter::doImage2DCount() const { return _state->textures.size(); }

Int ObjImporter::doMesh3DForName(const std::string& name) {
    return 0;
}

std::string ObjImporter::doMesh3DName(UnsignedInt id) {
    return "";
}

std::optional<MeshData3D> ObjImporter::doMesh3D(UnsignedInt id) {
    ObjMesh& mesh = *_state->meshes[id];

    std::vector<Vector3> positions, normals;
    std::vector<Vector2> textureCoords;

    Debug() << "Have" << mesh.indices.size() << "vertices"; //

    // resolve indices... probably use combineIndexArrays instead?
    positions.reserve(mesh.indices.size());
    normals.reserve(mesh.indices.size());
    textureCoords.reserve(mesh.indices.size());

    for(std::array<int, 3>& indexArray : mesh.indices) {
        positions.push_back(_state->positions[indexArray[0]-1]);

        if(indexArray[1] != -1) // even though this looks like it's handling the case, it's not, this is utterly useless.
            // may result in differently sized arrays, which is not valid.
            normals.push_back(_state->normals[indexArray[1]-1]);
        if(indexArray[2] != -1)
            textureCoords.push_back(_state->textureCoordinates[indexArray[2]-1]);
    }

    return MeshData3D(
                MeshPrimitive::Triangles,
                {},
                {positions},
                {normals},
                {textureCoords});
}

std::unique_ptr<AbstractMaterialData> ObjImporter::doMaterial(UnsignedInt id) {
    ObjMaterial& objMat = _state->materials[_state->meshMaterials[id]];
    PhongMaterialData::Flags flags;

    if(objMat.ambientTexture != -1) {
        flags |= PhongMaterialData::Flag::AmbientTexture;
    }
    if(objMat.diffuseTexture != -1) {
        flags |= PhongMaterialData::Flag::DiffuseTexture;
    }
    if(objMat.specularTexture != -1) {
        flags |= PhongMaterialData::Flag::SpecularTexture;
    }

    PhongMaterialData* mat = new PhongMaterialData{
            flags,
            objMat.specularity};

    if(objMat.ambientTexture == -1) {
        mat->ambientColor() = objMat.ambient;
    } else {
        mat->ambientTexture() = objMat.ambientTexture;
    }

    if(objMat.diffuseTexture == -1) {
        mat->diffuseColor() = objMat.diffuse;
    } else {
        mat->diffuseTexture() = objMat.diffuseTexture;
    }

    if(objMat.specularTexture == -1) {
        mat->specularColor() = objMat.specular;
    } else {
        mat->specularTexture() = objMat.specularTexture;
    }

    return std::unique_ptr<AbstractMaterialData>(mat);
}

std::optional<ImageData2D> ObjImporter::doImage2D(UnsignedInt id) {
    CORRADE_ASSERT(manager(), "Trade::ObjImporter::image2D(): the plugin must be instantiated with access to plugin manager in order to open image files", {});

    std::unique_ptr<AbstractImporter> imageImporter = manager()->loadAndInstantiate("TgaImporter"); // probably AnyImageImporter would be the way to go here...
    if(!imageImporter->openFile(_fileRoot + _state->textures[id])) {
        return std::nullopt;
    }

    Debug() << "Loading image2D" << _fileRoot + _state->textures[id];

    return imageImporter->image2D(0);
}


}}
