#pragma once
#include <execinfo.h>
// #include <folly/executors/CPUThreadPoolExecutor.h>
// #include <folly/executors/GlobalExecutor.h>
// #include <folly/executors/IOThreadPoolExecutor.h>
// #include <folly/init/Init.h>
#include <signal.h>
#include <thread>
#include "SolutionPipeline.h"
#include "app/ServiceDaemon.h"
#include "mgr/ConfigureManager.h"


void sig_proc(int sig) {
    LOG(INFO) << "ERROR: in sig proc function !!!! sig is:" << sig;
    algo::vision::SolutionPipeline::StopAll();
    std::this_thread::sleep_for(std::chrono::seconds(1));
    exit(0);
}

int main(int argc, char** argv) {
    // folly::init(&argc, &argv);
    gflags::ParseCommandLineFlags(&argc, &argv, true);
    FLAGS_alsologtostderr = 1;
    FLAGS_stderrthreshold = 0;
    FLAGS_minloglevel = 0;
    FLAGS_max_log_size = 40;
    FLAGS_logbufsecs = 0;  

    // signal(SIGKILL, sig_proc);
    // signal(SIGINT, sig_proc);
    // signal(SIGTERM, sig_proc);
    // signal(SIGSEGV, sig_proc);

    algo::vision::ServiceDaemon app(argc, argv);
    app.Loop();
    //
    std::this_thread::sleep_until(
        std::chrono::time_point<std::chrono::system_clock>::max());

    return 0;
}