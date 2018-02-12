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

#include "DownMsgData.h"

DownMsgData::DownMsgData(const char* name) {
    setName(name);
}

DownMsgData::DownMsgData(const DownMsgData& other)
{
    copy(other);
}

void DownMsgData::copy(const DownMsgData& other){
    setName(other.getName());
    this->msgMap = other.getMsgMap();
}

DownMsgData::~DownMsgData() {
}

void DownMsgData::insertElement(int msgVal, std::string hostName)
{
    msgMap.insert(std::pair<int,std::string>(msgVal,hostName));
}

MsgMapType DownMsgData::getMsgMap() const {
    return msgMap;
}

int DownMsgData::getMsgMapSize() const {
    return msgMap.size();
}


