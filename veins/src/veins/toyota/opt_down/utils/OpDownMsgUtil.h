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

#ifndef OPDOWNMSGUTIL_H_
#define OPDOWNMSGUTIL_H_

#include "veins/modules/heterogeneous/messages/HeterogeneousMessage_m.h"

class OpDownMsgUtil {

public:
    static HeterogeneousMessage* prepareHM(std::string name, std::string sourceAddress, std::string destinationAddress, int networkType,int dataLengthBits,int msgType=0, int serial=0);
};

#endif /* OPDOWNMSGUTIL_H_ */