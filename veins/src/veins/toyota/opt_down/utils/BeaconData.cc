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

#include "BeaconData.h"

using namespace OpDown;

BeaconData::~BeaconData() {
    // TODO Auto-generated destructor stub
}

BeaconData::BeaconData(std::string vehicleIdIn,Coord positionIn,double speedIn,double angleIn,std::string currentRoadIn,double timestampIn,int TTLIn) :
    vehicleId(vehicleIdIn),
    position(positionIn),
    speed(speedIn),
    angle(angleIn),
    currentRoad(currentRoadIn),
    timestamp(timestampIn),
    TTL(TTLIn)
{ }

BeaconData::BeaconData(std::string msgString) {
    std::vector<std::string> words;
    boost::split(words, msgString, boost::is_any_of(" "), boost::token_compress_on);

    std::vector<std::string>::iterator iter = words.begin();

    vehicleId = *iter++;
    double posX = atof((*iter++).c_str());
    double posY = atof((*iter++).c_str());
    //double posZ = atof((*iter++).c_str());
    speed = atof((*iter++).c_str());
    angle = atof((*iter++).c_str());
    position = Coord(posX,posY);
    currentRoad = *iter++;
    timestamp = atof((*iter++).c_str());
    TTL = atoi((*iter).c_str());

    //std::cout << "Created: " << vehicleId << "," << position << "," << speed << "," << angle << "," << currentRoad << "," << timestamp << "," << TTL << std::endl;
    //DEBUG_ID("Created: " << vehicleId << "," << position << "," << speed << "," << angle << "," << currentRoad << "," << timestamp << "," << TTL);
}

std::string BeaconData::toString() {
    std::stringstream sstream;
    sstream << vehicleId << " " << position.x << " " << position.y << " " << speed << " " << angle << " " << currentRoad << " " << timestamp << " " << TTL;
    //std::cout << "Bec: " << sstream.str() << std::endl;
    return sstream.str();
}

void BeaconData::fromString(std::string msgString) {
    //std::cout << "fromString begin" << std::endl;
    std::vector<std::string> words;
    boost::split(words, msgString, boost::is_any_of(" "), boost::token_compress_on);

    std::vector<std::string>::iterator iter = words.begin();
    vehicleId = *iter++;
    double posX = atof((*iter++).c_str());
    double posY = atof((*iter++).c_str());
    speed = atof((*iter++).c_str());
    angle = atof((*iter++).c_str());
    position = Coord(posX,posY);

    currentRoad = *iter++;
    timestamp = atof((*iter++).c_str());
    TTL = atoi((*iter).c_str());

    //std::stringstream sstream;
    //std::cout << "Created: " << vehicleId << "," << position << "," << speed << "," << angle << "," << currentRoad << "," << timestamp << "," << TTL <<std::endl;
    //DEBUG_ID("Created: " << vehicleId << "," << position << "," << speed << "," << angle << "," << currentRoad << "," << timestamp << "," << TTL);
}

std::string BeaconData::getVehicleId() {
    return vehicleId;
}

Coord BeaconData::getPosition() {
    return position;
}

double BeaconData::getSpeed() {
    return speed;
}

double BeaconData::getAngle() {
    return angle;
}

std::string BeaconData::getCurrentRoad() {
    return currentRoad;
}

double BeaconData::getTimestamp() {
    return timestamp;
}

int BeaconData::getTTL() {
    return TTL;
}

void BeaconData::setVehicleId(std::string vehicleIdIn) {
    vehicleId = vehicleIdIn;
}

void BeaconData::setPosition(Coord positionIn) {
    position = positionIn;
}

void BeaconData::setSpeed(double speedIn) {
    speed = speedIn;
}

void BeaconData::setTimestamp(double timestampIn) {
    timestamp = timestampIn;
}

void BeaconData::setCurrentRoad(std::string currRoadIn) {
    currentRoad = currRoadIn;
}

void BeaconData::setTTL(int TTLIn) {
    TTL = TTLIn;
}
