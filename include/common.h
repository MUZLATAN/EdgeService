#pragma once

#include <chrono>
#include <opencv2/opencv.hpp>
#include <string>
#include <vector>
#include <MetaObject.h>
#include <memory>
#include <thread>
#include <stdarg.h>

#define META_SYS_PATH "sys_path"        // system path
#define ROOTPATH "../"

// NOTE: storage for node name
#define META_NODE_MONITOR "MonitorNode"
#define META_NODE_DETECT "DetectorNode"
#define META_NODE_USBCAMERA ""
#define META_NODE_DISPATCH "DispatchNode"
#define META_NODE_RTSP "RtspLoaderNode"
#define META_NODE_LOCALVIDEO "LocalVideoLoaderNode"
#define META_NODE_FLOWRPC "FlowRpcAsynNode"


#define MAX_LOG_MSG_LEN (1023)
#define TIME_FORMAT_STR_LEN (20)

void log_func(const char* time, const char *file, const int line, const char *fmt, ...);

#define LOG_I(fmt, ...) log_func(__TIMESTAMP__ ,__FILE__, __LINE__, fmt, ##__VA_ARGS__)

#ifndef __TYPEDEFINE_META_FRAME__
#define __TYPEDEFINE_META_FRAME__
namespace meta {
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


class BaseFrame : public meta::vision::MetaObject {
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
        // cv::resize(tmp, frame_->cvImage, dsize);
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
}  // namespace meta
#endif





namespace meta {
namespace vision {

class MetaData : public MetaObject{
 public:
    MetaData(){
        obj_type = 2;
    };
    MetaData(std::vector<meta::core::Box>& boxes,
              std::shared_ptr<meta::core::AlgoFrame> frame)
        : object_boxes(boxes), frame_(frame){
            obj_type = 2;
        };
 public:
    std::vector<meta::core::Box> object_boxes;
    std::shared_ptr<meta::core::AlgoFrame> frame_;
};

struct GlobalVariable {
 public:

    std::string meta_version = "";
    std::string sys_version = "";

    std::string sys_path = "";
    std::string client_sn = "";
    std::string camera_sns = "";    // Maybe multiple
    std::string data_inputs = "";  // Maybe multiple

    bool sys_quit = false;

    std::vector<std::thread> g_thread;
};

GlobalVariable* GetGlobalVariable(void);

}  // namespace vision
}  // namespace meta
