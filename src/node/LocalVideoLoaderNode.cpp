#include <dirent.h>
#include <fnmatch.h>

#include <chrono>
//#include <opencv2/opencv.hpp>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <thread>
#include <vector>

#include "SolutionPipeline.h"
#include "mgr/ConfigureManager.h"
#include "mgr/QueueManager.h"
#include "node/DataLoaderNode.h"

#define VIDEOFORMAT ".mp4"

namespace meta {
namespace vision {
void LocalVideoLoaderNode::init() {
    QueueManager::SafeGet(META_NODE_DETECT, output_queue_);
}

void LocalVideoLoaderNode::run() {
    while (true) {
        if (gt->sys_quit) {
            return;
        }

        auto frame = std::make_shared<meta::core::AlgoFrame>();
        capture_->read(frame->cvImage);

        frame_id_++;
        frame->setCurrentTime();
//        frame->camera_time = frame_id_;
        frame->camera_sn = gt->camera_sns;
        frame->frame_id = frame_id_;

        output_queue_->Push(frame, false);
        is_ready_ = true;
        std::this_thread::sleep_for(std::chrono::milliseconds(5000));
    }
}


}  // namespace vision
}  // namespace meta