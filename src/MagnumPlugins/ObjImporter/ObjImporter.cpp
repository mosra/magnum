/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017, 2018, 2019,
                2020, 2021, 2022, 2023, 2024, 2025, 2026
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

#include <climits> /* INT_MIN, INT_MAX */
#include <unordered_map>
#include <Corrade/Containers/GrowableArray.h>
#include <Corrade/Containers/Optional.h>
#include <Corrade/Containers/StridedArrayView.h>
#include <Corrade/Containers/String.h>
#include <Corrade/Containers/StringStlHash.h>
#include <Corrade/Utility/Algorithms.h>
#include <Corrade/Utility/ConfigurationGroup.h>

#include "Magnum/Mesh.h"
#include "Magnum/MeshTools/RemoveDuplicates.h"
#include "Magnum/MeshTools/Duplicate.h"
#include "Magnum/Math/Color.h"
#include "Magnum/Trade/MeshData.h"

namespace Magnum { namespace Trade {

using namespace Containers::Literals;

namespace {

struct Mesh {
    /* Points to File::fileData, the end is implicitly `(this + 1)->begin` */
    const char* begin;
    /* Name of the mesh. The first mesh has a name only if it appears before
       any data line (v/vt/vn/... or p/l/f/...).  */
    Containers::StringView name;
    /* Offset of the first position, texture coordinate and normal index.
       Assuming that not only vertex data but also index data follow the mesh
       name, this way we don't have to parse the whole file if just a single
       mesh out of many is requested. */
    UnsignedInt positionIndexOffset;
    UnsignedInt textureCoordinateIndexOffset;
    UnsignedInt normalIndexOffset;
};

}

struct ObjImporter::File {
    std::unordered_map<Containers::StringView, UnsignedInt> meshesForName;
    /* Contains always n + 1 entries, with the last entry being an upper bound
       on the file range and index offsets */
    Containers::Array<Mesh> meshes;
    Containers::Array<char> fileData;
};

namespace {

/* The spec doesn't say anything explicit about whitespace, but at the very
   least the examples at http://paulbourke.net/dataformats/obj/ pad the values
   with whitespace and various tools are also producing such files, as reported
   at https://forum.babylonjs.com/t/extra-whitespace-breaks-obj-parsing/5244

   Besides space I'm considering a tab and a CR character. *Not* a newline,
   since that is a significant delimiter that has to be treated separately. */
constexpr Containers::StringView Whitespace = " \t\r"_s;

/* Mostly just a copy of Corrade's Utility::Json::parseFloatInternal() and
   parseUnsignedIntInternal() */
/** @todo make a common API in Corrade once we have something that can parse
    numbers like a grownup */
inline bool parseFloat(const char* const errorPrefix, const Containers::StringView string, Float& out) {
    /** @todo replace with something that can parse non-null-terminated stuff,
        then drop this "too long" error */
    char buffer[128];
    const std::size_t size = string.size();
    if(size > Containers::arraySize(buffer) - 1) {
        Error{} << errorPrefix << "too long numeric literal" << string;
        return false;
    }

    std::memcpy(buffer, string.data(), size);
    buffer[size] = '\0';
    char* end;
    out = std::strtof(buffer, &end);
    if(!string || std::size_t(end - buffer) != size) {
        Error{} << errorPrefix << "invalid floating-point literal" << string;
        return false;
    }

    /* Success; value already written above */
    return true;
}

inline bool parseInt(const char* const errorPrefix, const Containers::StringView string, Int& out) {
    /** @todo replace with something that can parse non-null-terminated stuff,
        then drop this "too long" error */
    char buffer[128];
    const std::size_t size = string.size();
    if(size > Containers::arraySize(buffer) - 1) {
        Error{} << errorPrefix << "too long numeric literal" << string;
        return false;
    }

    std::memcpy(buffer, string.data(), size);
    buffer[size] = '\0';
    char* end;
    /* Not using strtol() here as on Windows it's 32-bit and we would have to
       additionally check errno to detect overflows */
    const std::int64_t outLong = std::strtoll(buffer, &end, 10);
    if(!string || std::size_t(end - buffer) != size) {
        Error{} << errorPrefix << "invalid integer literal" << string;
        return false;
    }
    if(outLong < INT_MIN || outLong > INT_MAX) {
        Error{} << errorPrefix << "too small or large integer literal" << string;
        return false;
    }

    /* On success convert the value to 32 bits */
    out = outLong;
    return true;
}

}

#ifdef MAGNUM_BUILD_DEPRECATED
ObjImporter::ObjImporter() = default; /* LCOV_EXCL_LINE */
#endif

ObjImporter::ObjImporter(PluginManager::AbstractManager& manager, const Containers::StringView& plugin): AbstractImporter{manager, plugin} {}

ObjImporter::~ObjImporter() = default;

ImporterFeatures ObjImporter::doFeatures() const { return ImporterFeature::OpenData; }

void ObjImporter::doClose() {
    _file = {};
}

bool ObjImporter::doIsOpened() const { return !!_file; }

void ObjImporter::doOpenData(Containers::Array<char>&& data, const DataFlags dataFlags) {
    _file.emplace();

    /* Copy file content. Take over the existing array or copy the data if we
       can't. We need to keep the data around as JSON tokens are views onto it
       and also for the GLB binary chunk. */
    if(dataFlags & (DataFlag::Owned|DataFlag::ExternallyOwned)) {
        _file->fileData = Utility::move(data);
    } else {
        _file->fileData = Containers::Array<char>{NoInit, data.size()};
        Utility::copy(data, _file->fileData);
    }

    /* First mesh starts at the beginning, its indices start from 1 */
    UnsignedInt positionIndexOffset = 1;
    UnsignedInt normalIndexOffset = 1;
    UnsignedInt textureCoordinateIndexOffset = 1;
    arrayAppend(_file->meshes, InPlaceInit, _file->fileData.begin(), Containers::StringView{}, positionIndexOffset, textureCoordinateIndexOffset, normalIndexOffset);

    /* The first mesh doesn't have name by default but we might find it later,
       so we need to track whether there are any data before first name */
    bool thisIsFirstMeshAndItHasNoData = true;

    /** @todo check size < 1G on 32b? currently it'd just assert, but it's
        unlikely that such amount of contiguous memory would even be available
        there, so ¯\_(ツ)_/¯ */
    Containers::StringView in = _file->fileData;
    while(in) {
        /* Get a (trimmed) line from the input */
        const Containers::StringView lineEnd = in.findOr('\n', in.end());
        const Containers::StringView line = in.prefix(lineEnd.begin()).trimmed(Whitespace);
        in = in.suffix(lineEnd.end());

        /* Comment or empty line, skip */
        if(!line || line.hasPrefix('#'))
            continue;

        /* Parse the keyword */
        const Containers::StringView keywordEnd = line.findAnyOr(Whitespace, line.end());
        const Containers::StringView keyword = line.prefix(keywordEnd.begin());

        /* Mesh name */
        if(keyword == "o"_s) {
            const Containers::StringView name = line.suffix(keywordEnd.end()).trimmed(Whitespace);

            /* This is the name of first mesh */
            if(thisIsFirstMeshAndItHasNoData) {
                thisIsFirstMeshAndItHasNoData = false;

                /* Update its name and add it to name map */
                if(name)
                    _file->meshesForName.emplace(name, _file->meshes.size() - 1);
                _file->meshes.back().name = name;

            /* Otherwise this is a name of new mesh */
            } else {
                /* Save name and offset of the new one. The end offset will be
                   updated later. */
                if(name)
                    _file->meshesForName.emplace(name, _file->meshes.size());
                arrayAppend(_file->meshes, InPlaceInit, in.begin(), name, positionIndexOffset, textureCoordinateIndexOffset, normalIndexOffset);
            }

            continue;

        /* If there are any data/indices before the first name, it means that
           the first object is unnamed. We need to check for them. */

        /* Vertex data, update index offset for the following meshes */
        } else if(keyword == "v"_s) {
            ++positionIndexOffset;
            thisIsFirstMeshAndItHasNoData = false;
        } else if(keyword == "vt"_s) {
            ++textureCoordinateIndexOffset;
            thisIsFirstMeshAndItHasNoData = false;
        } else if(keyword == "vn"_s) {
            ++normalIndexOffset;
            thisIsFirstMeshAndItHasNoData = false;

        /* Index data, just mark that we found something for first unnamed
           object */
        } else if(keyword == "p"_s ||
                  keyword == "l"_s ||
                  keyword == "f"_s) {
            thisIsFirstMeshAndItHasNoData = false;
        }
    }

    /* Save the final offset so we have an upper bound on index offsets */
    arrayAppend(_file->meshes, InPlaceInit, in.begin(), Containers::StringView{}, positionIndexOffset, textureCoordinateIndexOffset, normalIndexOffset);
}

UnsignedInt ObjImporter::doMeshCount() const {
    /* There's always one more item for an upper bound */
    return _file->meshes.size() - 1;
}

Int ObjImporter::doMeshForName(const Containers::StringView name) {
    const auto it = _file->meshesForName.find(name);
    return it == _file->meshesForName.end() ? -1 : it->second;
}

Containers::String ObjImporter::doMeshName(const UnsignedInt id) {
    return _file->meshes[id].name;
}

namespace {

template<class T> bool checkAndDuplicateInto(const Containers::StridedArrayView1D<const UnsignedInt>& indices, const Containers::Array<T>& data, const Containers::StridedArrayView1D<T>& out, UnsignedInt offset) {
    /* Check that indices are in range. Add back the original index offset for
       easier data debugging. */
    for(UnsignedInt i: indices) if(i >= data.size()) {
        Error{} << "Trade::ObjImporter::mesh(): index" << (i + offset) << "out of range for" << data.size() << "vertices";
        return false;
    }

    MeshTools::duplicateInto(indices, stridedArrayView(data), out);
    return true;
}

}

Containers::Optional<MeshData> ObjImporter::doMesh(const UnsignedInt id, UnsignedInt) {
    /* Seek the file, set mesh parsing parameters */
    const Mesh& mesh = _file->meshes[id];

    Containers::Optional<MeshPrimitive> primitive;
    Containers::Array<Vector3> positions;
    Containers::Array<Vector2> textureCoordinates;
    Containers::Array<Vector3> normals;
    /* Taking a shortcut as there's fortunately nothing else than just 3 types
       of data. First positions, then normals, then texture coordinates. */
    Containers::Array<Vector3ui> indices;
    std::size_t textureCoordinateIndexCount = 0, normalIndexCount = 0;

    Containers::StringView in{mesh.begin, std::size_t(_file->meshes[id + 1].begin - mesh.begin)};
    while(in) {
        /* Get a line from the input */
        const Containers::StringView lineEnd = in.findOr('\n', in.end());
        const Containers::StringView line = in.prefix(lineEnd.begin()).trimmed(Whitespace);
        in = in.suffix(lineEnd.end());

        /* Comment or empty line, skip */
        if(!line || line.hasPrefix('#'))
            continue;

        /* Parse the keyword */
        const Containers::StringView keywordEnd = line.findAnyOr(Whitespace, line.end());
        const Containers::StringView keyword = line.prefix(keywordEnd.begin());

        /* Skip keywords that are not interesting to us or that were parsed
           earlier. In particular, the `o` can be here because the mesh range
           is everything until the next mesh data start, so including the next mesh name. */
        if(keyword == "o"_s ||
           keyword == "g"_s ||
           keyword == "s"_s ||
           keyword == "mtllib"_s ||
           keyword == "usemtl"_s)
            continue;

        /* Keyword contents */
        Containers::StringView contents = line.suffix(keywordEnd.end()).trimmedPrefix(Whitespace);

        /* Vertex data */
        if(keyword == "v"_s || keyword == "vt"_s || keyword == "vn"_s) {
            /* Decide on how many components we expect at most. There's
               optional behavior for four-component positions and
               three-component texture coordinates, so it can't be an exact
               count. */
            std::size_t maxComponentCount;
            if(keyword == "v"_s)
                maxComponentCount = 4;
            else if(keyword == "vt"_s)
                maxComponentCount = 3;
            else if(keyword == "vn"_s)
                maxComponentCount = 3;
            else CORRADE_INTERNAL_ASSERT_UNREACHABLE(); /* LCOV_EXCL_LINE */

            /* Parse them all. If there's less than expected, `i` would be too
               small; if there's more then `contents` would stay non-empty. */
            Float data[4];
            std::size_t i = 0;
            for(; i != maxComponentCount && contents; ++i) {
                const Containers::StringView foundSpace = contents.findAnyOr(Whitespace, contents.end());

                if(!parseFloat("Trade::ObjImporter::mesh():", contents.prefix(foundSpace.begin()), data[i]))
                    return {};

                contents = contents.suffix(foundSpace.end()).trimmedPrefix(Whitespace);
            }

            /* Position */
            if(keyword == "v"_s) {
                if(i < 3 || contents) {
                    Error{} << "Trade::ObjImporter::mesh(): expected 3 or 4 position coordinates, got" << line.suffix(keywordEnd.end());
                    return {};
                }
                if(i == 4 && !Math::equal(data[3], 1.0f)) {
                    Error{} << "Trade::ObjImporter::mesh(): homogeneous coordinates are not supported";
                    return {};
                }

                arrayAppend(positions, Vector3::from(data));

            /* Texture coordinate */
            } else if(keyword == "vt"_s) {
                if(i < 2 || contents) {
                    Error{} << "Trade::ObjImporter::mesh(): expected 2 or 3 texture coordinates, got" << line.suffix(keywordEnd.end());
                    return {};
                }
                if(i == 3 && !Math::equal(data[2], 0.0f)) {
                    Error{} << "Trade::ObjImporter::mesh(): 3D texture coordinates are not supported";
                    return {};
                }

                arrayAppend(textureCoordinates, Vector2::from(data));

            /* Normal */
            } else if(keyword == "vn"_s) {
                if(i < 3 || contents) {
                    Error{} << "Trade::ObjImporter::mesh(): expected 3 normal coordinates, got" << line.suffix(keywordEnd.end());
                    return {};
                }

                arrayAppend(normals, Vector3::from(data));

            } else CORRADE_INTERNAL_ASSERT_UNREACHABLE(); /* LCOV_EXCL_LINE */

        /* Indices */
        } else if(keyword == "p"_s || keyword == "l"_s || keyword == "f"_s) {
            /* Decide on how many tuples we expect. Since we handle both
               triangles and quads, it can't be an exact count. */
            std::size_t maxIndexTupleCount;
            if(keyword == "p"_s)
                maxIndexTupleCount = 1;
            else if(keyword == "l"_s)
                maxIndexTupleCount = 2;
            else if(keyword == "f"_s)
                maxIndexTupleCount = 4;
            else CORRADE_INTERNAL_ASSERT_UNREACHABLE(); /* LCOV_EXCL_LINE */

            /* Parse them all. If there's less than expected, `i` would be too
               small; if there's more then `contents` would stay non-empty. */
            Vector3ui data[4];
            std::size_t i = 0;
            for(; i != maxIndexTupleCount && contents; ++i) {
                const Containers::StringView foundSpace = contents.findAnyOr(Whitespace, contents.end());
                Containers::StringView indexTuple = contents.prefix(foundSpace.begin());

                /* The number before first slash is a position index */
                const Containers::StringView foundSlash1 = indexTuple.findOr('/', indexTuple.end());
                Int index;
                if(!parseInt("Trade::ObjImporter::mesh():", indexTuple.prefix(foundSlash1.begin()), index))
                    return {};
                /* If the number is negative, it counts from the end (-1 is
                   the last known position at this point, counting from 1) */
                if(index < 0)
                    index += positions.size() + 1;
                data[i][0] = index - mesh.positionIndexOffset;

                /* If there was a slash, next is a texture coordinate or
                   empty */
                if(foundSlash1) {
                    indexTuple = indexTuple.suffix(foundSlash1.end());
                    const Containers::StringView foundSlash2 = indexTuple.findOr('/', indexTuple.end());
                    if(!foundSlash2 || foundSlash2.begin() != indexTuple.begin()) {
                        if(!parseInt("Trade::ObjImporter::mesh():", indexTuple.prefix(foundSlash2.begin()), index))
                            return {};
                        /* If the number is negative, it counts from the end
                           (-1 is the last known texture coordinate at this
                           point, counting from 1) */
                        if(index < 0)
                            index += textureCoordinates.size() + 1;
                        data[i][1] = index - mesh.textureCoordinateIndexOffset;
                        ++textureCoordinateIndexCount;
                    }

                    /* If there was a second slash, last is a normal */
                    if(foundSlash2) {
                        indexTuple = indexTuple.suffix(foundSlash2.end());
                        if(!parseInt("Trade::ObjImporter::mesh():", indexTuple, index))
                            return {};
                        /* If the number is negative, it counts from the end
                           (-1 is the last known normal at this point, counting
                           from 1) */
                        if(index < 0)
                            index += normals.size() + 1;
                        data[i][2] = index - mesh.normalIndexOffset;
                        ++normalIndexCount;
                    }
                }

                contents = contents.suffix(foundSpace.end()).trimmedPrefix(Whitespace);
            }

            /* Points */
            if(keyword == "p") {
                if(primitive && primitive != MeshPrimitive::Points) {
                    Error() << "Trade::ObjImporter::mesh(): mixed primitive" << *primitive << "and" << MeshPrimitive::Points;
                    return {};
                }
                if(i < 1 || contents) {
                    Error() << "Trade::ObjImporter::mesh(): expected exactly 1 position index tuple for a point, got" << line.suffix(keywordEnd.end());
                    return {};
                }

                primitive = MeshPrimitive::Points;
                arrayAppend(indices, Containers::arrayView(data).prefix(1));

            /* Lines */
            } else if(keyword == "l") {
                if(primitive && primitive != MeshPrimitive::Lines) {
                    Error() << "Trade::ObjImporter::mesh(): mixed primitive" << *primitive << "and" << MeshPrimitive::Lines;
                    return {};
                }
                if(i < 2 || contents) {
                    Error() << "Trade::ObjImporter::mesh(): expected exactly 2 position index tuples for a line, got" << line.suffix(keywordEnd.end());
                    return {};
                }

                primitive = MeshPrimitive::Lines;
                arrayAppend(indices, Containers::arrayView(data).prefix(2));

            /* Faces */
            } else if(keyword == "f") {
                if(primitive && primitive != MeshPrimitive::Triangles) {
                    Error() << "Trade::ObjImporter::mesh(): mixed primitive" << *primitive << "and" << MeshPrimitive::Triangles;
                    return {};
                }
                if(i < 3 || contents) {
                    Error() << "Trade::ObjImporter::mesh(): expected 3 or 4 position index tuples for a face, got" << line.suffix(keywordEnd.end());
                    return {};
                }

                /* If it's a quad, convert it to two triangles */
                if(i == 4) {
                    /** @todo use MeshTools::generateQuadIndices() once it
                        can take extra index data into account */
                    /* 0 0---3
                       |\ \  |
                       | \ \ |
                       |  \ \|
                       1---2 2 */
                    arrayAppend(indices, {
                        data[0],
                        data[1],
                        data[2],
                        data[0],
                        data[2],
                        data[3]
                    });

                    /* If we have texture coordinate / normal indices, add two
                       more to the counters as well. If they matched the index
                       array size before, they'll continue to match; if they
                       didn't, they'll continue to not match. */
                    if(textureCoordinateIndexCount)
                        textureCoordinateIndexCount += 2;
                    if(normalIndexCount)
                        normalIndexCount += 2;
                } else arrayAppend(indices, Containers::arrayView(data).prefix(3));

                primitive = MeshPrimitive::Triangles;

            } else CORRADE_INTERNAL_ASSERT_UNREACHABLE(); /* LCOV_EXCL_LINE */

        /* Unknown keyword */
        } else {
            Error{} << "Trade::ObjImporter::mesh(): unknown keyword" << keyword;
            return {};
        }
    }

    /* There should be at least indexed position data */
    if(positions.isEmpty() || indices.isEmpty()) {
        Error() << "Trade::ObjImporter::mesh(): incomplete position data";
        return {};
    }

    /* If there are index data, there should be also vertex data (and also the other way) */
    if(textureCoordinates.isEmpty() != (textureCoordinateIndexCount == 0)) {
        Error() << "Trade::ObjImporter::mesh(): incomplete texture coordinate data";
        return {};
    }
    if(normals.isEmpty() != (normalIndexCount == 0)) {
        Error() << "Trade::ObjImporter::mesh(): incomplete normal data";
        return {};
    }

    /* All index arrays should have the same length */
    if(textureCoordinateIndexCount && textureCoordinateIndexCount != indices.size()) {
        CORRADE_INTERNAL_ASSERT(textureCoordinateIndexCount < indices.size());
        Error() << "Trade::ObjImporter::mesh(): some texture coordinate indices are missing";
        return {};
    }
    if(normalIndexCount && normalIndexCount != indices.size()) {
        CORRADE_INTERNAL_ASSERT(normalIndexCount < indices.size());
        Error() << "Trade::ObjImporter::mesh(): some normal indices are missing";
        return {};
    }

    /* Merge index arrays, unless disabled. If any of the attributes was not
       there, the whole index array has zeros, not affecting the uniqueness in
       any way. */
    Containers::Array<char> indexData;
    std::size_t vertexCount;
    if(configuration().value<bool>("mergeIndexArrays")) {
        indexData = Containers::Array<char>{NoInit, indices.size()*sizeof(UnsignedInt)};
        const auto indexDataI = Containers::arrayCast<UnsignedInt>(indexData);
        vertexCount = MeshTools::removeDuplicatesInPlaceInto(
            Containers::arrayCast<2, char>(arrayView(indices)), indexDataI);

    /* If merging was disabled, this behaves like if all index tuples were
       unique. No other change needed. */
    } else vertexCount = indices.size();

    /* Allocate attribute and vertex data */
    std::size_t attributeCount = 1;
    UnsignedInt stride = sizeof(Vector3);
    if(textureCoordinateIndexCount) {
        ++attributeCount;
        stride += sizeof(Vector2);
    }
    if(normalIndexCount) {
        ++attributeCount;
        stride += sizeof(Vector3);
    }
    Containers::Array<MeshAttributeData> attributeData{ValueInit, attributeCount};
    Containers::Array<char> vertexData{NoInit, vertexCount*stride};

    /* Duplicate the vertices into the output */
    const auto indicesPerAttribute = Containers::arrayCast<2, const UnsignedInt>(stridedArrayView(indices)).transposed<0, 1>();
    std::size_t attributeIndex = 0;
    std::size_t offset = 0;
    {
        Containers::StridedArrayView1D<Vector3> view{vertexData,
            reinterpret_cast<Vector3*>(vertexData.data()), vertexCount, stride};
        if(!checkAndDuplicateInto(indicesPerAttribute[0].prefix(vertexCount), positions, view, mesh.positionIndexOffset))
            return {};
        attributeData[attributeIndex++] = MeshAttributeData{MeshAttribute::Position, view};
        offset += sizeof(Vector3);
    }
    if(textureCoordinateIndexCount) {
        Containers::StridedArrayView1D<Vector2> view{vertexData,
            reinterpret_cast<Vector2*>(vertexData.data() + offset), vertexCount, stride};
        if(!checkAndDuplicateInto(indicesPerAttribute[1].prefix(vertexCount), textureCoordinates, view, mesh.textureCoordinateIndexOffset))
            return {};
        attributeData[attributeIndex++] = MeshAttributeData{MeshAttribute::TextureCoordinates, view};
        offset += sizeof(Vector2);
    }
    if(normalIndexCount) {
        Containers::StridedArrayView1D<Vector3> view{vertexData,
            reinterpret_cast<Vector3*>(vertexData.data() + offset), vertexCount, stride};
        if(!checkAndDuplicateInto(indicesPerAttribute[2].prefix(vertexCount), normals, view, mesh.normalIndexOffset))
            return {};
        attributeData[attributeIndex++] = MeshAttributeData{MeshAttribute::Normal, view};
        offset += sizeof(Vector3);
    }
    CORRADE_INTERNAL_ASSERT(offset == stride && attributeIndex == attributeCount);

    /* If mergeIndexArrays was disabled, indexData is nullptr and the mesh is
       not indexed */
    const auto meshIndices = indexData ?
        Trade::MeshIndexData{MeshIndexType::UnsignedInt, indexData} :
        Trade::MeshIndexData{};
    return MeshData{*primitive,
        Utility::move(indexData), meshIndices,
        Utility::move(vertexData), Utility::move(attributeData)};
}

}}

CORRADE_PLUGIN_REGISTER(ObjImporter, Magnum::Trade::ObjImporter,
    MAGNUM_TRADE_ABSTRACTIMPORTER_PLUGIN_INTERFACE)
