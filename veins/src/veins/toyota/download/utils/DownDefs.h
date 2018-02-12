#ifndef DOWNDEFS_H_
#define DOWNDEFS_H_

#include <map>


typedef std::map<int,bool> MsgBitVecType;

#define DW_HEARTBEAT_MSG        "HEARTBEAT"
#define DW_HEARTBEAT            100

// PSIDs
#define DW_SERVER_DATA_PSID     301
#define DW_VEHICLE_DATA_PSID    302
#define DW_VEHICLE_REQ_PSID     303
#define DW_VEHICLE_SND_PSID     304

#define DW_SERVER               "Server"

#define DW_MSG_MAP              "MsgMap"

#endif /* DOWNDEFS_H_ */
