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
#include <algorithm>

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
        debug = par("debug").boolValue();
        infoLogging = par("infoLogging");
        toDecisionMaker = findGate("toDecisionMaker");
        fromDecisionMaker = findGate("fromDecisionMaker");
        startTime = par("startTime");
        stopTime = par("stopTime");
        beaconInterval = par("beaconInterval").doubleValue();
        firstServerRequestTime = par("firstServerRequestTime").doubleValue();
        totalFileChunks = par("totalFileChunks").longValue();

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

        initializeChunksNeeded();

        // Statistics
        beaconSentCount = beaconReceivedCount = 0;
        chunkRequestServer = chunkRequestCar = 0;
        chunkReceivedServer = chunkReceivedCar = 0;

        // references
        beaconTimer = new cMessage(SEND_BEACON);
        requestChunksFromServerMsg = new cMessage(REQUEST_SERVER_CHUNKS);

        scheduleAt(simTime() + startTime + uniform(0,1), beaconTimer);
        scheduleAt(simTime() + firstServerRequestTime + uniform(0,2), requestChunksFromServerMsg);
    }
}


void VehOpDownApp::handleMessage(cMessage *msg) {

    if (msg == beaconTimer) {
        sendBeacon();
    }
    else if (msg == requestChunksFromServerMsg) {
        requestChunksFromServer();
    }
    else if (msg == requestChunksFromCarsMsg) {
        requestChunksFromCars();
    }
    else if (msg->getKind() == CAM_TYPE) {
       onBeacon(msg);
    }
    else if (strcmp(msg->getName(),"positionUpdate")) {
        handlePositionUpdate();
    }
}

// Send vehicle info to neighbors (ID, position, speed, direction angle, road, timestamp, cached chunks)
void VehOpDownApp::sendBeacon() {
    BeaconData payload = BeaconData(sumoId, mobility->getCurrentPosition(),mobility->getSpeed(),mobility->getAngleRad(),mobility->getRoadId(),simTime().dbl());
    HeterogeneousMessage *msg = OpDownMsgUtil::prepareHM("beacon",sumoId,"-1",DSRC, 2000, CAM_TYPE);
    // msg->setChannelNumber(Channels::CCH); Determine channel in decesion maker
    std::string pl = payload.toString();
    msg->setWsmData(pl.c_str());
    //Send Vector w/ ID's of chunks node possess
    ChunkHistory *cmd = new ChunkHistory(getChunkIds());
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

    if (processNeighbors(hMsg)) {
        // If neighbor check if any chunks needed
       // FIXME: std::vector<int> neighborChunks =
    }
    DEBUG_ID("Veh: " << sumoId << " received beacon from Veh " << hMsg->getSourceAddress());
    delete msg;
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
    }
    return true;
}

void VehOpDownApp::findNeighborChunks() {

}

// Get IDs of chunks a vehicle has
chunkVecType VehOpDownApp::getChunkIds() {
    // FIXME:
    return chunksReceived;
}

void VehOpDownApp::handlePositionUpdate() {
    for (peerMapType::iterator iter = localDynamicMap.begin();
            iter != localDynamicMap.end(); ) {
        double msgTime = iter->second.first;
        // Remove peer after three missed beacon intervals
        if (simTime() - msgTime - 3 * beaconInterval >= 0.0) {
            peerMapType::iterator tmp = iter;
            iter++;
            localDynamicMap.erase(tmp);
        }
        else {
            iter++;
        }
    }
}

void VehOpDownApp::initializeChunksNeeded() {
    for (int i=0; i < totalFileChunks; i++) {
        chunksNeeded.push_back(i);
    }
    // Randomly shuffle chunks using vehicle ID as seed
    int seed = atoi(sumoId.c_str());
    std::srand(seed);
    std::random_shuffle(chunksNeeded.begin(),chunksNeeded.end());
}

void VehOpDownApp::requestChunksFromServer() {
    //TODO: Vehicles Attempt to request unique chunks from server
    //      All vehicles know the size of the contents use to request
    //      chunks (vehicle's ID and modulous)
    //      May try mor sophisticated technique
    //FIXME: Temporary solution uses random selection of chunks
    //      Change to use consitent hashing (CHORD)

    if(chunksNeeded.empty()) {
        return;
    }
    chunkRequestServer++;
    int chunkNum = chunksNeeded.at(0);
    ChunkMsgData *cm = new ChunkMsgData(CMD_MSGTYPE_REQUEST,CMD_SENDERTYPE_CAR,chunkNum);

    HeterogeneousMessage *msg = OpDownMsgUtil::prepareHM("data",sumoId,"server",LTE, 1000, CHUNK_TYPE);
    msg->setWsmData(cm->toString().c_str());
    send(msg, toDecisionMaker);
    INFO_ID("Veh: " << sumoId << "Requesting from server chunk " << chunkNum);
}

void VehOpDownApp::requestChunksFromCars() {
    // TODO: Define how vehicles will coordinate the exchange of contents
}

void VehOpDownApp::chunkReceived(HeterogeneousMessage *hMsg) {
    ChunkMsgData *cm = new ChunkMsgData(hMsg->getWsmData());

    // Update statistics for chunk
    if (cm->getSenderType() == CMD_SENDERTYPE_SERVER) {
        chunkReceivedServer++;
    }
    else if (cm->getSenderType() == CMD_SENDERTYPE_CAR) {
        chunkReceivedCar++;
    }

    // Nothing to do if all chunks received
    if (allChunksReceived) {
        return;
    }

    // Check if chunk received is needed
    std::vector<int>::iterator it = std::find(chunksNeeded.begin(),chunksNeeded.end(), cm->getSeqno());
    if (it == chunksNeeded.end()) {
        // Chunk not needed must already have chunk
        return;
    }

    chunksNeeded.erase(it);

    // Add chunk to received list
    chunksReceived.push_back(cm->getSeqno());

    INFO_ID("Chunk: " << cm->getSeqno() << " received from " << hMsg->getSourceAddress());
}

void VehOpDownApp::finish() {
    recordScalar("beaconsSent", beaconSentCount);
    recordScalar("beaconsReceived", beaconReceivedCount);
    recordScalar("chunkRequestServer",chunkRequestServer);
    recordScalar("chunkRequestCar",chunkRequestCar);
}
