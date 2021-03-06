#include "event/Event.h"
#include <json/json.h>
#include <chrono>
#include <fstream>

#include "network/HttpClient.h"
#include "node/FlowRpcAsynNode.h"



namespace meta {
namespace vision {


inline void FlowPub(const std::string& limbustype, const std::string& dataurl, const int datatype, const std::string& eventStr) {

    FlowRpcAsynNode_flow->SendToFlowServer(limbustype, dataurl, datatype, eventStr);
};

void LoggerEvent::handler() {
    Json::Value data ;
    data["time"] = timeStamp;
    data["camera_sn"] = cameraSn;
    data["prefix"] = prefix_;
    std::string payload = Json::FastWriter().write(data);
    std::cout<<__TIMESTAMP__<<"  ["<< __FILE__<<": " <<__LINE__<<"]  "<<payload<<std::endl;
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
        std::cout<<__TIMESTAMP__<<"  ["<< __FILE__<<": " <<__LINE__<<"]  "<<payload<<std::endl;

        FlowPub("MonitorEvent", "", -1, payload );

    }
};

}  // namespace vision
}  // namespace meta