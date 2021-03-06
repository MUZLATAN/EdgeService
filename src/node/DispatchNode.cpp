
#include "node/DispatchNode.h"


#include "mgr/QueueManager.h"

namespace meta {
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

         std::shared_ptr<meta::vision::MetaObject> message;

	     input_queue_->Pop(message);
	     std::shared_ptr<Event> event = std::dynamic_pointer_cast<Event>(message);
         dispatch(event);
    }
    std::cout<<__TIMESTAMP__<<"  ["<< __FILE__<<": " <<__LINE__<<"]  " << node_name_ << " Exit .......";
}
}  // namespace vision
}  // namespace meta