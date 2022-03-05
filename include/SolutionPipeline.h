#pragma once

#include <iostream>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>
#include <thread>
#include <unistd.h>

#include "mgr/ObjectManager.h"
#include "mgr/QueueManager.h"
#include "node/Node.h"
#include "util.h"

namespace meta {
namespace vision {

class Node;
class SolutionPipeline {
 public:
    SolutionPipeline(const std::string& solution_name)
        : solution_name_(solution_name) {}

    static void BuildVideoInput();
    static void StartVideoInput();

    static void BuildAndStartGlobal();

    // the finally build core global executors.
    static void BuildAndStartCoreGlobal();

    static void StartStaticExecutors();
    static void StopAll();

    void AddExecutors();
    void Build();

    void Start();
    void ResetPipeline();
    void RestartExecutor(const std::string& executor_name);
    void StopExecutor(const std::string& executor_name);

 public:
    static std::vector<std::shared_ptr<std::thread>> vec_thread;

 public:
    std::string solution_name_;
    std::string device_name_;
    std::vector<std::string> device_name_vec_;
    std::vector<std::shared_ptr<Node>> executors_;
    static std::vector<std::string> global_executor_names_;
    static std::unordered_map<std::string, std::shared_ptr<Node>>
        global_executors_;
    static std::vector<std::string> global_data_input_;
    static std::unordered_map<std::string, std::shared_ptr<Node>>
        global_data_input_executors_;

    // build seq
    static std::vector<std::string> global_core_executor_names_;
    static std::unordered_map<std::string, std::shared_ptr<Node>>
        global_core_executors_;
};

using SolutionPipelinePtr = std::shared_ptr<SolutionPipeline>;

class SolutionPipelineManager
    : public SafeObjectManager<SolutionPipelineManager,
                               std::shared_ptr<SolutionPipeline>> {};
}  // namespace vision
}  // namespace meta
