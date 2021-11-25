#include "network/HttpClient.h"

#include <mutex>
#include <thread>
#include "common.h"

namespace meta {
namespace vision {

static std::mutex mt_;

        typedef struct {
            char*       memory;
            size_t      size;
        } MemoryStruct;

class HttpClient {
 public:
    HttpClient();
    ~HttpClient();
    // true means success
    bool Post(const char *url, const std::string &value, std::string &re);
    bool Get(const char *url, const std::string &value, std::string &re);

 private:
    static size_t OnWriteData(void *buffer, size_t size, size_t nmemb,
                              void *lpVoid) {
        std::string *str = dynamic_cast<std::string *>((std::string *)lpVoid);
        if (NULL == str || NULL == buffer) {
            return -1;
        }

        char *pData = (char *)buffer;
        str->append(pData, size * nmemb);
        return nmemb;
    };

 

 private:

};

HttpClient::HttpClient() {

};

bool HttpClient::Post(const char *url, const std::string &value,
                      std::string &re) {

    return false;
};

bool HttpClient::Get(const char *url, const std::string &value,
                     std::string &re) {
    return false;
};




HttpClient::~HttpClient() {

};





bool HttpPost(const char *url, const std::string &value, std::string &re) {
    HttpClient client;
    return client.Post(url, value, re);
};

bool HttpGet(const char *url, const std::string &value, std::string &re) {
    HttpClient client;
    return client.Get(url, value, re);
};

}  // 
}  //