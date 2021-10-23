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

#define BACKTRACE_SIZE   16
void dump(int sig)
{

    int j, nptrs;
    void *buffer[BACKTRACE_SIZE];
    char **strings;

    nptrs = backtrace((void**)buffer, BACKTRACE_SIZE);
    std::cout<< "ERROR: in sig proc function !!!! sig is:" << sig<<std::endl;

    std::cout<<"backtrace() returned %d addresses\n "<<nptrs<<std::endl;

    strings = backtrace_symbols((void* const*)buffer, nptrs);
    if (strings == NULL) {
        std::cout<<"backtrace_symbols"<<std::endl;
    }

    for (j = 0; j < nptrs && strings[j] != NULL; j++)  {
        std::cout<< j<<" "<<strings[j]<<std::endl;
    }

    free(strings);

}

void sig_proc(int sig) {
    algo::vision::SolutionPipeline::StopAll();
    dump(sig);
    _exit(sig);
}

int main(int argc, char** argv) {
    /**
       signal(SIGILL, sig_proc);
        * 信号量处理函数区域，主要用于程序 稳定性维护、异常退出资源释放、打点等作用
       signal(SIGBUS, sig_proc);
        */
    signal(SIGCHLD, sig_proc);
    {
        signal(SIGQUIT, sig_proc);
        /**
    signal(SIGINT, sig_proc);
        *  需要忽略的信号，以下这些信号会导致进程退出，但是其实这些信号并不重要
    signal(SIGTERM, sig_proc);
        */
        signal(SIGSEGV, sig_proc);
        signal(SIGHUP,SIG_IGN); //终端挂起或者控制进程终止 ，典型如shell 启动的进程中shell 退出后其实会结束就是因为这个
        signal(SIGABRT, sig_proc);
#ifndef DEBUG
        signal(SIGINT,SIG_IGN); //键盘中断（如break键被按下）
        signal(SIGQUIT,SIG_IGN); // 键盘的退出键被按下
#endif
        signal(SIGPIPE,SIG_IGN); // 管道错误，忽略即可，io 怎么可能不出错呢
        signal(SIGALRM,SIG_IGN); // 时钟信号，不处理、不忽略就会杀进程
        signal(SIGTERM, SIG_IGN); // 请求中止进程，kill命令缺省发送，可忽略处理，先忽略处理
        signal(SIGSTOP, SIG_IGN); // 有人请求结束，先忽略处理
        signal(SIGTSTP, SIG_IGN); // 后台进程企图从控制终端读
        signal(SIGTTOU, SIG_IGN); // 后台进程企图从控制终端写
        signal(SIGCHLD, SIG_IGN); // 子进程结束

        /**
         * 无法处理和忽略。也就是其实不能处理的信号量
         */
        signal(SIGKILL, sig_proc); //强制停止, 无法忽略、阻塞，出了 信号量出来程序就会死

        /**
         * 需要处理的信号，程序发生严重错误，为了保证后续可以恢复，需要重启进程
         */
        signal(SIGILL, sig_proc); // 非法指令
        signal(SIGABRT, sig_proc); // 由abort(3)发出的退出指令 ，如libc内存错误发出的退出信号
        signal(SIGFPE, sig_proc);  // 浮点异常
        signal(SIGSEGV, sig_proc); // 无效的内存引用
        signal(SIGBUS, sig_proc);  // 总线错误(错误的内存访问)
        signal(SIGSYS, sig_proc);  // 无效的系统调用 (SVID)
        signal(SIGVTALRM, sig_proc);  // 实际时间报警时钟信号(4.2 BSD)
        signal(SIGXCPU, sig_proc);  // 超出设定的CPU时间限制(4.2 BSD)
        signal(SIGXFSZ, sig_proc);  // 超出设定的文件大小限制(4.2 BSD)
    }

    algo::vision::ServiceDaemon app(argc, argv);
    app.Loop();
    //
    std::this_thread::sleep_until(
        std::chrono::time_point<std::chrono::system_clock>::max());

    return 0;
}