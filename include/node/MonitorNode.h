#pragma once

#include <map>
#include <opencv2/opencv.hpp>
#include <vector>
#include "DynamicFactory.h"
#include "Node.h"
#include "common.h"

namespace algo {
namespace vision {
;
class MonitorNode : public Node, DynamicCreator<MonitorNode> {
 public:
    MonitorNode() : Node(ALGO_NODE_MONITOR) {}
    virtual ~MonitorNode() {}
    virtual void run();

    virtual void init();

 private:
   void SendMsg(std::shared_ptr<AlgoData> message);


 private:
    std::vector<int> x_;
    std::vector<int> y_;
    std::vector<int> id_;
    bool need_visualize;
   //  std::shared_ptr<Tracker> tracker_;
    int cur_interval_cnt_;
    int monitor_interval;
    std::map<std::string, int> cls_id_map;
};
}  // namespace vision
}  // namespace algo