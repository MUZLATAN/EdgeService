#include "event/Event.h"

#include <json/json.h>
#include <glog/logging.h>

#include <chrono>
#include <fstream>

#include "network/HttpClient.h"
#include "node/FlowRpcAsynNode.h"



namespace algo {
namespace vision {

#ifdef __BUILD_GTESTS__
std::string __event__last_data__;
std::string LoadLastEventSendData(void) { return __event__last_data__; };
#endif

inline void MQTTPub(const char *topic, std::string &value) {

};

inline void FlowPub(const std::string& limbustype, const std::string& dataurl, const int datatype, const std::string& eventStr) {

    FlowRpcAsynNode_flow->SendToFlowServer(limbustype, dataurl, datatype, eventStr);
};

void saveCache(Event *e, const std::vector<uchar> &imageBuff,
               const std::string &ossPath) {

};

void resendCache(void);


std::mutex resendCache_mt;
void resendCache(void) {
};


inline std::string GenValue() { return ""; };






void LoggerEvent::handler() {
    Json::Value data ;
    data["time"] = timeStamp;
    data["camera_sn"] = cameraSn;
    data["prefix"] = prefix_;
    std::string payload = Json::FastWriter().write(data);
    LOG(INFO)<<payload;
    FlowPub("LoggerEvent", "", -1, payload );
}



void MonitorEvent::handler() {
    if (!x_axis.empty() && !y_axis.empty() && !id.empty()) {
        Json::Value monitorValue;
        std::string response;

        Json::Value crowd_info_vec = Json::arrayValue;
        for (int i = 0; i < x_axis.size(); ++i) {
            Json::Value crowd_info_map;
            crowd_info_map["x"] = x_axis[i];
            crowd_info_map["y"] = y_axis[i];
            crowd_info_map["id"]= id[i];

            crowd_info_vec.append(crowd_info_map);
        }
        monitorValue["camera_sn"] = cameraSn;
        monitorValue["timestamp"] = timeStamp;
        monitorValue["Crowd"] = crowd_info_vec;

        std::string payload = Json::FastWriter().write(monitorValue);
        LOG(INFO)<<payload;

        FlowPub("MonitorEvent", "", -1, payload );

    }
};

}  // namespace vision
}  // namespace algo