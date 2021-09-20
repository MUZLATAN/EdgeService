#include "node/Node.h"

#include <glog/logging.h>

#include "SolutionPipeline.h"

namespace algo {
namespace vision {
void Node::init() {
    QueueManager::SafeGet(next_node_name_, output_queue_);
    if (!output_queue_) LOG(FATAL) << node_name_ << " Get Queue error.";

    LOG(INFO) << node_name_ << " base Node init.. "
              << "current node: " << node_name_
              << " Next node: " << next_node_name_;
}

}  // namespace vision
}  // namespace algo