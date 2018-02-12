package org.car2x.veins.modules.heterogeneous.application;

import org.car2x.veins.base.modules.IBaseHeteroApplLayer;

//
// A simple application that just sends messages.
//
simple SimpleApp like IBaseHeteroApplLayer{
	parameters:
		bool debug = default(false);
		bool infoLogging = default(true);
    gates:
        output toDecisionMaker;
        input  fromDecisionMaker;
}
