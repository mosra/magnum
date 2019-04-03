/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017, 2018, 2019
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
#include <unordered_map>
#include <Corrade/Containers/ArrayView.h>
#include <Corrade/Containers/Optional.h>
#include <Corrade/Utility/DebugStl.h>
#include <Corrade/Utility/String.h>

#include "Magnum/Mesh.h"
#include "Magnum/MeshTools/CombineIndexedArrays.h"
#include "Magnum/MeshTools/Duplicate.h"
#include "Magnum/Math/Color.h"
#include "Magnum/Trade/MeshData3D.h"

namespace Magnum { namespace Trade {

struct ObjImporter::File {
    std::unordered_map<std::string, UnsignedInt> meshesForName;
    std::vector<std::string> meshNames;
    std::vector<std::tuple<std::streampos, std::streampos, UnsignedInt, UnsignedInt, UnsignedInt>> meshes;
    Containers::Pointer<std::istream> in;
};

namespace {

void ignoreLine(std::istream& in) {
    in.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
}

template<std::size_t size> Math::Vector<size, Float> extractFloatData(const std::string& str, Float* extra = nullptr) {
    std::vector<std::string> data = Utility::String::splitWithoutEmptyParts(str, ' ');
    if(data.size() < size || data.size() > size + (extra ? 1 : 0)) {
        Error() << "Trade::ObjImporter::mesh3D(): invalid float array size";
        throw 0;
    }

    Math::Vector<size, Float> output;

    for(std::size_t i = 0; i != size; ++i)
        output[i] = std::stof(data[i]);

    if(data.size() == size+1) {
        /* This should be obvious from the first if, but add this just to make
           Clang Analyzer happy */
        CORRADE_INTERNAL_ASSERT(extra);

        *extra = std::stof(data.back());
    }

    return output;
}

template<class T> void reindex(const std::vector<UnsignedInt>& indices, std::vector<T>& data) {
    /* Check that indices are in range */
    for(UnsignedInt i: indices) if(i >= data.size()) {
        Error() << "Trade::ObjImporter::mesh3D(): index out of range";
        throw 0;
    }

    data = MeshTools::duplicate(indices, data);
}

}

ObjImporter::ObjImporter() = default;

ObjImporter::ObjImporter(PluginManager::AbstractManager& manager, const std::string& plugin): AbstractImporter{manager, plugin} {}

ObjImporter::~ObjImporter() = default;

auto ObjImporter::doFeatures() const -> Features { return Feature::OpenData; }

void ObjImporter::doClose() { _file.reset(); }

bool ObjImporter::doIsOpened() const { return !!_file; }

void ObjImporter::doOpenFile(const std::string& filename) {
    Containers::Pointer<std::istream> in{new std::ifstream{filename, std::ios::binary}};
    if(!in->good()) {
        Error() << "Trade::ObjImporter::openFile(): cannot open file" << filename;
        return;
    }

    _file.reset(new File);
    _file->in = std::move(in);
    parseMeshNames();
}

void ObjImporter::doOpenData(Containers::ArrayView<const char> data) {
    _file.reset(new File);
    _file->in.reset(new std::istringstream{{data.begin(), data.size()}});

    parseMeshNames();
}

void ObjImporter::parseMeshNames() {
    /* First mesh starts at the beginning, its indices start from 1. The end
       offset will be updated to proper value later. */
    UnsignedInt positionIndexOffset = 1;
    UnsignedInt normalIndexOffset = 1;
    UnsignedInt textureCoordinateIndexOffset = 1;
    _file->meshes.emplace_back(0, 0, positionIndexOffset, normalIndexOffset, textureCoordinateIndexOffset);

    /* The first mesh doesn't have name by default but we might find it later,
       so we need to track whether there are any data before first name */
    bool thisIsFirstMeshAndItHasNoData = true;
    _file->meshNames.emplace_back();

    while(_file->in->good()) {
        /* The previous object might end at the beginning of this line */
        const std::streampos end = _file->in->tellg();

        /* Comment line */
        if(_file->in->peek() == '#') {
            ignoreLine(*_file->in);
            continue;
        }

        /* Parse the keyword */
        std::string keyword;
        *_file->in >> keyword;

        /* Mesh name */
        if(keyword == "o") {
            std::string name;
            std::getline(*_file->in, name);
            name = Utility::String::trim(name);

            /* This is the name of first mesh */
            if(thisIsFirstMeshAndItHasNoData) {
                thisIsFirstMeshAndItHasNoData = false;

                /* Update its name and add it to name map */
                if(!name.empty())
                    _file->meshesForName.emplace(name, _file->meshes.size() - 1);
                _file->meshNames.back() = std::move(name);

                /* Update its begin offset to be more precise */
                std::get<0>(_file->meshes.back()) = _file->in->tellg();

            /* Otherwise this is a name of new mesh */
            } else {
                /* Set end of the previous one */
                std::get<1>(_file->meshes.back()) = end;

                /* Save name and offset of the new one. The end offset will be
                   updated later. */
                if(!name.empty())
                    _file->meshesForName.emplace(name, _file->meshes.size());
                _file->meshNames.emplace_back(std::move(name));
                _file->meshes.emplace_back(_file->in->tellg(), 0, positionIndexOffset, textureCoordinateIndexOffset, normalIndexOffset);
            }

            continue;

        /* If there are any data/indices before the first name, it means that
           the first object is unnamed. We need to check for them. */

        /* Vertex data, update index offset for the following meshes */
        } else if(keyword == "v") {
            ++positionIndexOffset;
            thisIsFirstMeshAndItHasNoData = false;
        } else if(keyword == "vt") {
            ++textureCoordinateIndexOffset;
            thisIsFirstMeshAndItHasNoData = false;
        } else if(keyword == "vn") {
            ++normalIndexOffset;
            thisIsFirstMeshAndItHasNoData = false;

        /* Index data, just mark that we found something for first unnamed
           object */
        } else if(thisIsFirstMeshAndItHasNoData) for(const std::string& data: {"p", "l", "f"}) {
            if(keyword == data) {
                thisIsFirstMeshAndItHasNoData = false;
                break;
            }
        }

        /* Ignore the rest of the line */
        ignoreLine(*_file->in);
    }

    /* Set end of the last object */
    _file->in->clear();
    _file->in->seekg(0, std::ios::end);
    std::get<1>(_file->meshes.back()) = _file->in->tellg();
}

UnsignedInt ObjImporter::doMesh3DCount() const { return _file->meshes.size(); }

Int ObjImporter::doMesh3DForName(const std::string& name) {
    const auto it = _file->meshesForName.find(name);
    return it == _file->meshesForName.end() ? -1 : it->second;
}

std::string ObjImporter::doMesh3DName(UnsignedInt id) {
    return _file->meshNames[id];
}

Containers::Optional<MeshData3D> ObjImporter::doMesh3D(UnsignedInt id) {
    /* Seek the file, set mesh parsing parameters */
    std::streampos begin, end;
    UnsignedInt positionIndexOffset, textureCoordinateIndexOffset, normalIndexOffset;
    std::tie(begin, end, positionIndexOffset, textureCoordinateIndexOffset, normalIndexOffset) = _file->meshes[id];
    _file->in->seekg(begin);

    Containers::Optional<MeshPrimitive> primitive;
    std::vector<Vector3> positions;
    std::vector<std::vector<Vector2>> textureCoordinates;
    std::vector<std::vector<Vector3>> normals;
    std::vector<UnsignedInt> positionIndices;
    std::vector<UnsignedInt> textureCoordinateIndices;
    std::vector<UnsignedInt> normalIndices;

    try { while(_file->in->good() && _file->in->tellg() < end) {
        /* Ignore comments */
        if(_file->in->peek() == '#') {
            ignoreLine(*_file->in);
            continue;
        }

        /* Get the line */
        std::string line;
        std::getline(*_file->in, line);
        line = Utility::String::trim(line);

        /* Ignore empty lines */
        if(line.empty()) continue;

        /* Split the line into keyword and contents */
        const std::size_t keywordEnd = line.find(' ');
        const std::string keyword = line.substr(0, keywordEnd);
        const std::string contents = keywordEnd != std::string::npos ?
            Utility::String::ltrim(line.substr(keywordEnd+1)) : "";

        /* Vertex position */
        if(keyword == "v") {
            Float extra{1.0f};
            const Vector3 data = extractFloatData<3>(contents, &extra);
            if(!Math::TypeTraits<Float>::equals(extra, 1.0f)) {
                Error() << "Trade::ObjImporter::mesh3D(): homogeneous coordinates are not supported";
                return Containers::NullOpt;
            }

            positions.push_back(data);

        /* Texture coordinate */
        } else if(keyword == "vt") {
            Float extra{0.0f};
            const auto data = extractFloatData<2>(contents, &extra);
            if(!Math::TypeTraits<Float>::equals(extra, 0.0f)) {
                Error() << "Trade::ObjImporter::mesh3D(): 3D texture coordinates are not supported";
                return Containers::NullOpt;
            }

            if(textureCoordinates.empty()) textureCoordinates.emplace_back();
            textureCoordinates.front().emplace_back(data);

        /* Normal */
        } else if(keyword == "vn") {
            if(normals.empty()) normals.emplace_back();
            normals.front().emplace_back(extractFloatData<3>(contents));

        /* Indices */
        } else if(keyword == "p" || keyword == "l" || keyword == "f") {
            const std::vector<std::string> indexTuples = Utility::String::splitWithoutEmptyParts(contents, ' ');

            /* Points */
            if(keyword == "p") {
                /* Check that we don't mix the primitives in one mesh */
                if(primitive && primitive != MeshPrimitive::Points) {
                    Error() << "Trade::ObjImporter::mesh3D(): mixed primitive" << *primitive << "and" << MeshPrimitive::Points;
                    return Containers::NullOpt;
                }

                /* Check vertex count per primitive */
                if(indexTuples.size() != 1) {
                    Error() << "Trade::ObjImporter::mesh3D(): wrong index count for point";
                    return Containers::NullOpt;
                }

                primitive = MeshPrimitive::Points;

            /* Lines */
            } else if(keyword == "l") {
                /* Check that we don't mix the primitives in one mesh */
                if(primitive && primitive != MeshPrimitive::Lines) {
                    Error() << "Trade::ObjImporter::mesh3D(): mixed primitive" << *primitive << "and" << MeshPrimitive::Lines;
                    return Containers::NullOpt;
                }

                /* Check vertex count per primitive */
                if(indexTuples.size() != 2) {
                    Error() << "Trade::ObjImporter::mesh3D(): wrong index count for line";
                    return Containers::NullOpt;
                }

                primitive = MeshPrimitive::Lines;

            /* Faces */
            } else if(keyword == "f") {
                /* Check that we don't mix the primitives in one mesh */
                if(primitive && primitive != MeshPrimitive::Triangles) {
                    Error() << "Trade::ObjImporter::mesh3D(): mixed primitive" << *primitive << "and" << MeshPrimitive::Triangles;
                    return Containers::NullOpt;
                }

                /* Check vertex count per primitive */
                if(indexTuples.size() < 3) {
                    Error() << "Trade::ObjImporter::mesh3D(): wrong index count for triangle";
                    return Containers::NullOpt;
                } else if(indexTuples.size() != 3) {
                    Error() << "Trade::ObjImporter::mesh3D(): polygons are not supported";
                    return Containers::NullOpt;
                }

                primitive = MeshPrimitive::Triangles;

            } else CORRADE_ASSERT_UNREACHABLE(); /* LCOV_EXCL_LINE */

            for(const std::string& indexTuple: indexTuples) {
                std::vector<std::string> indices = Utility::String::split(indexTuple, '/');
                if(indices.size() > 3) {
                    Error() << "Trade::ObjImporter::mesh3D(): invalid index data";
                    return Containers::NullOpt;
                }

                /* Position indices */
                positionIndices.push_back(std::stoul(indices[0]) - positionIndexOffset);

                /* Texture coordinates */
                if(indices.size() == 2 || (indices.size() == 3 && !indices[1].empty()))
                    textureCoordinateIndices.push_back(std::stoul(indices[1]) - textureCoordinateIndexOffset);

                /* Normal indices */
                if(indices.size() == 3)
                    normalIndices.push_back(std::stoul(indices[2]) - normalIndexOffset);
            }

        /* Ignore unsupported keywords, error out on unknown keywords */
        } else if(![&keyword](){
            /* Using lambda to emulate for-else construct like in Python */
            for(const std::string expected: {"mtllib", "usemtl", "g", "s"})
                if(keyword == expected) return true;
            return false;
        }()) {
            Error() << "Trade::ObjImporter::mesh3D(): unknown keyword" << keyword;
            return Containers::NullOpt;
        }

    }} catch(const std::exception&) {
        Error() << "Trade::ObjImporter::mesh3D(): error while converting numeric data";
        return Containers::NullOpt;
    } catch(...) {
        /* Error message already printed */
        return Containers::NullOpt;
    }

    /* There should be at least indexed position data */
    if(positions.empty() || positionIndices.empty()) {
        Error() << "Trade::ObjImporter::mesh3D(): incomplete position data";
        return Containers::NullOpt;
    }

    /* If there are index data, there should be also vertex data (and also the other way) */
    if(normals.empty() != normalIndices.empty()) {
        Error() << "Trade::ObjImporter::mesh3D(): incomplete normal data";
        return Containers::NullOpt;
    }
    if(textureCoordinates.empty() != textureCoordinateIndices.empty()) {
        Error() << "Trade::ObjImporter::mesh3D(): incomplete texture coordinate data";
        return Containers::NullOpt;
    }

    /* All index arrays should have the same length */
    if(!normalIndices.empty() && normalIndices.size() != positionIndices.size()) {
        CORRADE_INTERNAL_ASSERT(normalIndices.size() < positionIndices.size());
        Error() << "Trade::ObjImporter::mesh3D(): some normal indices are missing";
        return Containers::NullOpt;
    }
    if(!textureCoordinates.empty() && textureCoordinateIndices.size() != positionIndices.size()) {
        CORRADE_INTERNAL_ASSERT(textureCoordinateIndices.size() < positionIndices.size());
        Error() << "Trade::ObjImporter::mesh3D(): some texture coordinate indices are missing";
        return Containers::NullOpt;
    }

    /* Merge index arrays, if there aren't just the positions */
    std::vector<UnsignedInt> indices;
    if(!normalIndices.empty() || !textureCoordinateIndices.empty()) {
        std::vector<std::reference_wrapper<std::vector<UnsignedInt>>> arrays;
        arrays.reserve(3);
        arrays.emplace_back(positionIndices);
        if(!normalIndices.empty()) arrays.emplace_back(normalIndices);
        if(!textureCoordinateIndices.empty()) arrays.emplace_back(textureCoordinateIndices);
        indices = MeshTools::combineIndexArrays(arrays);

        /* Reindex data arrays */
        try {
            reindex(positionIndices, positions);
            if(!normalIndices.empty()) reindex(normalIndices, normals.front());
            if(!textureCoordinateIndices.empty()) reindex(textureCoordinateIndices, textureCoordinates.front());
        } catch(...) {
            /* Error message already printed */
            return Containers::NullOpt;
        }

    /* Otherwise just use the original position index array. Don't forget to
       check range */
    } else {
        indices = std::move(positionIndices);
        for(UnsignedInt i: indices) if(i >= positions.size()) {
            Error() << "Trade::ObjImporter::mesh3D(): index out of range";
            return Containers::NullOpt;
        }
    }

    return MeshData3D{*primitive, std::move(indices), {std::move(positions)}, std::move(normals), std::move(textureCoordinates), {}, nullptr};
}

}}

CORRADE_PLUGIN_REGISTER(ObjImporter, Magnum::Trade::ObjImporter,
    "cz.mosra.magnum.Trade.AbstractImporter/0.3")
