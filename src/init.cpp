
#include "init.h"

#include <math.h>
// #include <openssl/md5.h>

#include <fstream>
#include <thread>
#include <json/json.h>

#include "common.h"
#include "mgr/ConfigureManager.h"
#include "network/HttpClient.h"
#include "node/Node.h"
#include "util.h"

//"NOTE: THE CODE'S VERSION"
#define ALGO_VERSION "8.0.4.1"

namespace algo {
namespace vision {

enum class AlgoDeviceType {
    kAICamera,
    kNVR,
    kHKCamera,
};

int gen_company_sn(const char *src, int len, AlgoDeviceType type, int channel,
                   char *dst_sn) {

    return 0;
}

void EnvironmentInit::init() {
    gt = GetGlobalVariable();

    initServiceVariable();
    initModelPlugin();

    initMetric();
}

void EnvironmentInit::uploadVersion() {

};

void EnvironmentInit::initMetric() {

}

void EnvironmentInit::initOtherComponent() {
}

void EnvironmentInit::initServiceVariable() {
    // init version
    gt->sys_path = ConfigureManager::instance()->getAsString(ALGO_SYS_PATH);
    std::string version_file = gt->sys_path + "/version";
 
    // init system sn
    initDeviceSn();

    // init group information of the device
    initDeviceGroupInfo();

    // init code version

    // todo other sys env
    // init algo face mqtt topic
    // init pad relate configure item.
    // read configure item from local file.
    std::string sys_path =
        ConfigureManager::instance()->getAsString(ALGO_SYS_PATH);


}

std::vector<std::string> EnvironmentInit::readCameraSnFromFile(
    std::string &file) {
    std::vector<std::string> sn_vec;
    std::ifstream fCameraSnRead(file);

    if (!fCameraSnRead) {
        fCameraSnRead.close();
        LOG(ERROR) << "NO DEVICE SN";
        LOG(ERROR) << "PATH: " << file;
        LOG(ERROR) << " please calture camera sn.";
        // log metric
        return sn_vec;
    } else {
        while (!fCameraSnRead.eof()) {
            std::string lineTxt;
            fCameraSnRead >> lineTxt;
            if (!lineTxt.compare(0, 9, "camera_sn")) {
                std::string sn = lineTxt.substr(10, lineTxt.length() - 9);
                sn_vec.emplace_back(sn);
            }
        }
    }
    fCameraSnRead.close();
    return sn_vec;
}

void EnvironmentInit::initDeviceSn() {
    std::string sys_path =
        ConfigureManager::instance()->getAsString(ALGO_SYS_PATH);
    std::string clientSnFile = sys_path + "/sys_sn";
    std::ifstream fClientSnRead(clientSnFile);
    if (!fClientSnRead) {
        gt->client_sn = "default";
    } else {
        while (!fClientSnRead.eof()) {
            std::string lineTxt;
            fClientSnRead >> lineTxt;
            if (!lineTxt.compare(0, 9, "client_sn")) {
                gt->client_sn = lineTxt.substr(10, lineTxt.length() - 9);
            }
        }
    }
    fClientSnRead.close();

    std::string cameraSnFile = sys_path + "/camera_sn";
    std::vector<std::string> camera_sn_vec = readCameraSnFromFile(cameraSnFile);
    if (camera_sn_vec.size() > 0) gt->camera_sns = camera_sn_vec[0];

    std::vector<std::string> device_name_vec =
        ConfigureManager::instance()->getAsVectorString("video_input");
    for (size_t i = 0; i < device_name_vec.size(); ++i) {
        if (camera_sn_vec.size() >= i + 1) {
            LOG(INFO) << device_name_vec[i] << " <-rtsp/usb, : sn->"
                      << camera_sn_vec[i];
            gt->video_inputs = device_name_vec[i];
            // ServiceVariableManager::Add(device_name_vec[i],
            // camera_sn_vec[i]);
        }
    }


    // set default service variable,
}

void EnvironmentInit::initDeviceGroupInfo() {
}

void EnvironmentInit::initModelPlugin() {
}

}  // namespace vision
}  // namespace algo
