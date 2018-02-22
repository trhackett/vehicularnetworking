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

#include <iostream>
#include <sstream>
#include <string>
#include <Coord.h>
#include <boost/algorithm/string.hpp>

#define CMD_MSGTYPE_REQUEST     100
#define CMD_MSGTYPE_DATA        101

#define CMD_SENDERTYPE_SERVER   200
#define CMD_SENDERTYPE_CAR      201

#define CMD_NAME_DATA "Chunk Data"
#define CMD_NAME_REQU "Chunk Request"

class ChunkMsgData {
public:
    ChunkMsgData(int msgTypeIn, int senderTypeIn, int seqnoIn, Coord positionIn, std::string dataIn="");
    ChunkMsgData(std::string msgString);
    virtual ~ChunkMsgData();
    std::string toString();
    void fromString(std::string msgString);

    int getMsgType();
    int getSenderType();
    int getSeqno();
    Coord getPosition();
    std::string getData();

    void setMsgType(int msgTypeIn);
    void setSenderType(int senderTypeIn);
    void setSeqno(int seqnoIn);
    void setPosition(Coord positionIn);
    void setData(std::string dataIn);

private:
    int msgType;
    int senderType;
    int seqno;
    Coord position;
    std::string data;
};

#endif /* CHUNKMSGDATA_H_ */
