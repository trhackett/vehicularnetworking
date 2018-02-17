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

ChunkMsgData::~ChunkMsgData() {
    // TODO Auto-generated destructor stub
}

ChunkMsgData::ChunkMsgData(int msgTypeIn, int senderTypeIn, int seqnoIn, std::string dataIn) :
        msgType(msgTypeIn),
        senderType(senderTypeIn),
        seqno(seqnoIn),
        data(dataIn)
{}

ChunkMsgData::ChunkMsgData(std::string msgString) {
    std::vector<std::string> words;
    boost::split(words, msgString, boost::is_any_of(" "), boost::token_compress_on);

    std::vector<std::string>::iterator iter = words.begin();

    msgType = atoi((*iter++).c_str());
    senderType = atoi((*iter++).c_str());
    seqno = atoi((*iter++).c_str());
    data = *iter;
}

std::string ChunkMsgData::toString() {
    std::stringstream sstream;
    sstream << msgType << " " << senderType << " " << seqno << " " << data;
    return sstream.str();
}

void ChunkMsgData::fromString(std::string msgString) {
    std::vector<std::string> words;
    boost::split(words, msgString, boost::is_any_of(" "), boost::token_compress_on);

    std::vector<std::string>::iterator iter = words.begin();

    msgType = atoi((*iter++).c_str());
    senderType = atoi((*iter++).c_str());
    seqno = atoi((*iter++).c_str());
    data = *iter;
}

int ChunkMsgData::getMsgType() {
    return msgType;
}

int ChunkMsgData::getSenderType() {
    return senderType;
}

int ChunkMsgData::getSeqno() {
    return seqno;
}

std::string ChunkMsgData::getData() {
    return data;
}

void ChunkMsgData::setMsgType(int msgTypeIn) {
    msgType = msgTypeIn;
}

void ChunkMsgData::setSenderType(int senderTypeIn) {
    senderType = senderTypeIn;
}

void ChunkMsgData::setSeqno(int seqnoIn) {
    seqno = seqnoIn;
}

void ChunkMsgData::setData(std::string dataIn) {
    data = dataIn;
}
