
#include "SolutionPipeline.h"
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
    std::cout<<__TIMESTAMP__<<"  ["<< __FILE__<<": " <<__LINE__<<"]  " << " load node: " << node_name<<std::endl;
    auto ptr = NodeBuild::MakeSharedNode(node_name, params...);

    if (!ptr) {
        // todo log
        //*todo crash log
        std::cout<<__TIMESTAMP__<<"  ["<< __FILE__<<": " <<__LINE__<<"]  "<< node_name
                   << " dynamic create error because the node don't "
                      "register, user don't implement the node."<<std::endl;
    }

    ptr->setNextNode(next);

    std::cout<<__TIMESTAMP__<<"  ["<< __FILE__<<": " <<__LINE__<<"]  " << " this node queue_name: " << name<<std::endl;
    auto queue = std::make_shared<Queue<std::shared_ptr<algo::vision::AlgoObject>>>(__MAX_IMAGE_QUEUE_SIZE__);

    QueueManager::SafeAdd(name, queue);
    std::cout<<__TIMESTAMP__<<"  ["<< __FILE__<<": " <<__LINE__<<"]  " << "  after this node queue_name: " << name<<std::endl;


    return ptr;
}

void SolutionPipeline::Build() {
    std::vector<std::string> solution_process_vec = feature_map[solution_name_];
    for (int i = 0; i < solution_process_vec.size(); ++i) {
        std::string next;
        if (i != solution_process_vec.size() - 1) {
            next = solution_process_vec[i + 1];
        } else {
            std::cout<<__TIMESTAMP__<<"  ["<< __FILE__<<": " <<__LINE__<<"]  " << "solution last node: " << solution_process_vec[i]<<std::endl;
            next = ALGO_NODE_DISPATCH;
        }

        auto ptr = buildNode(solution_process_vec[i], next);
        executors_.emplace_back(ptr);
        ptr->init();
    }
}

void SolutionPipeline::BuildVideoInput() {
    std::cout<<__TIMESTAMP__<<"  ["<< __FILE__<<": " <<__LINE__<<"]  " << "build video input."<<std::endl;
    global_video_input_ =
        ConfigureManager::instance()->getAsVectorString("video_input");

    if (global_video_input_.size() < 1) {
        std::cout<<__TIMESTAMP__<<"  ["<< __FILE__<<": " <<__LINE__<<"]  " << " solution don't assigin video/rtst stream."<<std::endl;
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
            std::cout<<__TIMESTAMP__<<"  ["<< __FILE__<<": " <<__LINE__<<"]  " << "build rtmp input."<<std::endl;
        } else {
            // local video file
            data_loader_name = ALGO_NODE_LOCALVIDEO;
        }

        auto ptr =
            buildNode(data_loader_name, ALGO_NODE_DETECT, *iter);
        ptr->init();
        global_video_input_executors_[*iter] = ptr;
    }
}

void SolutionPipeline::BuildAndStartCoreGlobal() {
    for (auto iter = global_core_executor_names_.begin();
         iter != global_core_executor_names_.end(); iter++) {
        std::string node_name = "algo::vision::" + (*iter);
        std::cout<<__TIMESTAMP__<<"  ["<< __FILE__<<": " <<__LINE__<<"]  " << " core node: " << node_name<<std::endl;

        std::string next;
        auto ptr = buildNode(*iter, next);

        global_core_executors_[*iter] = ptr;

        if (*iter == ALGO_NODE_DETECT) {
            // set next node
        }

        ptr->init();

        int idx = GetGlobalVariable()->g_thread.size();
        GetGlobalVariable()->g_thread.emplace_back(std::thread([&](){
            std::cout<<__TIMESTAMP__<<"  ["<< __FILE__<<": " <<__LINE__<<"]  " << ptr->getNodeName() << " start run."<<std::endl;
            return ptr->run();
        }));
        GetGlobalVariable()->g_thread[idx].detach();
        sleep(1);
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
            std::cout<<__TIMESTAMP__<<"  ["<< __FILE__<<": " <<__LINE__<<"]  " << ptr->getNodeName() << " start run."<<std::endl;
            ptr->run();
        });
        GetGlobalVariable()->g_thread.emplace_back(std::move(t));
        GetGlobalVariable()->g_thread[idx].detach();
        sleep(1);

    }

    // init global callback function
    auto node =
        SolutionPipeline::global_executors_[ALGO_NODE_FLOWRPC];
    FlowRpcAsynNode_flow =
        dynamic_cast<FlowRpcAsynNode*>(node.get());
}

void SolutionPipeline::Start() {
    for (auto iter = executors_.begin(); iter != executors_.end(); ++iter) {
        std::cout<<__TIMESTAMP__<<"  ["<< __FILE__<<": " <<__LINE__<<"]  " << " thread start: " << (*iter)->getNodeName()<<std::endl;
        int idx = GetGlobalVariable()->g_thread.size();
        GetGlobalVariable()->g_thread.push_back(std::thread([&](){
            std::cout<<__TIMESTAMP__<<"  ["<< __FILE__<<": " <<__LINE__<<"]  " << (*iter)->getNodeName() << " start run."<<std::endl;
            return (*iter)->run();
        }));
        GetGlobalVariable()->g_thread[idx].detach();
        sleep(1);
    }
}

void SolutionPipeline::StopAll() { GetGlobalVariable()->sys_quit = true; }

void SolutionPipeline::StartVideoInput() {
    for (auto iter = global_video_input_executors_.begin();
         iter != global_video_input_executors_.end(); ++iter) {
        std::cout<<__TIMESTAMP__<<"  ["<< __FILE__<<": " <<__LINE__<<"]  " << "video input thread start: "
                  << iter->second->getNodeName()<<std::endl;
        int idx = GetGlobalVariable()->g_thread.size();
        GetGlobalVariable()->g_thread.push_back(std::thread([&](){
            std::cout<<__TIMESTAMP__<<"  ["<< __FILE__<<": " <<__LINE__<<"]  " << iter->second->getNodeName() << " start run."<<std::endl;
            return iter->second->run();
        }));
        GetGlobalVariable()->g_thread[idx].detach();

        sleep(1);
    }
}
}  // namespace vision
}  // namespace algo
