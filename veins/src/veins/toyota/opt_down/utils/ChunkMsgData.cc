//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
// 
// You should have received a copy of the GNU Lesser General Public License
// along with this program.  If not, see http://www.gnu.org/licenses/.
// 

#include "ChunkMsgData.h"

ChunkMsgData::ChunkMsgData(const char* name) {
    setName(name);
}

ChunkMsgData::ChunkMsgData(std::vector<int> chunkIds, const char* name) {
    setName(name);
    chunkVec = chunkIds;
}

ChunkMsgData::ChunkMsgData(const ChunkMsgData& other)
{
    copy(other);
}

void ChunkMsgData::copy(const ChunkMsgData& other){
    setName(other.getName());
    this->chunkVec = other.getchunkVec();
}

ChunkMsgData::~ChunkMsgData() {
}

void ChunkMsgData::insertElement(int chunkId)
{
    chunkVec.push_back(chunkId);
}

chunkVecType ChunkMsgData::getchunkVec() const {
    return chunkVec;
}

int ChunkMsgData::getChunkVecSize() const {
    return chunkVec.size();
}

