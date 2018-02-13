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

import inet.applications.IUDPApp;

simple ServerOpDownApp like IUDPApp {
	parameters:
		int localPort = default(4242);
		bool debug = default(false);
		bool infoLogging = default(true);
		double msgLength @unit(byte) = default(10KiB);//default(1GiB);
		int totalFileChunks;
		
	gates:
		input udpIn @labels(UDPControlInfo/up);
		output udpOut @labels(UDPControlInfo/down);
}