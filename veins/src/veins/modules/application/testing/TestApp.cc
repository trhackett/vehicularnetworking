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

#include "TestApp.h"

Define_Module(TestApp);

void TestApp::initialize(int stage)
{
    if (stage == 0) {
        debug = par("debug").boolValue();
        infoLogging = par("infoLogging");
        toDecisionMaker = findGate("toDecisionMaker");
        fromDecisionMaker = findGate("fromDecisionMaker");

        cModule *tmpMobility = getParentModule()->getSubmodule("veinsmobility");
        traci = dynamic_cast<Veins::TraCIMobility *>(tmpMobility);
        ASSERT(traci);
        sumoId = traci->getExternalId();

        scheduleAt(simTime() + uniform(0, 1), new cMessage("Send"));
    }
}

void TestApp::handleMessage(cMessage *msg)
{
    if (msg->isSelfMessage())
    {
        std::cout << simTime()  << ":: veh " << sumoId << " pos: " << traci->getCurrentPosition() << std::endl;
        // May need to put .x in pos

        scheduleAt(simTime() + 1, new cMessage("Send"));
    }
}
