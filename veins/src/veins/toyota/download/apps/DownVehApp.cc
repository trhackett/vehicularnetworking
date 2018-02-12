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

#include "DownVehApp.h"
#include <vector>

Define_Module(DownVehApp);

void DownVehApp::initialize(int stage) {
    if (stage == 0) {
            debug = par("debug").boolValue();
            infoLogging = par("infoLogging");
            chunkTimer = par("chunkTimer");
            toDecisionMaker = findGate("toDecisionMaker");
            fromDecisionMaker = findGate("fromDecisionMaker");
            receivedServerChunk = false;
            allChunksReceived = false;
            chunksReceived = 0;
            chunkLength = 0;
            chunkCheckTimer = chuncResponseTimer = nullptr;

            cModule *tmpMobility = getParentModule()->getSubmodule("veinsmobility");
            mobility = dynamic_cast<Veins::TraCIMobility *>(tmpMobility);
            ASSERT(mobility);
            sumoId = mobility->getExternalId();
    }
}

void DownVehApp::handleMessage(cMessage *msg) {

    HeterogeneousMessage* hetMsg = dynamic_cast<HeterogeneousMessage*>(msg);

    if (hetMsg->getPsid() == DW_SERVER_DATA_PSID)
    {
        handleServerMessage(hetMsg);
    }
    else if (hetMsg->getPsid() == DW_VEHICLE_DATA_PSID)
    {
        handleVehicleMessage(hetMsg);
    }
    else if (hetMsg->getPsid() == DW_VEHICLE_REQ_PSID)
    {
        INFO_ID("Veh" << sumoId << " Received REQ from " << hetMsg->getSourceAddress());

        // processChunkRequest(hetMsg->getWsmData()); This may cause some veh not to receive a value
        buildSendChunks(hetMsg->getWsmData());

        if (chunksRequested.size() > 0) {
            double waitTime = 1.0;
           SimTime sendTime = (simTime() + (waitTime * (1.0/chunksRequested.size())) + uniform(0.0,0.5));
           chuncResponseTimer = Veins::DownData::prepareHM("timerSend", DONTCARE, "0", "0", 0);
           //INFO_ID("CurrentTime:" << simTime().trunc(SIMTIME_US) << " SendTime: " << sendTime.trunc(SIMTIME_US)); // $$$
           scheduleAt(sendTime, chuncResponseTimer);
       }
    }
    else if (!strcmp(hetMsg->getName(),"timerSend"))
    {
        sendChunks();
    }
    else if(!strcmp(hetMsg->getName(),"timer"))
    {
        if (allChunksReceived)
            return;

        requestChunks();
    }
    else
    {
        INFO_ID("unknown PSID (" << hetMsg->getPsid() << ")");
    }
    // delete (hetMsg);
}

void DownVehApp::handleServerMessage (HeterogeneousMessage* hetMsg) {

    // Check if server chunk has been received
    if (receivedServerChunk)
        return;

    receivedServerChunk = true; // Should only receive one
    statsCollectionStart = simTime();

    // Get Data Map form Vehicles
    if (hetMsg->hasObject(DW_MSG_MAP)) {
        cObject *obj = hetMsg->getObject(DW_MSG_MAP);
        msgMap = dynamic_cast<DownMsgData*>(obj)->getMsgMap();
    }

    // Msg received from server to vehicle
    int d = std::stoi(hetMsg->getWsmData());

    constructMsgBitVector(d);

    chunkLength = hetMsg->getByteLength();

    HeterogeneousMessage* vehMsg = Veins::DownData::prepareHM("data", DSRC, sumoId,
            "-1",simTime(),DW_VEHICLE_DATA_PSID,hetMsg->getByteLength(), hetMsg->getWsmData());

    INFO_ID("Veh" << sumoId << " broadcasting " << vehMsg->getWsmData());

    SimTime rndTime = uniform(simTime(),simTime()+1) - simTime();
    sendDelayed(vehMsg, rndTime, toDecisionMaker); // Send random delay avoid sync transmissions
    // send(vehMsg, toDecisionMaker);

    // Schedule check if all messages have not been received
    checkAllChunksReceived();
    if (!allChunksReceived)
    {
        constructRequestSet();
        chunkCheckTimer = Veins::DownData::prepareHM("timer", DONTCARE, "0", "0", 0);
        scheduleAt(simTime()+chunkTimer + rndTime, chunkCheckTimer);
    }
}

