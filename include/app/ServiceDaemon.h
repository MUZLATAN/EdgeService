#pragma once



#include <string>

#include "init.h"

namespace algo {
namespace vision {
class ServiceDaemon {
 public:
    ServiceDaemon(int argc, char** argv);
    ~ServiceDaemon();

    void Loop();

 private:
 private:
    std::string version_;

    EnvironmentInit env;
};

}  // namespace vision
}  // namespace algo
