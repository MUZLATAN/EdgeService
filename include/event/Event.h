#pragma once


#include <opencv2/opencv.hpp>
#include <json/json.h>

#include "common.h"
#include "AlgoObject.h"
#include "mgr/ConfigureManager.h"

namespace algo {
namespace vision {

enum class EventType {
    kDefaultEvent = 0,  // 0
    kLoggerEvent,
    kMonitorEvent
};

class Event : public AlgoObject{
 public:
    template <typename... Type>
    Event(Type &&... value) {
        Event(std::forward<Type>(value)...);
        obj_type = 3;
    };

    Event(EventType e, int64_t camera_time = 0, std::string camera_sn = "")
        : et(e), timeStamp(camera_time), cameraSn(camera_sn){
            obj_type = 3;
        };
    virtual ~Event() {}
    virtual void handler() = 0;

    std::string toString() const {
        switch (et) {
            case EventType::kLoggerEvent:
                return "monitor";
            default:
                return "default";
        }
    };

 public:
    EventType et;
    int64_t timeStamp;
    std::string cameraSn;
};



class LoggerEvent : public Event {
 public:
    LoggerEvent(const std::string &camera_sn, int64_t camera_time)
        : Event(EventType::kLoggerEvent, camera_time, camera_sn){};
    void handler();


 public:
    std::string prefix_ = "monitor";
};

class MonitorEvent : public Event {
 public:
    MonitorEvent(const std::string &cameraSn, std::vector<int> &x_axis,
                 std::vector<int> &y_axis, std::vector<int> &id,
                 int64_t camera_time)
        : Event(EventType::kMonitorEvent, camera_time, cameraSn),
          x_axis(x_axis),
          y_axis(y_axis),
          id(id) {
        api_url = "";
    };
    void handler();

 public:
    std::string api_url;
    std::vector<int> x_axis;
    std::vector<int> y_axis;
    std::vector<int> id;
};

}  // namespace vision
}  // namespace algo
