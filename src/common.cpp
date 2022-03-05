#include "common.h"

namespace meta {
namespace vision {

GlobalVariable* GetGlobalVariable(void) {
    static GlobalVariable t;
    return &t;
};



}  // namespace vision
}  // namespace meta

void log_func(const char* time, const char *file, const int line, const char *fmt, ...){

    char  sg_text_buf[MAX_LOG_MSG_LEN + 1];
    char *tmp_buf = sg_text_buf;
    char *o       = tmp_buf;
    memset(tmp_buf, 0, sizeof(sg_text_buf));
    char time_str[TIME_FORMAT_STR_LEN] = {0};

    va_list ap;
    va_start(ap, fmt);
    snprintf(o, MAX_LOG_MSG_LEN - 2 - strlen(tmp_buf), fmt, ap);
    va_end(ap);

    strcat(tmp_buf, "\r\n");
    printf("[ %s %s %d ]  %s", time, file, line, tmp_buf );
}