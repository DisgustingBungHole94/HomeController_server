#include "homecontroller.h"

#include <csignal>
#include <unistd.h>

HomeController controller;

int main() {
    char cwdBuf[256];
    getcwd(cwdBuf, 256);
    std::cout << "Current working directory: " << std::string(cwdBuf) << std::endl;

    std::signal(SIGINT, [](int s) {
        controller.shutdown();
    });

    if (!controller.start()) {
        std::cout << "Exited with non-zero status code." << std::endl;
        return -1;
    }

    std::cout << "Exited with zero status code." << std::endl;
    return 0;
}