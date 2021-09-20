#pragma once

#include <map>
#include <opencv2/opencv.hpp>

#include "DynamicFactory.h"
#include "node/Node.h"
#include <opencv2/opencv.hpp>
#include "common.h"

namespace algo {
namespace vision {

class DetectorNode : public Node, DynamicCreator<DetectorNode> {
 public:
    DetectorNode() : Node(ALGO_NODE_DETECT) {}
    virtual ~DetectorNode() {}
    virtual void run();
    virtual void init();

 private:
    void labelBox(cv::Mat& rgbImage,
                  std::vector<algo::core::Box>& det_results);
    cv::Mat template_;
    cv::Mat template2_;
    std::vector<cv::Mat> circles_;
    int frame_idx_ = -1;
    int64_t last_hasface_time_ = 0;

 private:
    std::map<std::string, QueuePtr> next_node_unit_;
    int64_t last_image_monitor_time_;
    int64_t last_model_metric_monitor_time_;
	int64_t image_monitor_interval;

    bool algo_door_ = false;
};
}  // namespace vision
}  // namespace algo