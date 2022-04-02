#include "node/DetectorNode.h"

#include <algorithm>
#include <thread>

#include "event/Event.h"
#include "mgr/ConfigureManager.h"
#include "mgr/QueueManager.h"
#include "util.h"

using namespace meta::core;

namespace meta {
namespace vision {

void DetectorNode::init() {
    QueueManager::SafeGet(node_name_, input_queue_);
    QueueManager::SafeGet(META_NODE_DISPATCH, output_queue_);

    std::vector<std::string> features =
        ConfigureManager::instance()->getAsVectorString("feature");
    for (int i = 0; i < features.size(); ++i) {
        std::string base_node = feature_map[features[i]][0];
        QueuePtr q;
        QueueManager::SafeGet(base_node, q);
        next_node_unit_[base_node] = q;
        std::cout<<__TIMESTAMP__<<"  ["<< __FILE__<<": " <<__LINE__<<"]  " << base_node << " add to detectornode next queue. "<<std::endl;
    }
}

void DetectorNode::run() {
    while (true) {
        if (gt->sys_quit) {
            break;
        }

        std::shared_ptr<meta::vision::MetaObject> context;

        input_queue_->Pop(context);
        std::shared_ptr<AlgoFrame> frame = std::dynamic_pointer_cast<meta::core::AlgoFrame>(context);

        if (frame->cvImage.empty()) {
            std::cout<<__TIMESTAMP__<<"  ["<< __FILE__<<": " <<__LINE__<<"]  " << " queue size: " << input_queue_->Size()<<std::endl;
            std::this_thread::sleep_for(std::chrono::milliseconds(2));
            continue;
        }
        //可以注释
        std::vector<Box> object_boxes;
        std::shared_ptr<Event> ent = std::make_shared<LoggerEvent>("", 1648804066659);
        output_queue_->Push(ent);

        std::shared_ptr<MetaData>data = std::make_shared<MetaData>(object_boxes, frame);
        for (auto& kv : next_node_unit_) kv.second->Push(data);
    }
    std::cout<<__TIMESTAMP__<<"  ["<< __FILE__<<": " <<__LINE__<<"]  " << node_name_ << " exit ......."<<std::endl;
}

}  // namespace vision
}  // namespace meta
