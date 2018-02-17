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

#ifndef BEACONDATA_H_
#define BEACONDATA_H_

#include <iostream>
#include <Coord.h>
#include <sstream>
#include <string>
#include <boost/algorithm/string.hpp>

namespace OpDown {

class BeaconData {
public:
    BeaconData(std::string vehicleIdIn,Coord positionIn,double speedIn,double angleIn, std::string currentRoadIn,double timestampIn,int TTLIn=1);
    BeaconData(std::string msgString);
    virtual ~BeaconData();
    std::string toString();
    void fromString(std::string msgString);

    std::string getVehicleId();
    Coord getPosition();
    double getSpeed();
    double getAngle();
    std::string getCurrentRoad();
    double getTimestamp();
    int getTTL();

    void setVehicleId(std::string vehicleIdIn);
    void setPosition(Coord positionIn);
    void setSpeed(double speedIn);
    void setAngle(double angleIn);
    void setCurrentRoad(std::string currRoadIn);
    void setTimestamp(double simtimeIn);
    void setTTL(int TTLIn);

private:
    std::string vehicleId;
    Coord position;
    double speed;
    double angle;
    std::string currentRoad;
    double timestamp;
    int TTL;

};

} // End namespace
#endif /* BEACONDATA_H_ */
