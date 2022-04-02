#pragma once


#include <iostream>
#include <memory>
#include <queue>
#include <condition_variable>

#include "mgr/ObjectManager.h"
#include "MetaObject.h"

namespace meta {
namespace vision {
// we use std::queue implement, we also can use folly::mcmpqueue


template<typename T>
class Queue{
public:
    Queue(int cap){
        m_capacity = cap;
        m_not_full =true;
        m_queue_ptr = std::make_unique<std::queue<T>>();
    }
    ~Queue(){}

    bool Push(const T obj){
        std::unique_lock<std::mutex> lck(m_mtx);

        m_cond_var.wait(lck, [&](){return m_not_full && !Full();});
        m_queue_ptr->push(obj);
        m_not_empty = true;

        lck.unlock();
        m_cond_var.notify_one();
        return false;
    }

    void Pop(T& a){

        std::unique_lock<std::mutex> lck(m_mtx);

        m_cond_var.wait(lck, [&](){return m_not_empty;});

        a = m_queue_ptr->front();
        m_queue_ptr->pop();
        m_not_full = true;

        if (Empty())
            m_not_empty =false;
        lck.unlock();
        m_cond_var.notify_one();
        return;
    }

    size_t Size(){
        return m_queue_ptr->size();
    }

    bool Full(){
        return m_queue_ptr->size() == m_capacity;
    }

    bool Empty(){
        return m_queue_ptr->empty();
    }


private:
    size_t m_capacity;
    std::unique_ptr<std::queue<T>> m_queue_ptr;
    std::condition_variable m_cond_var;
    std::mutex m_mtx;
    bool m_not_empty;
    bool m_not_full;
};


//template <typename T>
//class Queue {
// public:
//    Queue(int size, bool wait = true) : wait_(wait), cap_(size) {
//        queue_impl_ = std::make_unique<std::queue<T>>();
//    }
//    bool Push(T&& t, bool block = true) {
//        // todo default lost some data.
//        if (Size() >= cap_) {
//            return false;
//        }
//        queue_impl_->push(t);
//        return true;
//    }
//    bool Pop(T& t, bool block = true) {
//        t = queue_impl_->front();
//        queue_impl_->pop();
//        return true;
//    }
//    bool Empty(){return queue_impl_->empty();}
//    void setWaitStatus(bool wait) { wait_ = wait; }
//    int Size() { return queue_impl_->size(); }
//    void setCapicaty(int cap) { cap_ = cap; }
//
// private:
//    bool wait_;
//    int cap_;
//    std::unique_ptr<std::queue<T>> queue_impl_;
//};

//template <>
//class Queue<std::shared_ptr<meta::vision::MetaObject>> {
// public:
//    Queue(int size, bool wait = true) : wait_(wait), cap_(size) {
//        queue_impl_ = std::make_unique<std::queue<std::shared_ptr<meta::vision::MetaObject>>>();
//    }
//    bool Push(std::shared_ptr<meta::vision::MetaObject>&& t, bool block = true) {
//        if (Size() >= cap_) {
//            // todo default lost some data.
//            //  std::cout<<__TIMESTAMP__<<"  ["<< __FILE__<<": " <<__LINE__<<"]  " << " this queue is full, now lost it.... " ;
//            return false;
//        }
//        queue_impl_->push(t);
//        return true;
//    }
//    bool Pop(std::shared_ptr<meta::vision::MetaObject>& t, bool block = true) {
//        t = queue_impl_->front();
//        queue_impl_->pop();
//        return true;
//    }
//    bool Empty() {return queue_impl_->empty();}
//    void setWaitStatus(bool wait) { wait_ = wait; }
//    int Size() { return queue_impl_->size(); }
//    void setCapicaty(int cap) { cap_ = cap; }
//
// private:
//    bool wait_;
//    int cap_;
//    std::unique_ptr<std::queue<std::shared_ptr<meta::vision::MetaObject>>> queue_impl_;
//};
using QueuePtr = std::shared_ptr<Queue<std::shared_ptr<meta::vision::MetaObject>>>;

class QueueManager : public SafeObjectManager<QueueManager, QueuePtr> {};
}  // namespace vision
}  // namespace meta