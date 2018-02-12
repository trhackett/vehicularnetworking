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

Define_Module(VehOpDownApp);


VehOpDownApp::VehOpDownApp() {
    beaconTimer = NULL;
}

VehOpDownApp::~VehOpDownApp() {
    cancelAndDelete(beaconTimer);
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

        if (stopTime >= SIMTIME_ZERO && stopTime < startTime)
            error("Invalid startTime/stopTime parameters");

        // state
        cModule *tmpMobility = getParentModule()->getSubmodule("veinsmobility");
        mobility = dynamic_cast<Veins::TraCIMobility *>(tmpMobility);
        ASSERT(mobility);
        sumoId = mobility->getExternalId();
        double interestRate = par("contentInterestRate").doubleValue();

        contentInterest = uniform(0,1) < interestRate;

        // Statistics
        beaconSentCount = beaconReceivedCount = 0;

        // references
        beaconTimer = new cMessage("sendBeacon");

        scheduleAt(simTime() + startTime + uniform(0,1), beaconTimer);
    }
}
void VehOpDownApp::handleMessage(cMessage *msg) {

    if (msg == beaconTimer) {
        sendBeacon();
    }
    else if (msg->getKind() == CAM_TYPE) {
       onBeacon(msg);
    }
    else if (strcmp(msg->getName(),"positionUpdate")) {
        handlePositionUpdate();
    }
}

void VehOpDownApp::finish() {
    recordScalar("beaconsSent", beaconSentCount);
    recordScalar("beaconsReceived", beaconReceivedCount);
}

void VehOpDownApp::sendBeacon() {
    BeaconData payload = BeaconData(sumoId, mobility->getCurrentPosition(),mobility->getSpeed(),mobility->getAngleRad(),mobility->getRoadId(),simTime().dbl());
    HeterogeneousMessage *msg = OpDownMsgUtil::prepareHM("beacon",sumoId,"-1",DSRC, 2000, CAM_TYPE);
    // msg->setChannelNumber(Channels::CCH); Determine channel in decesion maker
    std::string pl = payload.toString();
    msg->setWsmData(pl.c_str());
    //Send Vector w/ ID's of chunks node possess
    ChunkMsgData *cmd = new ChunkMsgData(getChunkIds());
    msg->addObject(cmd->dup());
    send(msg, toDecisionMaker);
    ++beaconSentCount;
    scheduleAt(simTime()+ uniform(0, 1) + beaconInterval, beaconTimer);
}

void VehOpDownApp::onBeacon(cMessage *msg) {
    HeterogeneousMessage *hMsg = dynamic_cast<HeterogeneousMessage *>(msg);
    ++beaconReceivedCount;
    processNeighbors(hMsg);
}

void VehOpDownApp::processNeighbors(HeterogeneousMessage *hMsg) {

    // Get vehicle info from peer's beacon
    BeaconData peerData = BeaconData(hMsg->getWsmData());

    // Get current vehicle's info
    std::string vehRoad = mobility->getRoadId();
    double vehAngle = mobility->getAngleRad();

    // Neighbors are on the same road and heading the same direction (May want to check distance)
    if (!((fabs(vehAngle - peerData.getAngle()) < 0.01) &&
            (vehRoad.compare(peerData.getCurrentRoad()) == 0))) {
       // Vehicles are not neighbors
        return;
    }
    else {
        std::string peerId = peerData.getVehicleId();
        chunkVecType chIds;
        // Get Object data (chunkIds) from beacon
        if (hMsg->hasObject(CMD_MSG_VEC)) {
            cObject *obj = hMsg->getObject(CMD_MSG_VEC);
            chIds = dynamic_cast<ChunkMsgData*>(obj)->getchunkVec();
        }
        // Add peer data to LDM
        peerDetailType pd = std::make_pair(simTime().dbl(),chIds);
        localDynamicMap[peerId] = pd;
    }
}

chunkVecType VehOpDownApp::getChunkIds() {
    chunkVecType chIds;
    //TODO: get chunk Ids
    return chIds;
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

