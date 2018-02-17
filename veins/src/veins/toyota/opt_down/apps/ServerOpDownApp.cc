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

#include "ServerOpDownApp.h"

Define_Module(ServerOpDownApp);

ServerOpDownApp::ServerOpDownApp() {
    // TODO Auto-generated constructor stub

}

ServerOpDownApp::~ServerOpDownApp() {
    // TODO Auto-generated destructor stub
}

void ServerOpDownApp::initialize(int stage) {
    ApplicationBase::initialize(stage);
    if(stage == 0){
        debug = par("debug").boolValue();
        infoLogging = par("infoLogging").boolValue();
        receivedMessages = sentMessages = 0;
        manager = TraCIScenarioManagerAccess().get();
        ASSERT(manager);

        msgLength = par("msgLength");
        localPort = par("localPort").longValue();
        totalFileChunks = par("totalFileChunks").longValue();
    }
}

void ServerOpDownApp::handleMessageWhenUp(cMessage *msg){
    HeterogeneousMessage* hMsg = dynamic_cast<HeterogeneousMessage*>(msg);
    if(hMsg){
        receivedMessages++;
        std::string sourceAddress = hMsg->getSourceAddress();
        INFO_ID("Received Heterogeneous Message from " << sourceAddress);

        ChunkMsgData *cmd = new ChunkMsgData(hMsg->getWsmData());

        if (cmd->getMsgType() == CMD_MSGTYPE_REQUEST) {
            sendChunk(sourceAddress, cmd->getSeqno());
        }


    }
    delete msg;
}

bool ServerOpDownApp::handleNodeStart(IDoneCallback *doneCallback){
    socket.setOutputGate(gate("udpOut"));
    int localPort = par("localPort");
    socket.bind(localPort);
    return true;
}

bool ServerOpDownApp::handleNodeShutdown(IDoneCallback *doneCallback){
    return true;
}

void ServerOpDownApp::handleNodeCrash(){}

void ServerOpDownApp::sendChunk(std::string destAddress, int seqnoRequest) {
    sentMessages++;
    HeterogeneousMessage *reply = new HeterogeneousMessage("Chunk Data");
    IPv4Address address = manager->getIPAddressForID(destAddress);
    reply->setSourceAddress("server");

    ChunkMsgData payload = ChunkMsgData(CMD_MSGTYPE_DATA,CMD_SENDERTYPE_SERVER,
            seqnoRequest,std::to_string(seqnoRequest));
    reply->setWsmData(payload.toString().c_str());

    reply->setByteLength(msgLength);

    INFO_ID("Sending Message back to " << address << " data " << seqnoRequest);
    socket.sendTo(reply, address, localPort);
}


void ServerOpDownApp::finish() {
    recordScalar("receivedMessages", receivedMessages);
    recordScalar("sentMessages", sentMessages);
    INFO_ID("Received " << receivedMessages << " messages via LTE.");
    INFO_ID("Sent " << sentMessages << " messages via LTE.");
}
