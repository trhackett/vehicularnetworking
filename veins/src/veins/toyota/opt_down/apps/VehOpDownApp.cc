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

#include "VehOpDownApp.h"
#include <math.h>
#include <cassert>

Define_Module(VehOpDownApp);


VehOpDownApp::VehOpDownApp() {
    beaconTimer = requestChunksFromServerMsg = requestChunksFromCarsMsg = NULL;
}

VehOpDownApp::~VehOpDownApp() {
    cancelAndDelete(beaconTimer);
    cancelAndDelete(requestChunksFromServerMsg);
    cancelAndDelete(requestChunksFromCarsMsg);
}

void VehOpDownApp::initialize(int stage) {
    if (stage == 0) {
        // Read params
            // Information logging
        debug = par("debug").boolValue();
        infoLogging = par("infoLogging");
            // Gates
        toDecisionMaker = findGate("toDecisionMaker");
        fromDecisionMaker = findGate("fromDecisionMaker");
            // Application control
        startTime = par("startTime");
        stopTime = par("stopTime");
            // Application data variables
        beaconInterval = par("beaconInterval").doubleValue();
        beaconLength = par("beaconLength").doubleValue();
        chunkRequestLength = par("chunkRequestLength").doubleValue();
        chunkDataLength = par("chunkDataLength").doubleValue();
        firstServerRequestTime = par("firstServerRequestTime").doubleValue();
        totalFileChunks = par("totalFileChunks").longValue();
        cooperativeDownload = par("cooperativeDownload").boolValue();
        minNumPeers = par("minNumPeers").longValue();
        noDownloading = par("noDownloading").boolValue();
        requestHashing = par("requestHashing").boolValue();
        serverRequesetInterval = par("serverRequesetInterval").doubleValue();
        requestRatio = par("requestRatio").doubleValue();
        clusterRequired = par("clusterRequired").boolValue();
        currentNumPeers = 0;

        received1stChunk = false;
        if (stopTime >= SIMTIME_ZERO && stopTime < startTime)
            error("Invalid startTime/stopTime parameters");

        // state
        cModule *tmpMobility = getParentModule()->getSubmodule("veinsmobility");

        mobility = dynamic_cast<Veins::TraCIMobility *>(tmpMobility);
        ASSERT(mobility);
        sumoId = mobility->getExternalId();
        double interestRate = par("contentInterestRate").doubleValue();

        allChunksReceived = false;
        contentInterest = uniform(0,1) < interestRate; // May need to move



        // Statistics
        beaconSentCount = beaconReceivedCount = 0;
        chunkRequestServerCount = chunkRequestCarCount = chunkRequReceiveCount = 0;
        chunkReceivedServerCount = chunkReceivedCarCount = chunkSentCount= 0;
        peerSignal = registerSignal("peerCounter");
        lastServerRequest = -1;
        firstChunkTime = 0;
        lastChunkTime = -1;
        chunksSuppressed = 0;

        // references
        if (contentInterest) {
        initializeChunksNeeded();
            beaconTimer = new cMessage(SEND_BEACON);
            requestChunksFromServerMsg = new cMessage(REQUEST_SERVER_CHUNKS);
            scheduleAt(simTime() + startTime + uniform(0,1), beaconTimer);
            scheduleAt(simTime() + firstServerRequestTime + uniform(0,2), requestChunksFromServerMsg);
        }
    }
}


void VehOpDownApp::handleMessage(cMessage *msg) {

    if (!contentInterest) {
        return;
    }

    if (msg == beaconTimer) {
        if (cooperativeDownload) {
            sendBeacon();
            computePeerStats(); // Since cars send beacons periodically this is called here
        }
    }
    else if (msg == requestChunksFromServerMsg) {
        if (cooperativeDownload) {
            // Only request from server after getting minNumPeers
            if (currentNumPeers >= minNumPeers) {
                if (requestHashing) {
                    // Use consistent hashing to request chunks
                    requestChunksFromServerHash();
                }
                else {
                    // Request random chunks from server
                    requestChunksFromServerRand();
                }
            }
            else {
                // Set to current time so another request will be made later
                lastServerRequest = simTime();
            }
        }
        else {
            requestChunksFromServerNonCoop();
        }
    }
    else if (msg == requestChunksFromCarsMsg) {
        // requestChunksFromCars();
    }
    else if (strcmp(msg->getName(),BEACON_NAME) == 0) {
       onBeacon(msg);
       delete msg;
    }
    else if (strcmp(msg->getName(),CMD_NAME_DATA) == 0) {
        chunkReceived(msg);
        delete msg;
    }
    else if (strcmp(msg->getName(),CMD_NAME_REQU) == 0) {
        onChunkRequest(msg);
        delete msg;
    }
    else if (strcmp(msg->getName(),"positionUpdate") == 0) {
        handlePositionUpdate();
    }
    else if (strcmp(msg->getName(),"sendChunk") == 0) {
        sendChunk(msg);
        delete(msg);
    }
}

