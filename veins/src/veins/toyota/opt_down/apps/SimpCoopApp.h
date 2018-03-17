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

#ifndef SIMPCOOPAPP_H_
#define SIMPCOOPAPP_H_

#include <omnetpp.h>
#include "veins/modules/heterogeneous/messages/HeterogeneousMessage_m.h"
#include "veins/modules/mobility/traci/TraCIScenarioManager.h"
#include "veins/modules/mobility/traci/TraCIMobility.h"
#include "veins/base/utils/SimpleLogger.h"
#include "veins/toyota/opt_down/utils/OpDownMsgUtil.h"
#include <vector>

using Veins::TraCIScenarioManager;
using Veins::TraCIScenarioManagerAccess;

class SimpCoopApp : public cSimpleModule{

protected:
    virtual void initialize(int stage);
    virtual void handleMessage(cMessage *msg);
    virtual void finish();

    /*
     * Beacon is a vehicles interest in a file
     */
    virtual void sendBeacon();

    /*
     * Veh recieves request for file
     * Vehicles with all data respond with data
     */
    virtual void onBeacon(cMessage *msg);

    /*
     * When a vehicle receives a chunk from the server or other vehicles
     * Cache chunk and remove from request list
     */
    virtual void chunkReceived(cMessage *msg);

    /*
     * Broadcast file data. Sends each file chunk individually to lower layer
     * TODO: This can be changed to send only a portion of data at a time (whatever works best)
     */
    void sendData();

protected:
    //configuration
    simtime_t startTime;
    simtime_t stopTime;
    bool debug;
    bool infoLogging;

    // state
    cMessage *beaconTimer;
    int toDecisionMaker;
    int fromDecisionMaker;
    std::string sumoId;
    Veins::TraCIMobility* mobility;
    double beaconInterval;
    double beaconLength;
    double chunkDataLength;
    int totalFileChunks;
    std::vector<int> chunksNeeded;
    std::vector<int> chunksReceived;
    bool allChunksReceived;
    double dataProviderRatio;
    bool currentlyRequesting;

    // statistics
   long chunkRequestCarCount;
   long chunkReceivedCarCount;
   long chunkSentCount;
   long chunkRequReceiveCount;

   simtime_t requestDataTime;
   simtime_t lastDataTime;

public:
    SimpCoopApp();
    virtual ~SimpCoopApp();
};

#endif /* SIMPCOOPAPP_H_ */
