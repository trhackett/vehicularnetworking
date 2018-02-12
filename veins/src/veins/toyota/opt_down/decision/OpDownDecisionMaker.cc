#include "OpDownDecisionMaker.h"

using Veins::TraCIMobilityAccess;

Define_Module(OpDownDecisionMaker);

OpDownDecisionMaker::OpDownDecisionMaker(){
	// Empty
}

OpDownDecisionMaker::~OpDownDecisionMaker(){
	// Empty
}

void OpDownDecisionMaker::initialize(int stage) {
    BasicDecisionMaker::initialize(stage);

    if (stage == 0) {
        mobility = TraCIMobilityAccess().get(getParentModule());
        traci = mobility->getCommandInterface();
        traciVehicle = mobility->getVehicleCommandInterface();
    }
}

void OpDownDecisionMaker::sendLteMessage(HeterogeneousMessage* msg) {
    send(msg, toLte);
}

void OpDownDecisionMaker::sendDSRCMessage(HeterogeneousMessage* msg) {
    msg->addBitLength(headerLength);
	sendWSM(msg);
}

void OpDownDecisionMaker::sendDontCareMessage(HeterogeneousMessage* msg) {
	if (dblrand() > 0.5) {
		msg->setNetworkType(LTE);
		sendLteMessage(msg);
	} else {
		msg->setNetworkType(DSRC);
		sendDSRCMessage(msg);
	}
}

void OpDownDecisionMaker::handlePositionUpdate(cObject* obj) {
    BasicDecisionMaker::handlePositionUpdate(obj);
    cMessage *positionUpdate = new cMessage("positionUpdate");
    send(positionUpdate,toApplication);
}

