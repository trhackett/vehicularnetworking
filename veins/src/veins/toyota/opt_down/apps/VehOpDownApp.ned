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

package org.car2x.veins.toyota.opt_down.apps;

import org.car2x.veins.base.modules.IBaseHeteroApplLayer;

simple VehOpDownApp like IBaseHeteroApplLayer {
	parameters:
       bool debug = default(false);
       bool infoLogging = default(true);
       volatile double beaconInterval @unit("s") = default(1s); // Time to wait between beacons
       double beaconLength @unit("bit");
       double chunkRequestLength @unit("bit");
       double chunkDataLength @unit("bit");
       double startTime @unit("s") = default(uniform(0s,this.beaconInterval)); // Send first beacon at start time
       double stopTime @unit("s") = default(-1s); // time to finish sending, negative values mean forever
       double contentInterestRate = default(1);
       double firstServerRequestTime @unit("s");
       int totalFileChunks;
       
       // statistics
       @signal[peerCounter](type="long");
       @statistic[peerCount](title="Number of Peers"; source=peerCounter; record=vector,stats; interpolationmode=none);
       
   	gates:
        output toDecisionMaker;
        input  fromDecisionMaker;
}