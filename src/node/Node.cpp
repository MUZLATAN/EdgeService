#include "node/Node.h"
#include "SolutionPipeline.h"

namespace meta {
namespace vision {
void Node::init() {
    QueueManager::SafeGet(next_node_name_, output_queue_);
    if (!output_queue_) std::cout<<__TIMESTAMP__<<"  ["<< __FILE__<<": " <<__LINE__<<"]  " << node_name_ << " Get Queue error."<<std::endl;

    std::cout<<__TIMESTAMP__<<"  ["<< __FILE__<<": " <<__LINE__<<"]  " << node_name_ << " base Node init.. "
              << "current node: " << node_name_
              << " Next node: " << next_node_name_;
}

}  // namespace vision
}  // namespace meta