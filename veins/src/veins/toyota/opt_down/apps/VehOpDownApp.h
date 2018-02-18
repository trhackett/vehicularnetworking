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
#include "veins/toyota/opt_down/utils/ChunkHistory.h"
#include "veins/toyota/opt_down/utils/ChunkMsgData.h"
#include <map>
#include <vector>
#include <utility>

using Veins::TraCIScenarioManager;
using Veins::TraCIScenarioManagerAccess;
using namespace OpDown;

// Message Strings
#define SEND_BEACON "sendBeacon"
#define REQUEST_SERVER_CHUNKS "requestSever"

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
    virtual void sendChunk(int chunkId);
    virtual void onBeacon(cMessage *msg);
    virtual void onChunkRequest(cMessage *msg);
    virtual bool processNeighbors(HeterogeneousMessage *hMsg);
    virtual chunkVecType getPeerChunkIds(std::string peer);
    virtual void handlePositionUpdate();
    virtual void requestChunksFromServer();
    virtual void requestChunksFromCars(std::vector<int> peerChunks);
    virtual void initializeChunksNeeded();
    virtual void chunkReceived(cMessage *msg);

protected:
    //configuration
    simtime_t startTime;
    simtime_t stopTime;
    bool debug;
    bool infoLogging;

    // state
    cMessage *beaconTimer;
    cMessage *requestChunksFromServerMsg;
    cMessage *requestChunksFromCarsMsg;
    simtime_t firstChunkTime;
    simtime_t lasatChunkTime;
    int toDecisionMaker;
    int fromDecisionMaker;
    std::string sumoId;
    bool contentInterest;
    Veins::TraCIMobility* mobility;
    double beaconInterval;
    double beaconLength;
    double chunkRequestLength;
    double chunkDataLength;
    double firstServerRequestTime;
    // May move to independent file
    typedef std::pair<double,chunkVecType> peerDetailType;
    typedef std::map<std::string,peerDetailType> peerMapType;
    peerMapType localDynamicMap;
    int totalFileChunks;
    std::vector<int> chunksNeeded;
    std::vector<int> chunksReceived;
    bool allChunksReceived;

    // statistics
    long beaconSentCount;
    long beaconReceivedCount;
    long chunkRequestServerCount;
    long chunkRequestCarCount;
    long chunkReceivedServerCount;
    long chunkReceivedCarCount;
    long chunkSentCount;
    long chunkRequReceiveCount;
    //TODO: Add statistic for average number of peers
};

#endif /* VEHOPDOWNAPP_H_ */
