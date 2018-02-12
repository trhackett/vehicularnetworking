#include "DownDecisionMaker.h"

using Veins::TraCIMobilityAccess;

Define_Module(DownDecisionMaker);

DownDecisionMaker::DownDecisionMaker(){
	// Empty
}

DownDecisionMaker::~DownDecisionMaker(){
	// Empty
}

void DownDecisionMaker::initialize(int stage) {
    BasicDecisionMaker::initialize(stage);

    if (stage == 0) {
        mobility = TraCIMobilityAccess().get(getParentModule());
        traci = mobility->getCommandInterface();
        traciVehicle = mobility->getVehicleCommandInterface();
    }
}

void DownDecisionMaker::sendLteMessage(HeterogeneousMessage* msg) {
	// MobilityBase* eNodeBMobility = dynamic_cast<MobilityBase*>(getModuleByPath("scenario.eNodeB1")->getSubmodule("mobility"));
	// ASSERT(eNodeBMobility);
	// AnnotationManager* annotations = AnnotationManagerAccess().getIfExists();
	// annotations->scheduleErase(1, annotations->drawLine(eNodeBMobility->getCurrentPosition(), getPosition(), "yellow"));
	send(msg, toLte);
}

void DownDecisionMaker::sendDSRCMessage(HeterogeneousMessage* msg) {
	msg->addBitLength(headerLength);
	msg->setChannelNumber(Channels::CCH);
	msg->setWsmVersion(2);
	msg->setRecipientAddress(BROADCAST);
	msg->setSenderPos(currentPosition);
	msg->setSerial(2);
	sendWSM(msg);
}

void DownDecisionMaker::sendDontCareMessage(HeterogeneousMessage* msg) {
	if (dblrand() > 0.5) {
		msg->setNetworkType(LTE);
		sendLteMessage(msg);
	} else {
		msg->setNetworkType(DSRC);
		sendDSRCMessage(msg);
	}
}

void DownDecisionMaker::handlePositionUpdate(cObject* obj) {
    BasicDecisionMaker::handlePositionUpdate(obj);
}

