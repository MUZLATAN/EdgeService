
#include "node/DispatchNode.h"


#include "mgr/QueueManager.h"

namespace algo {
namespace vision {

void DispatchNode::init() {
    // QueueManager::SafeGet(node_name_, input_queue_); 
    QueueManager::SafeGet("DispatchNode", input_queue_);
}

void DispatchNode::dispatch(std::shared_ptr<Event> event) {
    event->handler();
}
void DispatchNode::run() {
    while (true) {
        if (gt->sys_quit) {
            break;
        }
        LOG(INFO)<<"dispatch here";
        // std::shared_ptr<algo::vision::AlgoObject> message;
	    // //todo tmp debug code
        // if (input_queue_->Empty()) {
        //     std::this_thread::sleep_for(std::chrono::seconds(5));
        //     continue;
        // }
	    // input_queue_->Pop(message);
	    // std::shared_ptr<Event> event =
	    //         std::dynamic_pointer_cast<Event>(message);
        // dispatch(event);
    }
    LOG(INFO) << node_name_ << " Exit .......";
}
}  // namespace vision
}  // namespace algo