// Send vehicle info to neighbors (ID, position, speed, direction angle, road, timestamp, cached chunks)
void VehOpDownApp::sendBeacon() {
    BeaconData payload = BeaconData(sumoId, mobility->getCurrentPosition(),mobility->getSpeed(),mobility->getAngleRad(),mobility->getRoadId(),simTime().dbl());
    HeterogeneousMessage *msg = OpDownMsgUtil::prepareHM(BEACON_NAME,sumoId,"-1",DSRC, beaconLength);
    // msg->setChannelNumber(Channels::CCH); Determine channel in decesion maker
    std::string pl = payload.toString();
    msg->setWsmData(pl.c_str());
    //Send Vector w/ ID's of chunks node possess
    ChunkHistory *cmd = new ChunkHistory(chunksReceived);
    msg->addObject(cmd->dup());
    send(msg, toDecisionMaker);
    ++beaconSentCount;
    scheduleAt(simTime()+ uniform(0, 1) + beaconInterval, beaconTimer);
    DEBUG_ID("Veh: " << sumoId << " sending beacon");
}

// Process beacon received from neighbors
void VehOpDownApp::onBeacon(cMessage *msg) {
    HeterogeneousMessage *hMsg = dynamic_cast<HeterogeneousMessage *>(msg);
    ++beaconReceivedCount;

    if (!cooperativeDownload) {
        return;
    }

    if (processNeighbors(hMsg)) {
        // If neighbor check if any chunks needed
       std::vector<int> peerChunks = getPeerChunkIds(hMsg->getSourceAddress());
       if (peerChunks.size() > 0) {
           requestChunksFromCars(peerChunks);
       }
    }
    DEBUG_ID("Veh: " << sumoId << " received beacon from Veh " << hMsg->getSourceAddress());
}

bool VehOpDownApp::processNeighbors(HeterogeneousMessage *hMsg) {

    // Get vehicle info from peer's beacon
    BeaconData peerData = BeaconData(hMsg->getWsmData());

    // Get current vehicle's info
    std::string vehRoad = mobility->getRoadId();
    double vehAngle = mobility->getAngleRad();

    // Neighbors are on the same road and heading the same direction (May want to check distance)
    if (!((fabs(vehAngle - peerData.getAngle()) < 0.01) &&
            (vehRoad.compare(peerData.getCurrentRoad()) == 0))) {
       // Vehicles are not neighbors
        return false;
    }
    /*
     * else if (int(localDynamicMap.size()) >= minNumPeers && clusterRequired) {
        // Force the size of the cluster to be a fixed size
        return false;
    }
    */
    else {
        std::string peerId = peerData.getVehicleId();
        chunkVecType chIds;
        // Get Object data (chunkIds) from beacon
        if (hMsg->hasObject(CH_MSG_VEC)) {
            cObject *obj = hMsg->getObject(CH_MSG_VEC);
            chIds = dynamic_cast<ChunkHistory*>(obj)->getchunkVec();
        }
        // Add peer data to LDM
        peerDetailType pd = std::make_pair(simTime().dbl(),chIds);
        localDynamicMap[peerId] = pd;

        currentNumPeers = localDynamicMap.size();
    }
    return true;
}

// Get IDs of chunks a vehicle has
chunkVecType VehOpDownApp::getPeerChunkIds(std::string peer) {
    std::vector<int> peerChks = localDynamicMap[peer].second;
    return peerChks;
}

