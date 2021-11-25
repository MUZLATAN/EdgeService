#include <dirent.h>
#include <fnmatch.h>

#include <chrono>
#include <opencv2/opencv.hpp>
#include <thread>

#include "mgr/ConfigureManager.h"
#include "mgr/QueueManager.h"
#include "node/DataLoaderNode.h"

namespace meta {
namespace vision {

void RtspLoaderNode::init() {
    // todo
    QueueManager::SafeGet(META_NODE_DETECT, output_queue_);

    capture_ = std::make_unique<cv::VideoCapture>(rtsp_stream_addr_);
    capture_->set(cv::CAP_PROP_BUFFERSIZE, 3);
    capture_->set(cv::CAP_PROP_FPS, 15);

    std::cout<<__TIMESTAMP__<<"  ["<< __FILE__<<": " <<__LINE__<<"]  " << "rtsp init done";
}

void RtspLoaderNode::reopenCamera() {
    while (true) {
        is_ready_ = false;
        if (capture_) {
            capture_->release();
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        std::cout<<__TIMESTAMP__<<"  ["<< __FILE__<<": " <<__LINE__<<"]  " << "******Trying to reconnect rtsp camera now****";

        bool opened = capture_->open(rtsp_stream_addr_);

        if (!capture_->isOpened()) {
            std::this_thread::sleep_for(std::chrono::seconds(1));
            continue;
        }

        cv::Mat frame;
        capture_->read(frame);
        if (frame.empty()) {
            std::cout<<__TIMESTAMP__<<"  ["<< __FILE__<<": " <<__LINE__<<"]  " << "opened: " << frame.cols << " " << frame.rows;
            continue;
        }
        is_ready_ = true;
        return;
    }
}

void RtspLoaderNode::run() {
    int cnt = 0;

    while (true) {
        if (gt->sys_quit) {
            return;
        }
        frame_id_++;
        cnt = output_queue_->Size();
        std::this_thread::sleep_for(std::chrono::seconds(5*cnt));
        capture_->read(frame->cvImage);

        if (frame->cvImage.empty()) {
            std::cout<<__TIMESTAMP__<<"  ["<< __FILE__<<": " <<__LINE__<<"]  "<<"image empty"<<std::endl;
            reopenCamera();
            continue;
        }
        frame->setCurrentTime();
        frame->camera_sn = gt->camera_sns;
        frame->frame_id = frame_id_;
        output_queue_->Push(frame, false);

    }
    std::cout<<__TIMESTAMP__<<"  ["<< __FILE__<<": " <<__LINE__<<"]  " << node_name_ << " exit .......";
}

}  // namespace vision
}  // namespace meta
