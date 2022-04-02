#include "node/MonitorNode.h"

#include <algorithm>
#include <fstream>
#include <thread>

#include "event/Event.h"
#include "mgr/ConfigureManager.h"
#include "mgr/QueueManager.h"
#include "util.h"

using namespace meta::core;

namespace meta {
namespace vision {

void MonitorNode::init() {
    std::cout<<__TIMESTAMP__<<"  ["<< __FILE__<<": " <<__LINE__<<"]  "<<"MonitorNode init  and get queue"<<std::endl;
    QueueManager::SafeGet(node_name_, input_queue_);
    QueueManager::SafeGet(META_NODE_DISPATCH, output_queue_);
    monitor_interval =
        ConfigureManager::instance()->getAsInt("monitor_interval");
    cur_interval_cnt_ = 0;

    cls_id_map = {{"cls_ped", 1}, {"cls_face", 2}, {"cls_head", 3}};



    need_visualize = false;
}

void MonitorNode::run() {
    while (true) {
        if (gt->sys_quit) {
            break;
        }
        std::cout<<__TIMESTAMP__<<"  ["<< __FILE__<<": " <<__LINE__<<"]  " << node_name_ << " run ..."<<std::endl;

	     std::shared_ptr<meta::vision::MetaObject> context;
	     input_queue_->Pop(context);
	     std::shared_ptr<MetaData> message =
	             std::dynamic_pointer_cast<MetaData>(context);

         SendMsg(message);
         // 手动快速释放内存
	     if( message != nullptr && message->frame_ != nullptr ){
	             message->frame_->cvImage.release();
	     }
    }
    std::cout<<__TIMESTAMP__<<"  ["<< __FILE__<<": " <<__LINE__<<"]  " << node_name_ << " exit ......."<<std::endl;
}
void MonitorNode::SendMsg(std::shared_ptr<MetaData> message){
    std::vector<int> x_ {1, 2, 3};
    std::vector<int> y_ {4, 6,8};
    std::vector<int> id_{7, 8, 9};
    std::shared_ptr<Event> pae =
            std::make_shared<MonitorEvent>(message->frame_->camera_sn, x_, y_, id_, message->frame_->camera_time);

        std::cout<<__TIMESTAMP__<<"  ["<< __FILE__<<": " <<__LINE__<<"]  " << "push Monitor event."<<std::endl;
        output_queue_->Push(pae);
}

}  // namespace vision
}  // namespace meta
