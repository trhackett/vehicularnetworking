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

#ifndef __VEINS_TESTAPP_H_
#define __VEINS_TESTAPP_H_

#include <omnetpp.h>
#include "veins/modules/heterogeneous/messages/HeterogeneousMessage_m.h"
#include "veins/modules/mobility/traci/TraCIScenarioManager.h"
#include "veins/modules/mobility/traci/TraCIMobility.h"
#include "veins/base/utils/SimpleLogger.h"

using Veins::TraCIScenarioManager;
using Veins::TraCIScenarioManagerAccess;

class TestApp : public cSimpleModule
{
  protected:
    int toDecisionMaker;
    int fromDecisionMaker;
    bool debug;
    bool infoLogging;
    std::string sumoId;
    Veins::TraCIMobility* traci;

    virtual void initialize(int stage);
    virtual void handleMessage(cMessage *msg);
};

#endif
