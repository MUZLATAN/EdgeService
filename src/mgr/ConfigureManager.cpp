
#include "mgr/ConfigureManager.h"


#include <fstream>
#include <iostream>

#include "common.h"
#include "network/HttpClient.h"


namespace algo {
namespace vision {




class ConfigPack {
 private:
    std::vector<ConfigureManager> instances_ =
        std::vector<ConfigureManager>((int)CONFIGType::_END_);
    // std::mutex mt_;
 public:
    ConfigPack() { instances_[(int)CONFIGType::DEFAULT].init(); };
    ConfigureManager* getInstance(CONFIGType type) {
        // std::lock_guard<std::mutex> lock(mt_);
        return &instances_[(int)type];
    };
};
ConfigureManager* ConfigureManager::instance(CONFIGType type) {
    static ConfigPack cfp;
    return cfp.getInstance(type);
}
ConfigureManager::ConfigureManager() {
    config_path = ROOTPATH;
    config_path += "config/service_default.json";
    std::cout<<__TIMESTAMP__<<"  ["<< __FILE__<<": " <<__LINE__<<"]  " << config_path;
};
void ConfigureManager::init() {
    std::ifstream json_file_stream(config_path);
    if (json_file_stream.is_open()) {
        std::cout<<__TIMESTAMP__<<"  ["<< __FILE__<<": " <<__LINE__<<"]  " << "Load config success! path: " << config_path;
    } else {
        std::cout<<__TIMESTAMP__<<"  ["<< __FILE__<<": " <<__LINE__<<"]  " << "Load config failed! Please check " << config_path;
    }

    std::stringstream config_string_stream;
    config_string_stream << json_file_stream.rdbuf();
    json_file_stream.close();

    std::string config_string = config_string_stream.str();


    std::cout<<__TIMESTAMP__<<"  ["<< __FILE__<<": " <<__LINE__<<"]  " << config_string;
    Json::Value str_json;
    std::string err;
    Json::CharReaderBuilder jsoriReader;
    std::unique_ptr<Json::CharReader> const oriReader(jsoriReader.newCharReader());
    oriReader->parse(config_string.c_str(), config_string.c_str()+config_string.size(), &config_option_, &err);


    auto func = [&]() {
        std::string clientSnFile =
                config_option_["sys_path"].asString() + "/sys_sn";
        std::string client_sn;
        std::ifstream fClientSnRead(clientSnFile);
        if (!fClientSnRead) {
            client_sn = "default";
        } else {
            while (!fClientSnRead.eof()) {
                std::string lineTxt;
                fClientSnRead >> lineTxt;
                if (!lineTxt.compare(0, 9, "client_sn")) {
                    client_sn = lineTxt.substr(10, lineTxt.length() - 9);
                }
            }
        }
        fClientSnRead.close();
        return false;
    };

    if (!func()) {
        std::cout<<__TIMESTAMP__<<"  ["<< __FILE__<<": " <<__LINE__<<"]  " << "NOTE: use default config.json"<<std::endl;
    } else {
        std::cout<<__TIMESTAMP__<<"  ["<< __FILE__<<": " <<__LINE__<<"]  " << "NOTE: use remote config.json"<<std::endl;
    }
}
double ConfigureManager::getAsDouble(const std::string& key) {
    if (check(key)) {
        return config_option_[key].asDouble();
    }
    return 0;
}
int ConfigureManager::getAsInt(const std::string& key) {
    if (check(key)) {
        return config_option_[key].asInt();
    }
    return 0;
}
std::string ConfigureManager::getAsString(const std::string& key) {
    if (check(key)) {
        return config_option_[key].asString();
    }
    return "";
}
std::vector<std::string> ConfigureManager::getAsVectorString(
    const std::string& key) {
    std::vector<std::string> values;
    Json::Value item = config_option_[key];
    for (short idx = 0; idx < item.size(); ++idx){
        values.emplace_back(item[idx].asString());
    }
    return values;
}
bool ConfigureManager::check(const std::string& key) {
    return true;
}
Json::Value ConfigureManager::getAsAarry(const std::string& key) {
    Json::Value data;
    if (check(key)) {
        data = config_option_[key];
        return data;
    }
    return  data;
}

void ConfigureManager::setAsInt(const std::string& key, int value) {
    // todo add lock
    config_option_[key] = value;
}
}  // namespace vision
}  // namespace algo
