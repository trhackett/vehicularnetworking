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

#include <veins/toyota/download/apps/DownServerApp.h>

Define_Module(DownServerApp);

DownServerApp::DownServerApp() {
    msgMap = nullptr;
}

DownServerApp::~DownServerApp() {
    delete msgMap;
}

void DownServerApp::initialize(int stage) {
    ApplicationBase::initialize(stage);
        if(stage == 0){
            debug = par("debug").boolValue();
            infoLogging = par("infoLogging").boolValue();
            msgLength = par("msgLength").doubleValue();
            vehCount = par("vehCount").longValue();
            localPort = par("localPort").longValue();
            receivedMessages = 0;
            chunksSent = 0;
            manager = TraCIScenarioManagerAccess().get();

            msgMap = new DownMsgData(DW_MSG_MAP);

            HeterogeneousMessage* msg = Veins::DownData::prepareHM("timer", DONTCARE, "0", "0", 0);

            scheduleAt(simTime() + 5, msg);
        }
}

void DownServerApp::finish(){
    INFO_ID("Received " << receivedMessages << " messages via LTE");
}

void DownServerApp::handleMessageWhenUp(cMessage *msg){
    HeterogeneousMessage* hetMsg = dynamic_cast<HeterogeneousMessage*>(msg);

    std::string name = hetMsg->getName();

    if(hetMsg){
        if (name.find("timer") != std::string::npos)
        {
            int currentVehNum;
            currentVehNum = manager->getManagedHosts().size();

            if (currentVehNum >= vehCount) {
                sendData();
            }
            else {
                HeterogeneousMessage* msg = Veins::DownData::prepareHM("timer", DONTCARE, "0", "0", 0);
                scheduleAt(simTime() + 1, msg);
            }
        }
    }
}

bool DownServerApp::handleNodeStart(IDoneCallback *doneCallback){
    socket.setOutputGate(gate("udpOut"));
    int localPort = par("localPort");
    socket.bind(localPort);
    return true;
}

bool DownServerApp::handleNodeShutdown(IDoneCallback *doneCallback){
    return true;
}

void DownServerApp::handleNodeCrash(){}

void DownServerApp::sendData() {

    constructMsgMap();

    MsgMapType mm = msgMap->getMsgMap();

    int numHost = mm.size();   // Find all of the host in the scenario

    long chunkSize = msgLength / numHost;

    // Send a chunk of message to each host in network
    for (auto h : mm)
    {
        IPv4Address address = manager->getIPAddressForID(h.second);
        std::string hostName = h.second;

        std::string data = std::to_string(h.first);

        HeterogeneousMessage* hmsg = Veins::DownData::prepareHM("data",LTE,DW_SERVER,
                hostName,simTime(),DW_SERVER_DATA_PSID,chunkSize, data);

        hmsg->addObject(msgMap->dup());

        INFO_ID("Server sending message to " << hostName << " Bytes: " << chunkSize);
        socket.sendTo(hmsg,address,localPort);
    }
}

void DownServerApp::constructMsgMap()
{
    // Map host name to data

    std::map<std::string, cModule*> host = manager->getManagedHosts();

    for (auto h : host)
    {
        msgMap->insertElement(++chunksSent, h.first);
    }
}
