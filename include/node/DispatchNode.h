#pragma once

#include <chrono>
#include <thread>
#include "DynamicFactory.h"
#include "node/Node.h"
#include "event/Event.h"

namespace algo {
namespace vision {
class Process;

class DispatchNode : public Node, DynamicCreator<DispatchNode> {
 public:
	DispatchNode() : Node(ALGO_NODE_DISPATCH) {}
	virtual ~DispatchNode() {}
	virtual void run();

	virtual void init();
	void dispatch(std::shared_ptr<Event> event_ptr);
};
}	 // namespace vision
}	 // namespace algo