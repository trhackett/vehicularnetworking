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

#include "SimpCoopApp.h"

Define_Module(SimpCoopApp);

SimpCoopApp::SimpCoopApp() {
    // Initialize pointer
    beaconTimer = nullptr;

}

SimpCoopApp::~SimpCoopApp() {
    // Clean up message
    if (beaconTimer != nullptr)
        cancelAndDelete(beaconTimer);
}


void SimpCoopApp::initialize(int stage) {
    if (stage == 0) {
        // Read params
        debug = par("debug").boolValue();
        infoLogging = par("infoLogging");
        toDecisionMaker = findGate("toDecisionMaker");
        fromDecisionMaker = findGate("fromDecisionMaker");
        startTime = par("startTime");
        stopTime = par("stopTime");
        beaconInterval = par("beaconInterval").doubleValue();
        beaconLength = par("beaconLength").doubleValue();
        chunkDataLength = par("chunkDataLength").doubleValue();
        dataProviderRatio = par("dataProviderRatio").doubleValue(); // Ratio of vehicles initalized w/ file
        totalFileChunks = par("totalFileChunks").longValue();

        lastDataTime = -1;
        requestDataTime = 0;

        // state
       cModule *tmpMobility = getParentModule()->getSubmodule("veinsmobility");
       mobility = dynamic_cast<Veins::TraCIMobility *>(tmpMobility);
       ASSERT(mobility);
       sumoId = mobility->getExternalId();

       currentlyRequesting = false; // Vehicle not yet requesting data

        if (uniform(0,1) < dataProviderRatio) {
            // Data providers so initalize w/ all data
            for (int i=0; i < totalFileChunks; i++)
            {
                chunksReceived.push_back((i));
            }
            allChunksReceived = true;
            beaconTimer = nullptr;
        }
        else {
            // Interested in data will request from cars w/ it
            allChunksReceived = false;
            for (int i=0; i < totalFileChunks; i++)
            {
                chunksNeeded.push_back((i));
            }

            beaconTimer = new cMessage("sendBeacon"); // Timer to request data
            scheduleAt(simTime() + startTime + uniform(0,1), beaconTimer);
        }

    }
}


void SimpCoopApp::handleMessage(cMessage *msg) {
    if (msg == beaconTimer) {
        sendBeacon();
        if (!currentlyRequesting && !allChunksReceived) {
            // Vehicle's first request for data
            requestDataTime = simTime();
            currentlyRequesting = true;
            INFO_ID("Veh: " << sumoId << " first request");
        }
    }
    else if (strcmp(msg->getName(),"beacon") == 0) {
       onBeacon(msg);
       delete msg;
    }
    else if (strcmp(msg->getName(),"ChunkData") == 0) {
        chunkReceived(msg);
        delete msg;
    }
}

void SimpCoopApp::sendBeacon() {
    HeterogeneousMessage *msg = OpDownMsgUtil::prepareHM("beacon",sumoId,"-1",DSRC, beaconLength);
    send(msg, toDecisionMaker);
    ++chunkRequestCarCount; // Since beacons are request for data
    scheduleAt(simTime()+ uniform(0, 1) + beaconInterval, beaconTimer);
    INFO_ID("Veh: " << sumoId << " sending data request");
}

void SimpCoopApp::onBeacon(cMessage *msg) {
    // Only vehicles w/ all data respond
    if (!allChunksReceived) {
        return;
    }
    chunkRequReceiveCount++;

    HeterogeneousMessage *hMsg = dynamic_cast<HeterogeneousMessage *>(msg);

    INFO_ID("Veh: " << sumoId << " received request from Veh " << hMsg->getSourceAddress());

    sendData();
}

void SimpCoopApp::sendData () {

    std::string allData = "";

    // Send all data at a vehicle
    // can be changed if necessary
    for (int val : chunksReceived) {
        HeterogeneousMessage *hMsg = OpDownMsgUtil::prepareHM("ChunkData",sumoId,"-1",DSRC, chunkDataLength);
        std::string data = std::to_string(val);
        allData += data + ", ";
        hMsg->setWsmData(data.c_str());
        send(hMsg, toDecisionMaker);
    }

    INFO_ID("Veh: " << sumoId << " sending data: " << allData);
}

void SimpCoopApp::chunkReceived(cMessage *msg) {

    // Nothing to do if all chunks received
    // Or vehicle not currently requesting data
    if (allChunksReceived || !currentlyRequesting) {
        return;
    }

    HeterogeneousMessage *hMsg = dynamic_cast<HeterogeneousMessage *>(msg);
    std::string data = hMsg->getWsmData();
    int val = std::stoi(data);

    // Check if chunk received is needed
    std::vector<int>::iterator it = std::find(chunksNeeded.begin(),chunksNeeded.end(), val);
    if (it == chunksNeeded.end()) {
        // Chunk not needed must already have chunk
        return;
    }

    chunkReceivedCarCount++;
    chunksNeeded.erase(it); // Remove from chunksNeeded

    // Add chunk to received list
    chunksReceived.push_back(val);

    if (!allChunksReceived && chunksNeeded.size() == 0) {
        // Last chunk received
        INFO_ID("Veh: " << sumoId << "Received all data!");
        allChunksReceived = true;
        lastDataTime = simTime();
        currentlyRequesting = false;
        //cancelAndDelete(beaconTimer); // Stop requesting
        cancelEvent(beaconTimer);
    }

}

void SimpCoopApp::finish() {
    recordScalar("startRequestingData",requestDataTime);
    recordScalar("lastChunkReceived",lastDataTime);
    recordScalar("downloadTime",lastDataTime.dbl() - requestDataTime.dbl());
    recordScalar("chunkRequestCarCount",chunkRequestCarCount);
    recordScalar("chunkReceivedCarCount",chunkReceivedCarCount);
    recordScalar("chunkSentCount",chunkSentCount);
    recordScalar("chunkRequReceiveCount",chunkRequReceiveCount);
    recordScalar("AllDataRecieved", allChunksReceived);
}
