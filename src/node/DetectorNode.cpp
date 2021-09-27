#include "node/DetectorNode.h"

#include <algorithm>
#include <thread>

#include "event/Event.h"
#include "mgr/ConfigureManager.h"
#include "mgr/QueueManager.h"
#include "util.h"

using namespace algo::core;

namespace algo {
namespace vision {

void DetectorNode::init() {
    QueueManager::SafeGet(node_name_, input_queue_);
    QueueManager::SafeGet(ALGO_NODE_DISPATCH, output_queue_);

    std::vector<std::string> features =
        ConfigureManager::instance()->getAsVectorString("feature");
    for (int i = 0; i < features.size(); ++i) {
        std::string base_node = feature_map[features[i]][0];
        QueuePtr q;
        QueueManager::SafeGet(base_node, q);
        next_node_unit_[base_node] = q;
        LOG(INFO) << base_node << " add to detectornode next queue. ";
    }
}

void DetectorNode::run() {
    while (true) {
        if (gt->sys_quit) {
            break;
        }

        std::shared_ptr<algo::vision::AlgoObject> context;
        if (input_queue_->Empty()){
//            std::this_thread::sleep_for(std::chrono::seconds(5));
            continue;
        }
        input_queue_->Pop(context);
        std::shared_ptr<AlgoFrame> frame = std::dynamic_pointer_cast<algo::core::AlgoFrame>(context);

        if (frame->cvImage.empty()) {
            LOG(INFO) << " queue size: " << input_queue_->Size();
            std::this_thread::sleep_for(std::chrono::seconds(2));
            continue;
        }

        std::vector<Box> object_boxes;
        std::shared_ptr<Event> ent = std::make_shared<LoggerEvent>(frame->camera_sn, frame->camera_time);
        output_queue_->Push(ent);

        std::shared_ptr<AlgoData>data = std::make_shared<AlgoData>(object_boxes, frame);
        for (auto& kv : next_node_unit_) kv.second->Push(data);
    }
    LOG(INFO) << node_name_ << " exit .......";
}

}  // namespace vision
}  // namespace algo
