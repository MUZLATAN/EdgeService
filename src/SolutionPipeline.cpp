
#include "SolutionPipeline.h"

// #include <folly/executors/CPUThreadPoolExecutor.h>
// #include <folly/executors/GlobalExecutor.h>
// #include <folly/executors/IOThreadPoolExecutor.h>
// #include <folly/futures/Future.h>
// #include <folly/init/Init.h>
#include <glog/logging.h>


#include <functional>

#include "mgr/ConfigureManager.h"
#include "node/DataLoaderNode.h"
#include "node/head.h"


#define __MAX_IMAGE_QUEUE_SIZE__ 10  


namespace algo {
namespace vision {

std::vector<std::string> SolutionPipeline::global_executor_names_ = {

    ALGO_NODE_DISPATCH, ALGO_NODE_FLOWRPC};

std::unordered_map<std::string, std::shared_ptr<Node>>
    SolutionPipeline::global_executors_;

std::vector<std::string> SolutionPipeline::global_core_executor_names_ = {
    ALGO_NODE_DETECT};

std::unordered_map<std::string, std::shared_ptr<Node>>
    SolutionPipeline::global_core_executors_;

std::vector<std::string> SolutionPipeline::global_video_input_;
std::unordered_map<std::string, std::shared_ptr<Node>>
    SolutionPipeline::global_video_input_executors_;

std::vector<std::shared_ptr<std::thread>> SolutionPipeline::vec_thread;

template <typename... Targs>
static std::shared_ptr<Node> buildNode(std::string& name,
                                                 const std::string& next,
                                                 Targs&&... params) {
    std::string node_name = "algo::vision::" + name;
    LOG(INFO) << " load node: " << node_name;
    auto ptr = NodeBuild::MakeSharedNode(node_name, params...);

    if (!ptr) {
        // todo log
        //*todo crash log
        LOG(FATAL) << node_name
                   << " dynamic create error because the node don't "
                      "register, user don't implement the node.";
    }

    ptr->setNextNode(next);

    LOG(INFO) << " this node queue_name: " << name;
    auto queue = std::make_shared<Queue<std::shared_ptr<algo::vision::AlgoObject>>>(__MAX_IMAGE_QUEUE_SIZE__);

    QueueManager::SafeAdd(name, queue);
    LOG(INFO) << "  after this node queue_name: " << name;


    return ptr;
}

void SolutionPipeline::Build() {
    std::vector<std::string> solution_process_vec = feature_map[solution_name_];
    for (int i = 0; i < solution_process_vec.size(); ++i) {
        std::string next;
        if (i != solution_process_vec.size() - 1) {
            next = solution_process_vec[i + 1];
        } else {
            LOG(INFO) << "solution last node: " << solution_process_vec[i];
            next = ALGO_NODE_DISPATCH;
        }

        auto ptr = buildNode(solution_process_vec[i], next);
        executors_.emplace_back(ptr);
        ptr->init();
    }
}

void SolutionPipeline::BuildVideoInput() {
    LOG(INFO) << "build video input.";
    global_video_input_ =
        ConfigureManager::instance()->getAsVectorString("video_input");

    if (global_video_input_.size() < 1) {
        LOG(FATAL) << " solution don't assigin video/rtst stream.";
        // todo log to chunnel
    }

    for (auto iter = global_video_input_.begin();
         iter != global_video_input_.end(); ++iter) {
        std::string data_loader_name;
        // todo may be local video file
        if ((*iter).find("rtsp") != (*iter).npos) {
            // rtsp
            data_loader_name = ALGO_NODE_RTSP;
        } else if ((*iter).find("/dev") != (*iter).npos) {
            // usb
            data_loader_name = ALGO_NODE_USBCAMERA;

        } else if ((*iter).find("rtmp") != (*iter).npos) {
            // rtmp
            LOG(INFO) << "build rtmp input.";
        } else {
            // local video file
            data_loader_name = ALGO_NODE_LOCALVIDEO;
        }

        auto ptr =
            buildNode(data_loader_name, ALGO_NODE_DETECT, *iter);
        global_video_input_executors_[*iter] = ptr;
    }
}

void SolutionPipeline::BuildAndStartCoreGlobal() {
    for (auto iter = global_core_executor_names_.begin();
         iter != global_core_executor_names_.end(); iter++) {
        std::string node_name = "algo::vision::" + (*iter);
        LOG(INFO) << " core node: " << node_name;

        std::string next;
        auto ptr = buildNode(*iter, next);

        global_core_executors_[*iter] = ptr;

        if (*iter == ALGO_NODE_DETECT) {
            // set next node
        }

        ptr->init();

        int idx = GetGlobalVariable()->g_thread.size();
        GetGlobalVariable()->g_thread.push_back(std::thread([&](){
            LOG(INFO) << ptr->getNodeName() << " start run.";
            return ptr->run();
        }));
        GetGlobalVariable()->g_thread[idx].detach();
    }
}

void SolutionPipeline::BuildAndStartGlobal() {
    std::vector<std::string> features =
        ConfigureManager::instance()->getAsVectorString("feature");

    for (auto iter = global_executor_names_.begin();
         iter != global_executor_names_.end(); iter++) {
        auto ptr = buildNode(*iter, "");
        global_executors_[*iter] = ptr;
        ptr->init();

        int idx = GetGlobalVariable()->g_thread.size();
        std::thread t([&](){
            LOG(INFO) << ptr->getNodeName() << " start run.";
            ptr->run();
        });
        GetGlobalVariable()->g_thread.emplace_back(std::move(t));
        GetGlobalVariable()->g_thread[idx].detach();

    }

    // init global callback function
    auto node =
        SolutionPipeline::global_executors_[ALGO_NODE_FLOWRPC];
    FlowRpcAsynNode_flow =
        dynamic_cast<FlowRpcAsynNode*>(node.get());
}

void SolutionPipeline::Start() {
    for (auto iter = executors_.begin(); iter != executors_.end(); ++iter) {
        LOG(INFO) << " thread start: " << (*iter)->getNodeName();
//        folly::via(folly::getCPUExecutor().get()).thenValue([iter](auto&&) {
//            folly::setThreadName((*iter)->getNodeName());
//            return (*iter)->run();
//        });
        int idx = GetGlobalVariable()->g_thread.size();

        GetGlobalVariable()->g_thread.push_back(std::thread([&](){
            LOG(INFO) << (*iter)->getNodeName() << " start run.";
            return (*iter)->run();
        }));
        GetGlobalVariable()->g_thread[idx].detach();


    }
}

void SolutionPipeline::StopAll() { GetGlobalVariable()->sys_quit = true; }

void SolutionPipeline::StartVideoInput() {
    for (auto iter = global_video_input_executors_.begin();
         iter != global_video_input_executors_.end(); ++iter) {
        LOG(INFO) << "video input thread start: "
                  << iter->second->getNodeName();
//        folly::via(folly::getCPUExecutor().get()).thenValue([iter](auto&&) {
//            iter->second->init();
//            folly::setThreadName(iter->second->getNodeName());
//            return iter->second->run();
//        });
//        std::thread t();
//        t.detach();
        int idx = GetGlobalVariable()->g_thread.size();

        GetGlobalVariable()->g_thread.push_back(std::thread([&](){
            LOG(INFO) << iter->second->getNodeName() << " start run.";
            return iter->second->run();
        }));
        GetGlobalVariable()->g_thread[idx].detach();


    }
}
}  // namespace vision
}  // namespace algo
