#pragma once


#include <iostream>
#include <memory>
#include <queue>

#include "mgr/ObjectManager.h"
#include "AlgoObject.h"

namespace algo {
namespace vision {
// we use std::queue implement, we also can use

// or std::queue
template <typename T>
class Queue {
 public:
    Queue(int size, bool wait = true) : wait_(wait), cap_(size) {
        queue_impl_ = std::make_unique<std::queue<T>>();
    }
    bool Push(T&& t, bool block = true) {
        // todo default lost some data.
        if (Size() >= cap_) {
            // LOG(INFO) << " common cap_ full " ;
            return false;
        }
        queue_impl_->push(t);
        return true;
    }
    bool Pop(T& t, bool block = true) {
        t = queue_impl_->front();
        queue_impl_->pop();
        return true;
    }
    bool Empty(){return queue_impl_->empty();}
    void setWaitStatus(bool wait) { wait_ = wait; }
    int Size() { return queue_impl_->size(); }
    void setCapicaty(int cap) { cap_ = cap; }

 private:
    bool wait_;
    int cap_;
    std::unique_ptr<std::queue<T>> queue_impl_;
};

template <>
class Queue<std::shared_ptr<algo::vision::AlgoObject>> {
 public:
    Queue(int size, bool wait = true) : wait_(wait), cap_(size) {
        queue_impl_ = std::make_unique<std::queue<std::shared_ptr<algo::vision::AlgoObject>>>();
    }
    bool Push(std::shared_ptr<algo::vision::AlgoObject>&& t, bool block = true) {
        if (Size() >= cap_) {
            // todo default lost some data.
            //  LOG(INFO) << " this queue is full, now lost it.... " ;
            return false;
        }
        queue_impl_->push(t);
        return true;
    }
    bool Pop(std::shared_ptr<algo::vision::AlgoObject>& t, bool block = true) {
        t = queue_impl_->front();
        queue_impl_->pop();
        return true;
    }
    bool Empty() {return queue_impl_->empty();}
    void setWaitStatus(bool wait) { wait_ = wait; }
    int Size() { return queue_impl_->size(); }
    void setCapicaty(int cap) { cap_ = cap; }

 private:
    bool wait_;
    int cap_;
    std::unique_ptr<std::queue<std::shared_ptr<algo::vision::AlgoObject>>> queue_impl_;
};
using QueuePtr = std::shared_ptr<Queue<std::shared_ptr<algo::vision::AlgoObject>>>;

class QueueManager : public SafeObjectManager<QueueManager, QueuePtr> {};
}  // namespace vision
}  // namespace algo