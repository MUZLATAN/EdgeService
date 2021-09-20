#pragma once

#include <chrono>
#include <iostream>
#include <memory>
#include <set>
#include <string>
#include <vector>

#include "common.h"
#include "mgr/QueueManager.h"

namespace algo {
namespace vision {

class Node {
 public:
    Node(const std::string& name) : node_name_(name) {
        gt = GetGlobalVariable();
    }
    virtual ~Node() { LOG(INFO) << node_name_ << " exit."; }
    virtual void init();
    virtual void run() = 0;
    virtual void updateConf(){};
    void print() { LOG(INFO) << "print info " << node_name_; }
    std::string getNodeName() { return node_name_; }
    void setNextNode(const std::string& netx_node) {
        next_node_name_ = netx_node;
    }
    int64_t getCurrentTime() {
        return std::chrono::system_clock::now().time_since_epoch() /
               std::chrono::milliseconds(1);
    }

 protected:
    std::string node_name_;
    std::string next_node_name_;
    QueuePtr input_queue_;
    QueuePtr output_queue_;

    GlobalVariable* gt = nullptr;
};

}  // namespace vision
}  // namespace algo