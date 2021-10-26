#pragma once
#include <json/json.h>
#include <mutex>
#include <queue>
#include <sys/stat.h>

#include "DynamicFactory.h"
#include "mgr/ConfigureManager.h"
#include "node/Node.h"

#define  SEPARATION "#&&#"


namespace algo {
namespace vision {

class FlowRpcAsynNode : public Node,
                             DynamicCreator<FlowRpcAsynNode> {
 public:
    explicit FlowRpcAsynNode() : Node(ALGO_NODE_FLOWRPC){
       loop_times = 5;
       count_successfully = 0;
       count_failed = 0;
       success_flag = true;

    };
    virtual ~FlowRpcAsynNode() { 
       dump();
    }

    void SendToFlowServer(const std::string& limbustype, const std::string& dataurl, const int datatype, const std::string& eventStr);

    void run();
    void init() { std::cout<<__TIMESTAMP__<<"  ["<< __FILE__<<": " <<__LINE__<<"]  " << "flow client init"<<std::endl; }

 private:
    bool sendMessage(const std::string& data);

    void Split(const std::string& data);

    void LoadFileNames(const std::string& path);

    void MoveData();

    void dump();

 private:
    std::queue<std::string> q_;
    std::mutex qmt_;
    std::mutex resmt_;

    bool success_flag;
    int loop_times;
   //  std::condition_variable m_condt;
    std::string sys_data_path;
    std::queue<std::string> m_queue; //data queue
    std::queue<std::string> m_fail_queue; //failed queue
    std::mutex m_qmutex;   // data queue mutex
    std::mutex m_fqmutex;  //failed queue mutex;

    //记录保存本地的文件数据结构
    // key:     name
    // value:   time
    std::vector<std::pair<std::string, long int>> files;

    struct stat m_stat;

    //统计发送数据成功或失败的次数
    int count_successfully;
    int count_failed;


    //parse Json
    Json::Reader reader;
    

};

// TODO: tmp variable
extern FlowRpcAsynNode* FlowRpcAsynNode_flow;

}  // namespace vision
}  // namespace algo