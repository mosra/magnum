/*
    Copyright © 2010, 2011, 2012 Vladimír Vondruš <mosra@centrum.cz>

    This file is part of Magnum.

    Magnum is free software: you can redistribute it and/or modify
    it under the terms of the GNU Lesser General Public License version 3
    only, as published by the Free Software Foundation.

    Magnum is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
    GNU Lesser General Public License version 3 for more details.
*/

#include "CompressIndices.h"

#include <cstring>
#include <vector>
#include <algorithm>

#include "IndexedMesh.h"
#include "SizeTraits.h"

using namespace std;

namespace Magnum { namespace MeshTools {

#ifndef DOXYGEN_GENERATING_OUTPUT
namespace Implementation {

std::tuple<size_t, Type, char* > CompressIndices::operator()() const {
    return SizeBasedCall<Compressor>(*std::max_element(indices.begin(), indices.end()))(indices);
}

void CompressIndices::operator()(IndexedMesh* mesh, Buffer::Usage usage) const {
    size_t indexCount;
    Type indexType;
    char* data;
    std::tie(indexCount, indexType, data) = operator()();

    mesh->setIndexType(indexType);
    mesh->setIndexCount(indices.size());
    mesh->indexBuffer()->setData(indexCount*TypeInfo::sizeOf(indexType), data, usage);

    delete[] data;
}

template<class IndexType> std::tuple<size_t, Type, char*> CompressIndices::Compressor::run(const std::vector<unsigned int>& indices) {
    /* Create smallest possible version of index buffer */
    char* buffer = new char[indices.size()*sizeof(IndexType)];
    for(size_t i = 0; i != indices.size(); ++i) {
        IndexType index = indices[i];
        memcpy(buffer+i*sizeof(IndexType), reinterpret_cast<const char*>(&index), sizeof(IndexType));
    }

    return std::make_tuple(indices.size(), TypeTraits<IndexType>::indexType(), buffer);
}

}
#endif

}}