void VehOpDownApp::handlePositionUpdate() {
    // Handle the removal of peers w/ each position update
    int numMissedBeacons = 6;
    for (peerMapType::iterator iter = localDynamicMap.begin();
            iter != localDynamicMap.end(); ) {
        double msgTime = iter->second.first;
        // Remove peer after three missed beacon intervals
        if (simTime() - msgTime - numMissedBeacons * beaconInterval >= 0.0) {
            peerMapType::iterator tmp = iter;
            iter++;
            localDynamicMap.erase(tmp);
        }
        else {
            iter++;
        }
    }

    currentNumPeers = localDynamicMap.size();

    // Handle request from server
    if (!noDownloading && chunksNeeded.size() > 0 && lastServerRequest > 0) {
        if (simTime() - lastServerRequest - serverRequesetInterval >= 0.0) {
            scheduleAt(simTime(),requestChunksFromServerMsg);
            INFO_ID("Veh " << sumoId << " server request timeout! Peer count: " << currentNumPeers);
        }
    }
}

void VehOpDownApp::initializeChunksNeeded() {
    //TODO: Change method to use consistent hashing
    for (int i=0; i < totalFileChunks; i++) {
        chunksNeeded.push_back(i);
    }

    // Randomly shuffle chunks using vehicle ID as seed
    if (requestHashing) {
        // TODO
    }
    else {
        int seed = atoi(sumoId.c_str());
        std::srand(seed);
        std::random_shuffle(chunksNeeded.begin(),chunksNeeded.end());
    }
}

void VehOpDownApp::requestChunksFromServerNonCoop() {
    // Request all chunks from server

    // Prevent downloading
    if (noDownloading)
        return;

    std::string allChks = "";

    for (auto chk : chunksNeeded) {
        lastServerRequest = simTime();
        chunkRequestServerCount++;
        ChunkMsgData *cm = new ChunkMsgData(CMD_MSGTYPE_REQUEST,CMD_SENDERTYPE_CAR,chk,mobility->getCurrentPosition());
        HeterogeneousMessage *msg = OpDownMsgUtil::prepareHM(CMD_NAME_REQU,sumoId,"server",LTE, chunkRequestLength);
        msg->setWsmData(cm->toString().c_str());
        send(msg, toDecisionMaker);
        allChks += std::to_string(chk) + ", ";
    }
    INFO_ID("Veh: " << sumoId << " Non Coop requesting from SERVER chunk " << allChks);
}

void VehOpDownApp::requestChunksFromServerHash() {
    // Prevent downloading
    if (noDownloading)
        return;

    std::vector< std::pair <double,std::string> > hashedNods;

    peerMapType::iterator pi;

    // Hash peers
    for (pi=localDynamicMap.begin(); pi != localDynamicMap.end();pi++) {
        double hs = (hashFunc(pi->first) % 10000) / 10000.0;
        hashedNods.push_back(std::make_pair(hs,pi->first));
    }

    // Hash self
    double hs = (hashFunc(sumoId) % 10000) / 10000.0;
    hashedNods.push_back(std::make_pair(hs,sumoId));

    std::sort(hashedNods.begin(), hashedNods.end());

    // Find position on ring
    int c = 0;
    while (sumoId.compare(hashedNods[c].second)) {
        c++;
    }

    int numChkReq = ceil(chunksNeeded.size()/double(hashedNods.size()));
    numChkReq = std::min(numChkReq,int(ceil(requestRatio * totalFileChunks)));


    int start = c * numChkReq;
    int end = start + numChkReq;

    std::string allChks = "";

    for (int i=start; i < end && i < int(chunksNeeded.size()); i++) {
        chunkRequestServerCount++;
        int chunkNum = chunksNeeded.at(i);
        ChunkMsgData *cm = new ChunkMsgData(CMD_MSGTYPE_REQUEST,CMD_SENDERTYPE_CAR,chunkNum,mobility->getCurrentPosition());

        HeterogeneousMessage *msg = OpDownMsgUtil::prepareHM(CMD_NAME_REQU,sumoId,"server",LTE, chunkRequestLength);
        msg->setWsmData(cm->toString().c_str());
        lastServerRequest = simTime();
        send(msg, toDecisionMaker);
        allChks += std::to_string(chunkNum) + ", ";
        lastServerRequest = simTime();
    }
    INFO_ID("Veh: " << sumoId << " HAHS requesting from SERVER chunk " << allChks);
}

