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

#include "OpDownMsgUtil.h"

HeterogeneousMessage* OpDownMsgUtil::prepareHM(std::string name, std::string sourceAddress, std::string destinationAddress, int networkType,int dataLengthBits, int msgType, int serial) {
    HeterogeneousMessage *hm = new HeterogeneousMessage(name.c_str());
    hm->addBitLength(dataLengthBits);
    hm->setSourceAddress(sourceAddress.c_str());
    hm->setDestinationAddress(destinationAddress.c_str());
    hm->setNetworkType(networkType);
    hm->setSendingTime(simTime());
    hm->setKind(msgType);
    hm->setSerial(serial);
    return hm;
}
