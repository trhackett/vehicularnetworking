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
    virtual void sendChunk(cMessage* msg);
    virtual void onBeacon(cMessage *msg);
    virtual void onChunkRequest(cMessage *msg);
    virtual bool processNeighbors(HeterogeneousMessage *hMsg);
    virtual chunkVecType getPeerChunkIds(std::string peer);
    virtual void handlePositionUpdate();
    virtual void requestChunksFromServerHash();
    virtual void requestChunksFromServerRand();
    virtual void requestChunksFromServerNonCoop();
    virtual void requestChunksFromCars(std::vector<int> peerChunks);
    virtual void initializeChunksNeeded();
    /*
     * When a vehicle receives a chunk from the server or other vehicles
     */
    virtual void chunkReceived(cMessage *msg);
    /*
     * Called periodically to update the number of peer statistic
     */
    virtual void computePeerStats();

    /*
     * Called when node receives chunk. Check if node has same chunk
     * in it's send queue. If so node cancels the scheduled transmission
     */
    virtual void checkSendQueue(int chunkNum);

    virtual void scheduleChunkSend(ChunkMsgData* chunkMsg);

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
    std::map<int,cMessage*> chunksToSendQueue; // <chunkSeqNo,ScheduledMessage>
    simtime_t firstChunkTime;
    simtime_t lastChunkTime;
    simtime_t lastServerRequest;
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
    bool cooperativeDownload;
    bool received1stChunk;
    bool requestHashing;
    bool noDownloading;
    int minNumPeers;
    int currentNumPeers;

    // statistics
    long beaconSentCount;
    long beaconReceivedCount;
    long chunkRequestServerCount;
    long chunkRequestCarCount;
    long chunkReceivedServerCount;
    long chunkReceivedCarCount;
    long chunkSentCount;
    long chunkRequReceiveCount;
    long chunksSuppressed;

    simsignal_t peerSignal;
};

#endif /* VEHOPDOWNAPP_H_ */
