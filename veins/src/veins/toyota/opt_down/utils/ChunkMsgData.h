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

#ifndef CHUNKMSGDATA_H_
#define CHUNKMSGDATA_H_

#include <omnetpp.h>
#include <vector>

// <chunkId,chunkValue>
typedef std::vector<int> chunkVecType;
#define CMD_MSG_VEC "CHUNK_ID_VEC"

class ChunkMsgData : public cOwnedObject {
public:
    ChunkMsgData(const char *name=CMD_MSG_VEC);
    ChunkMsgData(std::vector<int> chunkIds,const char *name=CMD_MSG_VEC);
    ChunkMsgData(const ChunkMsgData& other);
    virtual ~ChunkMsgData();
    virtual void insertElement(int chunkId);
    virtual chunkVecType getchunkVec() const;
    virtual int getChunkVecSize() const;
    virtual ChunkMsgData* dup()const {return new ChunkMsgData(*this);}

private:
    void copy (const ChunkMsgData& other);

private:
    chunkVecType chunkVec;
};

#endif /* CHUNKMSGDATA_H_ */
