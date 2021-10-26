
#include "app/ServiceDaemon.h"


#include <stdlib.h>


#include <chrono>
#include <fstream>
#include <memory>
#include <thread>

#include "SolutionPipeline.h"
#include "mgr/ConfigureManager.h"
#include "node/DynamicFactory.h"
#include "node/head.h"

using namespace algo::vision;
using namespace std::chrono;


ServiceDaemon::ServiceDaemon(int argc, char** argv) : version_("") {
    std::cout<<__TIMESTAMP__<<"  ["<< __FILE__<<": " <<__LINE__<<"]  " << "NOTE: ServiceDaemon Start "<<std::endl;
    env.init();
}

void ServiceDaemon::Loop() {
    std::vector<std::string> features = ConfigureManager::instance()->getAsVectorString("feature");


    SolutionPipeline::BuildAndStartGlobal();


     // init solutionpipeline
     for (int i = 0; i < features.size(); ++i) {
         std::cout<<__TIMESTAMP__<<"  ["<< __FILE__<<": " <<__LINE__<<"]  " << i << " " << features[i]<<std::endl;
         SolutionPipelinePtr sptr =
             std::make_shared<SolutionPipeline>(features[i]);
         if (sptr) {
             sptr->Build();
             sptr->Start();
             SolutionPipelineManager::SafeAdd(features[i], sptr);
         }
     }
     SolutionPipeline::BuildAndStartCoreGlobal();
     SolutionPipeline::BuildVideoInput();
     SolutionPipeline::StartVideoInput();

    int timeClock = 0;
    auto gt = GetGlobalVariable();
    while (true) {
        std::this_thread::sleep_for(std::chrono::seconds(15));
        if (gt->sys_quit) {
            // todo  wait queue size -> 0
            SolutionPipeline::StopAll();
            std::this_thread::sleep_for(std::chrono::seconds(15));
            exit(0);
        }

        std::this_thread::sleep_for(std::chrono::seconds(15));
    }
}



ServiceDaemon::~ServiceDaemon() {  }
