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

#ifndef VEHOPDOWNAPP_H_
#define VEHOPDOWNAPP_H_

#include <omnetpp.h>
#include "veins/modules/heterogeneous/messages/HeterogeneousMessage_m.h"
#include "veins/modules/mobility/traci/TraCIScenarioManager.h"
#include "veins/modules/mobility/traci/TraCIMobility.h"
#include "veins/base/utils/SimpleLogger.h"
#include "veins/toyota/opt_down/utils/BeaconData.h"
#include "veins/toyota/opt_down/utils/OpDownMsgUtil.h"
#include "veins/toyota/opt_down/utils/ChunkMsgData.h"
#include <map>
#include <vector>
#include <utility>

using Veins::TraCIScenarioManager;
using Veins::TraCIScenarioManagerAccess;
using namespace OpDown;

class VehOpDownApp : public cSimpleModule{
public:
    VehOpDownApp();
    virtual ~VehOpDownApp();

protected:
    virtual void initialize(int stage);
    virtual void handleMessage(cMessage *msg);
    virtual void finish();

protected:
    virtual void sendBeacon();
    virtual void onBeacon(cMessage *msg);
    virtual void processNeighbors(HeterogeneousMessage *hMsg);
    virtual chunkVecType getChunkIds();
    virtual void handlePositionUpdate();

protected:
    //configuration
    simtime_t startTime;
    simtime_t stopTime;
    bool debug;
    bool infoLogging;

    // state
    cMessage *beaconTimer;
    simtime_t lastStart; // Do I need?
    int toDecisionMaker;
    int fromDecisionMaker;
    std::string sumoId;
    bool contentInterest;
    Veins::TraCIMobility* mobility;
    double beaconInterval;
    // May move to independent file
    typedef std::pair<double,chunkVecType> peerDetailType;
    typedef std::map<std::string,peerDetailType> peerMapType;
    peerMapType localDynamicMap;

    // statistics
    long beaconSentCount;
    long beaconReceivedCount;
};

#endif /* VEHOPDOWNAPP_H_ */
