/*
 * DownData.h
 *
 *  Created on: Nov 15, 2017
 *      Author: vince
 */

#ifndef DOWNDATA_H_
#define DOWNDATA_H_

#include "veins/modules/heterogeneous/messages/HeterogeneousMessage_m.h"

namespace Veins {

class DownData {
public:
    static HeterogeneousMessage* prepareHM(std::string name, int networkType, std::string source, std::string destination,
            simtime_t sendTime, int psid=0, long lengthByte=10, std::string data="");
};

} /* namespace Veins */

#endif /* DOWNDATA_H_ */
