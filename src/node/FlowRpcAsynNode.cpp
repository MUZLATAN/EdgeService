
#include "node/FlowRpcAsynNode.h"

#include <fstream>
#include <exception>
#include <thread>
#include <fnmatch.h>
#include <dirent.h>
#include <time.h>
#include <sys/timeb.h>
#include "common.h"
#include "mgr/ConfigureManager.h"
#include "network/HttpClient.h"

namespace meta {
namespace vision {
FlowRpcAsynNode* FlowRpcAsynNode_flow;

void FlowRpcAsynNode::SendToFlowServer(const std::string& limbustype,
                                            const std::string& dataurl,
                                            const int datatype,
                                            const std::string& eventStr) {
    Json::Value kafka_pack;
    Json::Value limbus_datapck;
    //按照既有的规定这里的datatype 如果不为 -1 这里的data就需要是字符串的json, 
    //如果datatype 为-1 那么这里的数据需要根据实际情况而定

    std::cout<<__TIMESTAMP__<<"  ["<< __FILE__<<": " <<__LINE__<<"]  "<<dataurl<<std::endl;
    Json::Value event;
    reader.parse(eventStr,event);
    limbus_datapck["body"] = event;
    limbus_datapck["url"] = dataurl;
    limbus_datapck["limbustype"] = limbustype;


    std::string outputJson = Json::FastWriter().write(limbus_datapck);
    std::cout<<__TIMESTAMP__<<"  ["<< __FILE__<<": " <<__LINE__<<"]  " <<outputJson<<std::endl;

    std::unique_lock<std::mutex> qlock(m_qmutex, std::defer_lock);
    qlock.lock();
    {
        m_queue.push(outputJson);
        // std::cout<<__TIMESTAMP__<<"  ["<< __FILE__<<": " <<__LINE__<<"]  "<<"push "<<m_queue.size()<<std::endl;
    }
    qlock.unlock();

}



bool FlowRpcAsynNode::sendMessage(const std::string& data) {
//    if (data.size() == 0) {
//        std::cout<<__TIMESTAMP__<<"  ["<< __FILE__<<": " <<__LINE__<<"]  " << "NOTE: message size must big than zero"<<std::endl;
//        return false;
//    }
//    std::string response;
//    HttpPost("", data, response);
//    return (response == "\"success\"" ? true: false);

//TODO 此处定义网络发送函数


    //此处暂时定义模拟发送函数, 模拟成功失败的概率
    struct timeb timeSeed;
    std::cout<<__TIMESTAMP__<<"  ["<< __FILE__<<": " <<__LINE__<<"]  "<<"in SendMethod"<<std::endl;
    ftime(&timeSeed);
    srand(timeSeed.time * 1000 + timeSeed.millitm);  // milli time
//    int r = rand() %11;
//    if (r % 7 == 0)
//        return true;

    return true;

}


void FlowRpcAsynNode::run() {
    sys_data_path =std::string(ROOTPATH) + "FLOW/data/";

    std::cout<<__TIMESTAMP__<<"  ["<< __FILE__<<": " <<__LINE__<<"]  " <<sys_data_path<<std::endl;

    LoadFileNames(sys_data_path);

    //新开一个线程, 来检查m_queue的状态
    //m_queue 为空 就向里面塞数据
    //fail_queue 数据过多, 存入磁盘
    //注意内存对齐  暂时未做
     std::thread t1([&](){
         MoveData();
     });
     t1.detach();

    std::unique_lock<std::mutex> m_qlock(m_qmutex, std::defer_lock);
    std::unique_lock<std::mutex> m_fqlock(m_fqmutex, std::defer_lock);


    //计算发送失败的次数
    int cnt = 0;
    while(true){
        if (gt->sys_quit) {
            dump();
            return;
        }
        // /**
        //  * 向主进程的变量汇报时间
        // */
        // if (set_time_cnt++ > 10 ){ 
        //     set_time_cnt = 0; 
        //     SetTime();
        // }
        //不断取数据
        std::cout<<__TIMESTAMP__<<"  ["<< __FILE__<<": " <<__LINE__<<"]  "<<"flow rpc"<<std::endl;

        if (m_queue.empty()){
            std::this_thread::sleep_for(std::chrono::seconds(5));
            continue;
        }
        m_qlock.lock();
        auto data_info = m_queue.front();
        m_queue.pop();
        m_qlock.unlock();


        while (cnt++ < loop_times ){
            //不断发送数据
            if (sendMessage(data_info)){
                cnt = 0;
                success_flag=true;
                break;
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(5000));
        }
        

        if (cnt >= loop_times){
            success_flag=false;
            m_fqlock.lock();
            m_fail_queue.push(data_info);
            cnt = 0;
            m_fqlock.unlock();
        }

        
        if (m_fail_queue.size() >= 5){
            if (files.size()> 100)
            {
                auto top = files.front();
                files.erase(files.begin());
                //删除文件
                if(::remove(top.first.c_str())!=0){
                    std::cout<<__TIMESTAMP__<<"  ["<< __FILE__<<": " <<__LINE__<<"]  "<<"delete file failed!"<<std::endl;
                }
            }

            long int time = std::chrono::system_clock::now().time_since_epoch() / std::chrono::seconds(1);
            std::string name = sys_data_path+std::to_string(time)+".data.over";
            
            files.push_back(std::pair<std::string, long int>(name, time));
            std::ofstream ofs(name, std::ios::app);
            m_fqlock.lock();
            while(m_fail_queue.size() > 0){
                ofs<<m_fail_queue.front()<<SEPARATION;
                m_fail_queue.pop();
            }
            ofs.close();
            std::cout<<__TIMESTAMP__<<"  ["<< __FILE__<<": " <<__LINE__<<"]  "<<"m_faile_queue size is greater than 5!!! save in file and m_faile_queue size is: "<<m_fail_queue.size()<<name<<std::endl;
            m_fqlock.unlock();
        }


        
    }
}


void FlowRpcAsynNode::Split(const std::string& data){
    int index = 0;
    int dataidx = 0;
    while ((dataidx = data.find(SEPARATION, index)) <= data.length())
    {
        std::string str = data.substr(index, dataidx);
        m_queue.push(str);
        index = dataidx+strlen(SEPARATION);
    }
    if (data.find_last_of(SEPARATION) != (data.length()-1))
        std::cout<<__TIMESTAMP__<<"  ["<< __FILE__<<": " <<__LINE__<<"]  "<<"not end of "<<SEPARATION<<", the lastest record maybe wrong"<<std::endl;

}

void FlowRpcAsynNode::MoveData() {
    std::unique_lock<std::mutex> m_qlock(m_qmutex, std::defer_lock);
    std::unique_lock<std::mutex> m_fqlock(m_fqmutex, std::defer_lock);
    while (true){
        if (gt->sys_quit)
        {
            return;
        }

        if (m_queue.empty() && !success_flag)            
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(50));
        }

