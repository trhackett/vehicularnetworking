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
#include <algorithm>
#include <functional>

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
    /*
     *  Broadcast vehicle info to neighbors
     *  Beacon contains: VehID, position, speed, direction angle, road, timestamp, cached chunks
     */
    virtual void sendBeacon();
    /*
     * Vehicle sends a single data chunk from chunksToSendQueue
     * and removes the chunk from the tosend queue
     */
    virtual void sendChunk(cMessage* msg);
    /*
     * Beacons determines if beacon is from neighbor
     * (neighbor - veh on same road heading same direction)
     * If cooperativeDownload set checks chunks indicated in beacon
     * Vehicle request all needed chunks from neighbor
     */
    virtual void onBeacon(cMessage *msg);
    /*
     * Veh recieves request for chunks. Receiving vehicle checks if it has chunks
     * if vehicle has chunks schedule to send chunk
     */
    virtual void onChunkRequest(cMessage *msg);
    /*
     * Determine if a vehicle is a neighbor (same road & direction)
     * if neighbor add peer id and chunk#s to local dynamic map
     */
    virtual bool processNeighbors(HeterogeneousMessage *hMsg);
    /*
     * Get the sequnce numbers of chunks a neighbor has from localDynamicMap
     */
    virtual chunkVecType getPeerChunkIds(std::string peer);
    /*
     * Called when vehicle position changes
     * Controls removal of peers from localDynamicMap after inactivity
     * Controls request from server after period of non-data exchange
     */
    virtual void handlePositionUpdate();
    /*
     * Request chunks from server using hashing scheme
     */
    virtual void requestChunksFromServerHash();
    /*
     * Request chunks from server using random scheme
     */
    virtual void requestChunksFromServerRand();
    /*
     * Request all chunks from server scheme
     */
    virtual void requestChunksFromServerNonCoop();
    /*
     * Request chunks from vehs based on beacon data
     */
    virtual void requestChunksFromCars(std::vector<int> peerChunks);
    /*
     * Init. chunks needed w/ chunks sequence number
     * If random shuffle chunks
     */
    virtual void initializeChunksNeeded();
    /*
     * When a vehicle receives a chunk from the server or other vehicles
     * If chunk is in chunksToSendQueue remove chunk if chunk is need cache
     * chunk and remove from needed list
     *
     */
    virtual void chunkReceived(cMessage *msg);
    /*
     * Called periodically to update the number of peer statistic
     * TODO: Change for all vector stats
     */
    virtual void computePeerStats();

    /*
     * Called when node receives chunk. Check if node has same chunk
     * in it's send queue. If so node cancels the scheduled transmission
     */
    virtual void checkSendQueue(int chunkNum);

    /*
     * Schedule chunk to send based on distance to vehicle
     * Further from vehicle shorter the time. Add chunk to
     * sendQueue
     */
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
    bool clusterRequired; // Only receive data from cluster members
    int minNumPeers;      // Min number of peers in cluster to initiate downloads
    int currentNumPeers;
    std::hash<std::string> hashFunc;
    double serverRequesetInterval;
    double requestRatio;

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