void VehOpDownApp::requestChunksFromServerRand() {
    // Prevent downloading
    if (noDownloading)
        return;

    if(chunksNeeded.empty()) {
        return;
    }

    int numChkReq = ceil(chunksNeeded.size()/double(localDynamicMap.size())) + 1;
    numChkReq = std::min(numChkReq, int(ceil(requestRatio * totalFileChunks)));

    std::string allChks = "";
    for (int i=0; i < numChkReq && i < int(chunksNeeded.size()); i++)
    {
        chunkRequestServerCount++;
        int chunkNum = chunksNeeded.at(i);
        ChunkMsgData *cm = new ChunkMsgData(CMD_MSGTYPE_REQUEST,CMD_SENDERTYPE_CAR,chunkNum,mobility->getCurrentPosition());

        HeterogeneousMessage *msg = OpDownMsgUtil::prepareHM(CMD_NAME_REQU,sumoId,"server",LTE, chunkRequestLength);
        msg->setWsmData(cm->toString().c_str());
        send(msg, toDecisionMaker);
        allChks += std::to_string(chunkNum) + ", ";
        lastServerRequest = simTime();
    }
    INFO_ID("Veh: " << sumoId << " RAND requesting from SERVER chunk " << allChks);
}


void VehOpDownApp::requestChunksFromCars(std::vector<int> peerChunks) {

    // Prevent downloading
    if (noDownloading)
        return;

    // For now request all chunks from peers
    for (auto chk : peerChunks) {

        std::vector<int>::iterator it = std::find(chunksReceived.begin(),chunksReceived.end(),chk);
        if (it != chunksReceived.end()) {
            // Chunk not needed
            continue;
        }

        chunkRequestCarCount++;
        ChunkMsgData *cm = new ChunkMsgData(CMD_MSGTYPE_REQUEST,CMD_SENDERTYPE_CAR,chk, mobility->getCurrentPosition());

        HeterogeneousMessage *msg = OpDownMsgUtil::prepareHM(CMD_NAME_REQU,sumoId,"-1",DSRC, chunkRequestLength);
        msg->setWsmData(cm->toString().c_str());
        send(msg, toDecisionMaker);
        INFO_ID("Veh: " << sumoId << " Requesting from CAR chunk " << chk);
    }
}

void VehOpDownApp::chunkReceived(cMessage *msg) {
    HeterogeneousMessage *hMsg = dynamic_cast<HeterogeneousMessage *>(msg);
    ChunkMsgData *cm = new ChunkMsgData(hMsg->getWsmData());

    // Check if waiting to send message
    checkSendQueue(cm->getSeqno());

    // Nothing to do if all chunks received
    if (allChunksReceived) {
        return;
    }

    if (clusterRequired) {
        std::string sender = hMsg->getSourceAddress();
        if (sender.compare("server") != 0 &&
                localDynamicMap.find(sender) == localDynamicMap.end()){
            // Sender isn't a member of cluster
            return;
        }
    }

    // Check if chunk received is needed
    std::vector<int>::iterator it = std::find(chunksNeeded.begin(),chunksNeeded.end(), cm->getSeqno());
    if (it == chunksNeeded.end()) {
        // Chunk not needed must already have chunk
        return;
    }

    if (!received1stChunk) {
        // First chunk of content received
        received1stChunk = true;
        firstChunkTime = simTime();
    }

    // Update statistics for chunk
    if (cm->getSenderType() == CMD_SENDERTYPE_SERVER) {
        chunkReceivedServerCount++;
    }
    else if (cm->getSenderType() == CMD_SENDERTYPE_CAR) {
        chunkReceivedCarCount++;
    }

    chunksNeeded.erase(it);

    // Add chunk to received list
    chunksReceived.push_back(cm->getSeqno());

    if (!allChunksReceived && chunksNeeded.size() == 0) {
        // Last chunk received
        allChunksReceived = true;
        lastChunkTime = simTime();
    }

    //allChunksReceived = int(chunksReceived.size()) == totalFileChunks;

    INFO_ID("Veh: " << sumoId << " chunk: " << cm->getSeqno() << " received from " << hMsg->getSourceAddress() << " "
            << (double(chunksReceived.size())/totalFileChunks)*100 << "% chunks received");
}