        if (m_queue.empty()){
            if (!m_fail_queue.empty() && success_flag){
                m_fqlock.lock();
                std::swap(m_fail_queue, m_queue);
                m_fqlock.unlock();
            }
        }
     

        
        if (!m_fail_queue.empty() && success_flag){
            //failed queue pop;
            std::cout<<__TIMESTAMP__<<"  ["<< __FILE__<<": " <<__LINE__<<"]  " <<"data from failed queue lock"<<std::endl;
            m_fqlock.lock();
            auto info = m_fail_queue.front();
            m_fail_queue.pop();
            m_fqlock.unlock();
            std::cout<<__TIMESTAMP__<<"  ["<< __FILE__<<": " <<__LINE__<<"]  "<<"data from failed queue ulock"<<std::endl;
            //data queue enqueue;
            if (!info.empty()){
                std::cout<<__TIMESTAMP__<<"  ["<< __FILE__<<": " <<__LINE__<<"]  "<<"failed queue to data queue lock"<<std::endl;
                m_qlock.lock();
                m_queue.push(info);
                m_qlock.unlock();
                std::cout<<__TIMESTAMP__<<"  ["<< __FILE__<<": " <<__LINE__<<"]  "<<"failed queue to data queue unlock"<<std::endl;
            }
                
        }
        