void DownVehApp::handleVehicleMessage(HeterogeneousMessage* msg) {

    processChunkRequest(msg->getWsmData());

    if (allChunksReceived)
        return;

    std::string vehMsg = msg->getWsmData();
    std::vector<int> data = dataToVector(vehMsg);

    // Add messages to msgBitVec
    for (int i : data){
        if (!msgBitVec[i]) {
            // data item has not been received
            msgBitVec[i] = true;
            chunksReceived++;
        }
    }

    INFO_ID("Veh" << sumoId << " received: " << vehMsg << " from: " << msg->getSourceAddress());

    constructRequestSet();

    checkAllChunksReceived();
    if (allChunksReceived && chunkCheckTimer != nullptr) {
        cancelAndDelete(chunkCheckTimer);
    }

}

void DownVehApp::constructMsgBitVector(int data)
{
    for (auto m : msgMap)
    {
        bool received = false;

        if (m.first == data)
        {
            received = true;
            chunksReceived++;
        }

        msgBitVec.insert(std::pair<int,bool>(m.first,received));
    }
}

bool DownVehApp::checkAllChunksReceived()
{
    bool done = chunksReceived == msgBitVec.size();

    if (done) {
        if (!allChunksReceived) {
            allChunksReceived = true;
            statsCollectionEnd = simTime();
        }
    }
    return done;
}

std::vector<int> DownVehApp::dataToVector(std::string data)
{
    std::vector<int> vec;

    if (data.find(";") != std::string::npos) {
            // More than one chunk in message
            size_t pos;

            while (data.find(";") != std::string::npos)
            {
                pos = data.find(";");
                std::string num = data.substr(0,pos);
                int val = std::stoi(num);
                vec.push_back(val);
                data.erase(0, pos + 1);
            }
        }
        else if (!data.empty())
        {
            vec.push_back(std::stoi(data));
        }

    return vec;
}

void DownVehApp::requestChunks() {
    if (allChunksReceived || requestSet.empty())
        return;

    std::string data = "";

    for (int i : requestSet)
    {
        data += std::to_string(i) + ";";
    }

    // int64 msgLth = chunkLength * chunksRequested.size(); //$$$ Reuest shouldn't me that large
    int64 msgLth = 10;

    HeterogeneousMessage* vehMsg = Veins::DownData::prepareHM("data", DSRC, sumoId,
           "-1",simTime(),DW_VEHICLE_REQ_PSID, msgLth, data);

    send(vehMsg, toDecisionMaker);

    INFO_ID("Veh" << sumoId << " requesting chunks: " << data);

    delete chunkCheckTimer;
    SimTime sendTime = simTime() + chunkTimer + uniform(simTime(),simTime()+0.5);

    //INFO_ID("CurrentTime:" << simTime().trunc(SIMTIME_US) << " SendTime: " << sendTime.trunc(SIMTIME_US)); //$$$
    chunkCheckTimer = Veins::DownData::prepareHM("timer", DONTCARE, "0", "0", 0);
    scheduleAt(sendTime, chunkCheckTimer);
}

void DownVehApp::processChunkRequest(std::string data) {
    std::vector<int> vec = dataToVector(data);

    // Manage requestSet
    for (int i : vec)
    {
        std::set<int>::iterator it = chunksRequested.find(i);
        if (it != chunksRequested.end()) {
            chunksRequested.erase(it);
        }
    }
}

void DownVehApp::buildSendChunks (std::string data) {
    std::vector<int> vec = dataToVector(data);

    for (int i : vec) {
        if (msgBitVec[i]) {
            chunksRequested.insert(i);
        }
    }
}

void DownVehApp::sendChunks() {
    std::string data = "";

    if (chunksRequested.size() == 0) {
        // All chunks to be sent has been sent by other vehicles
        return;
    }

    for (int i : chunksRequested)
    {
        data += std::to_string(i) + ";";
    }

    int64 msgLth = chunkLength * chunksRequested.size();

    HeterogeneousMessage* vehMsg = Veins::DownData::prepareHM("data", DSRC, sumoId,
            "-1",simTime(),DW_VEHICLE_DATA_PSID, msgLth, data);

    send(vehMsg,toDecisionMaker);

    INFO_ID("Veh" << sumoId << " sending chunks: " << data);
}

void DownVehApp::constructRequestSet() {
    requestSet.clear();

    for (auto m : msgBitVec) {
        if (!m.second)
            requestSet.insert(m.first);
    }
}

void DownVehApp::finish() {

    // Message stats
    recordScalar("CollectionStartTime", statsCollectionStart);
    recordScalar("CollectionEndTime", statsCollectionEnd);
    statsCollectionDuration = statsCollectionEnd - statsCollectionStart;
    recordScalar("CollectionDuration", statsCollectionDuration);

}


