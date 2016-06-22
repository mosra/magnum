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

#include "Magnum/Mesh.h"
#include "Magnum/MeshTools/CombineIndexedArrays.h"
#include "Magnum/MeshTools/Duplicate.h"
#include "Magnum/Math/Color.h"
#include "Magnum/Trade/MeshData3D.h"

#if defined(CORRADE_TARGET_NACL_NEWLIB) || defined(CORRADE_TARGET_ANDROID)
#include <sstream>
#endif

using namespace Corrade::Containers;

namespace Magnum { namespace Trade {

struct ImporterState {
    std::unordered_map<std::string, UnsignedInt> meshesForName;
    std::vector<std::string> meshNames;
    std::vector<std::tuple<std::streampos, std::streampos, UnsignedInt, UnsignedInt, UnsignedInt>> meshes;

    std::optional<MeshPrimitive> primitive;
    std::vector<Vector3> positions;
    std::vector<std::vector<Vector2>> textureCoordinates;
    std::vector<std::vector<Vector3>> normals;
    std::vector<UnsignedInt> positionIndices;
    std::vector<UnsignedInt> textureCoordinateIndices;
    std::vector<UnsignedInt> normalIndices;
};

namespace {

int findNext(const ArrayView<char>& pos, char c) {
    for(int i = 0; i < pos.size(); ++i) {
        if(pos[i] == c) {
            return i;
        }
    }
    return -1;
}

ArrayView<char> skipWhitespaces(const ArrayView<char>& pos) {
    for(int i = 0; i < pos.size(); ++i) {
        if(pos[i] != ' ' && pos[i] != '\t') {
            return pos.suffix(i);
        }
    }
    return ArrayView<char>{};
}

ArrayView<char> ignoreLine(const ArrayView<char>& pos) {
    return pos.suffix(findNext(pos, '\n'));
}

std::tuple<std::string, ArrayView<char>> nextLine(const ArrayView<char>& pos) {
    int i = 0;
    for(; i < pos.size(); ++i) {
        if(pos[i] == '\n') {
            break;
        }
    }
    std::string str = pos.prefix(i).data();
    return std::make_tuple(str, pos.suffix(i+1));
}

std::tuple<std::string, ArrayView<char>> nextWord(const ArrayView<char>& pos) {
    int i = 0;
    for(; i < pos.size(); ++i) {
        if(pos[i] == ' ') {
            break;
        }
    }
    std::string str = pos.prefix(i).data();
    return std::make_tuple(str, pos.suffix(i+1));
}

std::tuple<float, ArrayView<char>> asFloat(const ArrayView<char>& pos) {

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

bool ObjImporter::doIsOpened() const { return _in; }

void ObjImporter::doClose() { _in = nullptr; }

void ObjImporter::doOpenData(Containers::ArrayView<const char> data) {
    _in = Containers::Array<char>{data.size()};
    std::copy(data.begin(), data.end(), _in.begin());
    _state.reset(new ImporterState);
    parseMeshNames();
}

void ObjImporter::parseMeshNames() {
    /* First mesh starts at the beginning, its indices start from 1. The end
       offset will be updated to proper value later. */
    UnsignedInt positionIndexOffset = 1;
    UnsignedInt normalIndexOffset = 1;
    UnsignedInt textureCoordinateIndexOffset = 1;
    _state->meshes.emplace_back(0, 0, positionIndexOffset, normalIndexOffset, textureCoordinateIndexOffset);

    _state->meshNames.emplace_back();

    ArrayView<char> pos = _in;
    while(!pos.empty()) {

        /* Comment line */
        if(pos[0] == '#') {
            pos = ignoreLine(pos);
            continue;
        }

        /* Parse the keyword */
        std::string keyword;
        std::tie(keyword, pos) = nextWord(pos);

        /* Mesh name */
        if(keyword == "o") {
            // TODO
            continue;
        }

        /* Vertex position */
        if(keyword == "v") {
            Float extra{1.0f};
            const Vector3 data = extractFloatData<3>(pos, &extra);
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

        /* Ignore the rest of the line */
        pos = ignoreLine(pos);
    }
}

UnsignedInt ObjImporter::doMesh3DCount() const { return _state->meshes.size(); }

Int ObjImporter::doMesh3DForName(const std::string& name) {
    const auto it = _state->meshesForName.find(name);
    return it == _state->meshesForName.end() ? -1 : it->second;
}

std::string ObjImporter::doMesh3DName(UnsignedInt id) {
    return _state->meshNames[id];
}

std::optional<MeshData3D> ObjImporter::doMesh3D(UnsignedInt id) {
    /* Seek the file, set mesh parsing parameters */
    std::streampos begin, end;
    UnsignedInt positionIndexOffset, textureCoordinateIndexOffset, normalIndexOffset;
    std::tie(begin, end, positionIndexOffset, textureCoordinateIndexOffset, normalIndexOffset) = _state->meshes[id];

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
