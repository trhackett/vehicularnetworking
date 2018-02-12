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

#ifndef DOWNSERVERAPP_H_
#define DOWNSERVERAPP_H_


#include <omnetpp.h>
#include "ApplicationBase.h"
#include "INETDefs.h"
#include "UDPSocket.h"
#include "IPv4Address.h"
#include "veins/modules/heterogeneous/messages/HeterogeneousMessage_m.h"
#include "veins/modules/mobility/traci/TraCIScenarioManager.h"
#include "veins/base/utils/SimpleLogger.h"
#include "veins/toyota/download/utils/DownDefs.h"
#include "veins/toyota/download/utils/DownData.h"
#include "veins/toyota/download/utils/DownMsgData.h"

using Veins::TraCIScenarioManager;
using Veins::TraCIScenarioManagerAccess;

class DownServerApp: public ApplicationBase {
protected:
    UDPSocket socket;
    double msgLength;
    int vehCount;
    long receivedMessages;
    long localPort;
    bool debug;
    bool infoLogging;
    int chunksSent;
    TraCIScenarioManager* manager;
    DownMsgData *msgMap;


protected:

    virtual int numInitStages() const {
        return 4;
    }
    virtual void initialize(int stage);
    virtual void finish();
    virtual void handleMessageWhenUp(cMessage *msg);

    void sendData();

    virtual bool handleNodeStart(IDoneCallback *doneCallback);
    virtual bool handleNodeShutdown(IDoneCallback *doneCallback);
    virtual void handleNodeCrash();
    void constructMsgMap();


public:
    DownServerApp();
    virtual ~DownServerApp();
};

#endif /* DOWNSERVERAPP_H_ */