void VehOpDownApp::checkSendQueue(int chunkNum) {
    // Check queue for value
    std::map<int,cMessage*>::iterator it;
    it = chunksToSendQueue.find(chunkNum);

    if (it == chunksToSendQueue.end()) {
        // Chunk not present at node
        return;
    }

    INFO_ID("Veh " << sumoId << " suppressed chunk: " << chunkNum);
    // Cancel and delete scheduled message
    cancelAndDelete(it->second);
    // Remove from Map
    chunksToSendQueue.erase(it);
    chunksSuppressed++;

}

void VehOpDownApp::onChunkRequest(cMessage *msg) {
    chunkRequReceiveCount++;
    HeterogeneousMessage *hMsg = dynamic_cast<HeterogeneousMessage *>(msg);
    ChunkMsgData *cm = new ChunkMsgData(hMsg->getWsmData());

    INFO_ID("Veh: " << sumoId << " received chunk requ. from " << hMsg->getSourceAddress());

    // Check if vehicle has chunk
    std::vector<int>::iterator it = std::find(chunksReceived.begin(),chunksReceived.end(),cm->getSeqno());
    if (it == chunksReceived.end()) {
        // vehicle doesn't have chunk
        return;
    }

    scheduleChunkSend(cm);

    // sendChunk(cm->getSeqno());
}

void VehOpDownApp::scheduleChunkSend(ChunkMsgData* chunkMsg) {
    // Check queue for value
    std::map<int,cMessage*>::iterator it;
    it = chunksToSendQueue.find(chunkMsg->getSeqno());

    if (it != chunksToSendQueue.end()) {
        // Chunk scheduled for transmission
        return;
    }

    double distance = sqrt(pow(mobility->getCurrentPosition().x - chunkMsg->getPosition().x, 2.0)
            + pow(mobility->getCurrentPosition().y - chunkMsg->getPosition().y, 2.0));

    double timeOffset = 1.0 / distance;

    cMessage *msg = new cMessage("sendChunk");

    INFO_ID("Veh " << sumoId << " scheduled chunk: " << chunkMsg->getSeqno() << " in " << timeOffset << "s");

    chunksToSendQueue[chunkMsg->getSeqno()] = msg;
    scheduleAt(simTime() + timeOffset, msg);
}

void VehOpDownApp::sendChunk(cMessage *msg) {

    int chunkId = -1;
    std::map<int,cMessage*>::iterator it;
    for (it = chunksToSendQueue.begin(); it != chunksToSendQueue.end(); ++it) {
        if (msg == it->second) {
            chunkId = it->first;
            break;
        }
    }
    assert(chunkId >= 0); // Make sure chunk was scheduled
    // Delete and remove data chunk to send from map
    chunksToSendQueue.erase(it);

    // Transmit data
    ChunkMsgData *cm = new ChunkMsgData(CMD_MSGTYPE_DATA,CMD_SENDERTYPE_CAR,chunkId, mobility->getCurrentPosition());
    HeterogeneousMessage *hMsg = OpDownMsgUtil::prepareHM(CMD_NAME_DATA,sumoId,"-1",DSRC, chunkRequestLength);
    hMsg->setWsmData(cm->toString().c_str());
    send(hMsg, toDecisionMaker);
    chunkSentCount++;
    INFO_ID("Veh " << sumoId << " sending chunk " << chunkId);
}

void VehOpDownApp::computePeerStats() {
    int peerCount = currentNumPeers;
    // If localDynamicMap and currentNumPeers not equal there is a problem
    assert(int(localDynamicMap.size()) == currentNumPeers);
    emit(peerSignal,peerCount);
}

void VehOpDownApp::finish() {
    recordScalar("beaconsSent", beaconSentCount);
    recordScalar("beaconsReceived", beaconReceivedCount);
    recordScalar("chunkRequestServerCount",chunkRequestServerCount);
    recordScalar("chunkRequestCarCount",chunkRequestCarCount);
    recordScalar("chunkReceivedServerCount",chunkReceivedServerCount);
    recordScalar("chunkReceivedCarCount",chunkReceivedCarCount);
    recordScalar("chunkSentCount",chunkSentCount);
    recordScalar("chunkRequReceiveCount",chunkRequReceiveCount);
    recordScalar("downloadTime",lastChunkTime.dbl() - firstChunkTime.dbl());
    recordScalar("firstChunkReceived",firstChunkTime);
    recordScalar("lastChunkReceived",lastChunkTime);
    recordScalar("chunksSuppressed",chunksSuppressed);

}
