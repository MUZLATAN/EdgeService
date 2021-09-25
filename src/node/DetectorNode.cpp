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

const unsigned char LabelColors[][3] = {
    {2, 224, 17}, {251, 144, 17}, {206, 36, 255}, {247, 13, 145}, {0, 78, 255}};

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

bool EndsWith(std::string const& s, std::string const& ending) {
    return false;
}

void GetDateTime(std::string& date, std::string& time) {
    auto t =
        std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
    std::stringstream ss;
    ss << std::put_time(std::localtime(&t), "%m-%d ");
    date = ss.str();

    std::stringstream ss2;
    ss2 << std::put_time(std::localtime(&t), "%A");
    date += ss2.str().substr(0, 3);

    std::stringstream ss3;
    ss3 << std::put_time(std::localtime(&t), "%T");
    time = ss3.str();
}

void GetTimeGreetings(const std::string& time, std::string& greet) {
}

void DetectorNode::labelBox(cv::Mat& rgbImage,
                                 std::vector<Box>& det_results) {
}

void DetectorNode::run() {
    while (true) {
        if (gt->sys_quit) {
            break;
        }

        LOG(INFO)<<"here detects";

        // std::shared_ptr<algo::vision::AlgoObject> context;
        // if (input_queue_->Empty()){
        //     std::this_thread::sleep_for(std::chrono::seconds(5));
        //     continue;
        // }
        // LOG(INFO)<<"DetectNode read from the queue";
	    // input_queue_->Pop(context);
	    // std::shared_ptr<AlgoFrame> frame =
	    //         std::dynamic_pointer_cast<algo::core::AlgoFrame>(context);

        // if (frame->cvImage.empty()) {
        //     LOG(INFO) << " queue size: " << input_queue_->Size();
        //     std::this_thread::sleep_for(std::chrono::seconds(2));
        //     continue;
        // }


        // std::vector<Box> object_boxes;
        // int64_t cur_time = getCurrentTime();
        // // algo::sdk::detect(frame, object_boxes);
        // std::this_thread::sleep_for(std::chrono::seconds(5));
        // int64_t duration = getCurrentTime() - cur_time;

        // std::shared_ptr<Event> ent = std::make_shared<LoggerEvent>(
        //         frame->camera_sn, frame->camera_time);
        // output_queue_->Push(ent);
        // LOG(INFO)<<"push logger event";

        // LOG(INFO)<<"DispatchNode push queue";
        // std::shared_ptr<AlgoData>data = std::make_shared<AlgoData>(object_boxes, frame);
        // for (auto& kv : next_node_unit_) kv.second->Push(data);
    }
    LOG(INFO) << node_name_ << " exit .......";
}

}  // namespace vision
}  // namespace algo