        if (m_fail_queue.empty() && m_queue.empty() && success_flag){


            //读本地文件 files 按时间升序 排列 ,第一个即历史最久的文件
            if (files.size() > 0){

                auto top = files.front();
                std::ifstream in(top.first);
                std::ostringstream oss;
                oss<< in.rdbuf();
                std::string str = oss.str();
                std::cout<<__TIMESTAMP__<<"  ["<< __FILE__<<": " <<__LINE__<<"]  "<<str<<std::endl;
                //将str分开存入m_queuue
                m_qlock.lock();
                Split(str);
                m_qlock.unlock();
                //从维护的vector 中删除记录
                files.erase(files.begin());
                std::cout<<__TIMESTAMP__<<"  ["<< __FILE__<<": " <<__LINE__<<"]  "<<files.begin()->first<<std::endl;
                for (auto fileit = files.begin(); fileit != files.end(); fileit++)
                    std::cout<<__TIMESTAMP__<<"  ["<< __FILE__<<": " <<__LINE__<<"]  "<<fileit->first<<std::endl;

                //删除文件
                if(::remove(top.first.c_str())!=0){
                    std::cout<<__TIMESTAMP__<<"  ["<< __FILE__<<": " <<__LINE__<<"]  "<<"delete file failed!"<<std::endl;
                }
            }
            success_flag = false;
        }
    }
}


void FlowRpcAsynNode::LoadFileNames(const std::string& path){
    std::string pattern = ".data.over";
    std::vector<std::string> filenames;
    try{
        struct dirent *entry;
        DIR *dir = opendir(path.c_str());

        if (dir == NULL)
        {
            return;
        }
        while ((entry = readdir(dir)) != NULL)
        {
            std::string filename(entry->d_name);
            int idx = filename.find(".data.over");
            if (idx != std::string::npos){
                std::string times = filename.substr(0, idx);
                long int timestamp;
                timestamp = atol(times.c_str());
                files.emplace_back(std::make_pair(std::move(path+filename), timestamp));
            }
        }
        closedir(dir);

        // 将文件按时间排序
        std::sort(files.begin(), files.end(),
                     [&](std::pair<std::string, long int> a, std::pair<std::string, long int> b){
                                 return a.second < b.second;});

    }catch(std::exception e){
        std::cout<<__TIMESTAMP__<<"  ["<< __FILE__<<": " <<__LINE__<<"]  "<<e.what()<<std::endl;
    }
    for (auto it : filenames){
        std::cout<<__TIMESTAMP__<<"  ["<< __FILE__<<": " <<__LINE__<<"]  "<<it<<std::endl;
    }

}
void FlowRpcAsynNode::dump(){
    //if this Node exit
    // dump the queue's content
    long int time = std::chrono::system_clock::now().time_since_epoch() / std::chrono::seconds(1);
    std::string name = std::to_string(time)+".data.over";
    std::ofstream ofs(name, std::ios::app);

    if (!m_queue.empty()){
        std::cout<<__TIMESTAMP__<<"  ["<< __FILE__<<": " <<__LINE__<<"]  "<<"m_queue not empty!"<<std::endl;
        while(m_queue.size()> 0){
            ofs<<m_queue.front()<<SEPARATION;
            m_queue.pop();
        }
    }

    // dump the failed queue's content
    if (!m_fail_queue.empty()){
        std::cout<<__TIMESTAMP__<<"  ["<< __FILE__<<": " <<__LINE__<<"]  "<<"m_fail_queue not empty"<<std::endl;
        while(m_fail_queue.size() > 0){
            ofs<<m_fail_queue.front()<<SEPARATION;
            m_fail_queue.pop();
        }
    }
    ofs.close();
}

}  // namespace vision
}  // namespace meta