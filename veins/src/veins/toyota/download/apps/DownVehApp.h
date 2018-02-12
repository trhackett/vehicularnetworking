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

#ifndef DOWNVEHAPP_H_
#define DOWNVEHAPP_H_

#include <omnetpp.h>
#include "veins/modules/heterogeneous/messages/HeterogeneousMessage_m.h"
#include "veins/modules/mobility/traci/TraCIScenarioManager.h"
#include "veins/modules/mobility/traci/TraCIMobility.h"
#include "veins/base/utils/SimpleLogger.h"
#include "veins/toyota/download/utils/DownDefs.h"
#include "veins/toyota/download/utils/DownData.h"
#include "veins/toyota/download/utils/DownMsgData.h"
#include <set>


using Veins::TraCIScenarioManager;
using Veins::TraCIScenarioManagerAccess;

class DownVehApp: public cSimpleModule {

protected:
    int toDecisionMaker;
    int fromDecisionMaker;
    std::string sumoId;
    bool debug;
    bool infoLogging;
    bool receivedServerChunk;
    bool allChunksReceived;
    unsigned int chunksReceived; // Count of received chunks
    Veins::TraCIMobility* mobility;
    double chunkTimer; // Schedule check of chunks received
    MsgMapType msgMap; // Map of Chunks and chunk owners
    MsgBitVecType msgBitVec; // Maintain msgs received
    HeterogeneousMessage* chunkCheckTimer;
    HeterogeneousMessage* chuncResponseTimer;
    int64 chunkLength;
    std::set<int> requestSet;
    std::set<int> chunksRequested;

    // Stats
    simtime_t statsCollectionStart;
    simtime_t statsCollectionEnd;
    simtime_t statsCollectionDuration;

protected:
    virtual void initialize(int stage);
    virtual void handleMessage(cMessage *msg);
    void handleServerMessage(HeterogeneousMessage* msg);
    void handleVehicleMessage(HeterogeneousMessage* msg);
    virtual void requestChunks();
    void constructRequestSet();
    void processChunkRequest(std::string data);
    void buildSendChunks(std::string data);
    void sendChunks();
    virtual void finish();


private:
    void constructMsgBitVector(int data);
    bool checkAllChunksReceived();
    std::vector<int> dataToVector(std::string data);

};

#endif /* DOWNVEHAPP_H_ */
