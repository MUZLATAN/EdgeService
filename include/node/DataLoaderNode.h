#pragma once
#include <mutex>
#include <opencv2/videoio.hpp>

#include "DynamicFactory.h"
#include "common.h"
#include "network/HttpClient.h"

namespace algo {
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

class UsbDeviceLoaderNode
    : public DataLoaderNode,
      DynamicCreator<UsbDeviceLoaderNode, std::string&> {
 public:
    UsbDeviceLoaderNode(std::string& usb_device)
        : DataLoaderNode(ALGO_NODE_USBCAMERA),
          usb_device_(usb_device),
          is_ready_(false),
          frame_id_(0),
          global_frame_id_(0) {}
    virtual ~UsbDeviceLoaderNode() {}
    virtual void init();
    virtual void run();

 private:
    int findCamera();
    void reopenCamera();
    void selectResolution();
    bool infoCamera(const std::string& devName = "/dev/video0");
    void rotateImage(cv::Mat& src_img, int degree, cv::Mat& des_img);

 private:
    std::string usb_device_;
    bool is_ready_;
    std::unique_ptr<cv::VideoCapture> capture_;
    std::vector<std::pair<int, int>> suppered_resolution_vec_;
    int64_t frame_id_ = 0;
    int64_t global_frame_id_ = 0;
    int height_;
    int width_;
    int image_rotate_angle_ = 0;
};

class RtspLoaderNode : public DataLoaderNode,
                            DynamicCreator<RtspLoaderNode, std::string&> {
 public:
    RtspLoaderNode(std::string& rtsp)
        : DataLoaderNode(ALGO_NODE_RTSP), rtsp_stream_addr_(rtsp) {
           frame = std::make_shared<algo::core::AlgoFrame>();
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
    std::shared_ptr<algo::core::AlgoFrame>  frame;
};

class LocalVideoLoaderNode
    : public DataLoaderNode,
      DynamicCreator<LocalVideoLoaderNode, std::string&> {
 public:
    LocalVideoLoaderNode(std::string& local_video_file)
        : DataLoaderNode(ALGO_NODE_LOCALVIDEO),is_ready_(false) {}
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
}  // namespace algo
