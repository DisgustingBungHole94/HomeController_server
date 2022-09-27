#include "homecontroller.h"

#include <csignal>
#include <unistd.h>

homecontroller controller;

int main() {
    char cwdBuf[256];
    getcwd(cwdBuf, 256);
    std::cout << "current working directory: " << std::string(cwdBuf) << std::endl;

    std::signal(SIGINT, [](int s) {
        controller.signal_interrupt(s);
    });

    std::signal(SIGPIPE, [](int s) {
        controller.signal_pipe(s);
    });

    std::signal(SIGSEGV, [](int s) {
        controller.signal_segv(s);
    });

    if (!controller.start()) {
        std::cout << "exited with non-zero status code." << std::endl;
        return -1;
    }

    std::cout << "exited with zero status code." << std::endl;
    return 0;
}