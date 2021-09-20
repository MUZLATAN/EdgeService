#pragma once

#include <chrono>
#include <opencv2/opencv.hpp>
#include <string>
#include <vector>
#include <AlgoObject.h>




#ifndef __TYPEDEFINE_ALGO_FRAME__
#define __TYPEDEFINE_ALGO_FRAME__
namespace algo {
namespace core {

struct landmark {
    float x[5];
    float y[5];
};
typedef struct Box {
    cv::Rect rect;
    float x0;
    float y0;
    float x1;
    float y1;
    float confidence;
    float score;
    int id;
    int class_idx;
    float landmark_score;
    struct landmark points;
    bool with_mask;

} Box;

enum CategoryId { cls_face = 2, cls_head = 3, cls_ped = 1 };


class BaseFrame : public algo::vision::AlgoObject {
 public:
    BaseFrame() { 
        setCurrentTime(); 
        obj_type = 1;
    };
    BaseFrame(const BaseFrame& f)
        : camera_time(f.camera_time),
          cvImage(f.cvImage),
          camera_sn(f.camera_sn),
          frame_id(f.frame_id){
              obj_type = 1;
          };
    int64_t getCurrentTime() { return camera_time; };
    virtual cv::Mat& getcvMat(void) { return cvImage; };
    // heigth
    virtual int row(void) { return cvImage.rows; };
    // width
    virtual int col(void) { return cvImage.cols; };

    void setCurrentTime() {
        camera_time = std::chrono::system_clock::now().time_since_epoch() /
                      std::chrono::milliseconds(1);
    };

    // NOTE: crop func may change the rect !
    virtual std::shared_ptr<BaseFrame> crop(cv::Rect rect) {
        std::shared_ptr<BaseFrame> frame_ = std::make_shared<BaseFrame>(*this);
        frame_->cvImage = cvImage(rect);
        return frame_;
    };

    virtual std::shared_ptr<BaseFrame> cropAndResize(cv::Rect rect,
                                                     cv::Size dsize) {
        std::shared_ptr<BaseFrame> frame_ = std::make_shared<BaseFrame>(*this);
        cv::Mat tmp;
        tmp = cvImage(rect);
        cv::resize(tmp, frame_->cvImage, dsize);
        return frame_;
    };

 public:
    int64_t camera_time = 0;
    cv::Mat cvImage;
    std::string camera_sn;  // TODO: is this variable usable?F
    unsigned long long frame_id = 0;
};


typedef BaseFrame AlgoFrame;

}  // namespace core
}  // namespace algo
#endif


#define ALGO_SYS_PATH "sys_path"        // system path


// NOTE: storage for node name
#define ALGO_NODE_MONITOR "MonitorNode"
#define ALGO_NODE_DETECT "DetectorNode"
#define ALGO_NODE_DISPATCH "DispatchNode"
#define ALGO_NODE_RTSP "RtspLoaderNode"
#define ALGO_NODE_USBCAMERA "UsbDeviceLoaderNode"
#define ALGO_NODE_LOCALVIDEO "LocalVideoLoaderNode"
#define ALGO_NODE_FLOWRPC "FlowRpcAsynNode"


#define HEARBEAT_URL ""

namespace algo {
namespace vision {

class AlgoData : public AlgoObject{
 public:
    AlgoData(){
        obj_type = 2;
    };
    AlgoData(std::vector<algo::core::Box>& boxes,
              std::shared_ptr<algo::core::AlgoFrame> frame)
        : object_boxes(boxes), frame_(frame){
            obj_type = 2;
        };
 public:
    std::vector<algo::core::Box> object_boxes;
    std::shared_ptr<algo::core::AlgoFrame> frame_;
};

struct GlobalVariable {
 public:
    std::string algo_version = "";
    std::string sys_version = "";
    std::string sys_path = "";
    std::string client_sn = "";
    std::string camera_sns = "";    // Maybe multiple
    std::string video_inputs = "";  // Maybe multiple

    // value
    std::string chunnel_endpoint = "";
    std::string flowrpc_url = "";
    bool isDevEnv = false;

    bool sys_quit = false;
};

GlobalVariable* GetGlobalVariable(void);

}  // namespace vision
}  // namespace algo
