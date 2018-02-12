/*
 * DownData.cc
 *
 *  Created on: Nov 15, 2017
 *      Author: vince
 */

#include "DownData.h"

namespace Veins {

HeterogeneousMessage* DownData::prepareHM(std::string name, int networkType, std::string source, std::string destination,
        simtime_t sendTime, int psid, long lengthByte, std::string data)
{
    HeterogeneousMessage* hmsg = new HeterogeneousMessage(name.c_str());
    hmsg->setNetworkType(networkType);
    hmsg->setSendingTime(sendTime);
    hmsg->setSourceAddress(source.c_str());
    hmsg->setDestinationAddress(destination.c_str());
    hmsg->setPsid(psid);
    hmsg->setByteLength(lengthByte);
    hmsg->setWsmData(data.c_str());

    return hmsg;
}


} /* namespace Veins */
