
#ifndef OpDownDecisionMaker_H
#define OpDownDecisionMaker_H

#include "veins/modules/lte/BasicDecisionMaker.h"
#include "veins/modules/mobility/traci/TraCIMobility.h"
#include "veins/modules/mobility/traci/TraCICommandInterface.h"

using Veins::TraCIMobility;
using Veins::TraCICommandInterface;

class OpDownDecisionMaker: public BasicDecisionMaker{

    protected:
        TraCIMobility* mobility;
        TraCICommandInterface* traci;
        TraCICommandInterface::Vehicle* traciVehicle;

    public:
		OpDownDecisionMaker();
		~OpDownDecisionMaker();

	protected:
		virtual void initialize(int stage);
		virtual void sendLteMessage(HeterogeneousMessage* msg);
        virtual void sendDSRCMessage(HeterogeneousMessage* msg);
        virtual void sendDontCareMessage(HeterogeneousMessage* msg);
        virtual void handlePositionUpdate(cObject* obj);
};

#endif
