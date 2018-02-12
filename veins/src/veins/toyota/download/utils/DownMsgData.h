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

#ifndef DOWNMSGDATA_H_
#define DOWNMSGDATA_H_

#include "veins/toyota/download/utils/DownDefs.h"
#include <omnetpp.h>

typedef std::map<int,std::string> MsgMapType;

class DownMsgData : public cOwnedObject {
public:
    DownMsgData(const char *name=NULL);
    DownMsgData(const DownMsgData& other);
    virtual ~DownMsgData();
    virtual void insertElement(int msgVal, std::string hostName);
    virtual  MsgMapType getMsgMap() const;
    virtual int getMsgMapSize() const;
    virtual DownMsgData* dup()const {return new DownMsgData(*this);}

private:
    void copy (const DownMsgData& other);

private:
    MsgMapType msgMap;

};

#endif /* DOWNMSGDATA_H_ */
