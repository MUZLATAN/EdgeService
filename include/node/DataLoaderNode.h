#pragma once
#include <mutex>
#include <opencv2/videoio.hpp>

#include "DynamicFactory.h"
#include "common.h"
#include "network/HttpClient.h"

namespace meta {
namespace vision {
class DataLoaderNode
    : public Node,
      DynamicCreator<DataLoaderNode, const std::string&> {
 public:
    DataLoaderNode(const std::string& name) : Node(name) {}
    virtual ~DataLoaderNode() {}
    virtual void init() {}
    virtual void run() {}
    void updateConf(){};

 public:
    std::mutex mt_;
    int recordNum = 1;
};

class RtspLoaderNode : public DataLoaderNode,
                            DynamicCreator<RtspLoaderNode, std::string&> {
 public:
    RtspLoaderNode(std::string& rtsp)
        : DataLoaderNode(META_NODE_RTSP), rtsp_stream_addr_(rtsp) {
           frame = std::make_shared<meta::core::AlgoFrame>();
        }
    virtual ~RtspLoaderNode() {}
    virtual void init();
    virtual void run();

 private:
    void reopenCamera();

 private:
    std::string rtsp_stream_addr_;
    bool is_ready_;
    std::unique_ptr<cv::VideoCapture> capture_;
    int64_t frame_id_ = 0;
    std::shared_ptr<meta::core::AlgoFrame>  frame;
};

class LocalVideoLoaderNode
    : public DataLoaderNode,
      DynamicCreator<LocalVideoLoaderNode, std::string&> {
 public:
    LocalVideoLoaderNode(std::string& local_video_file)
        : DataLoaderNode(META_NODE_LOCALVIDEO),is_ready_(false) {}
    virtual ~LocalVideoLoaderNode() {}
    void init();
    void run();

 private:
    bool is_ready_;
    std::unique_ptr<cv::VideoCapture> capture_;
    std::vector<std::string> localVideoList_;
    int64_t frame_id_ = 0;
    int loadVideo();
};

}  // namespace vision
}  // namespace meta
