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

/*#include <sys/eventfd.h>
#include <sys/epoll.h>
#include <unistd.h>

#include <iostream>
#include <thread>
#include <chrono>

int main() {
    int epfd = epoll_create(1);

    struct epoll_event events[32];
    
    int evfd = eventfd(0, 0);

    struct epoll_event evfd_event;
    evfd_event.events = EPOLLIN;
    evfd_event.data.fd = evfd;

    epoll_ctl(epfd, EPOLL_CTL_ADD, evfd, &evfd_event);

    std::thread t([=]() {
        uint64_t d = 1;

        for (int i = 0; i < 5; i++) {
            int numBytes = write(evfd, &d, sizeof(uint64_t));

            if (numBytes != sizeof(uint64_t)) {
                std::cout << "invalid write" << std::endl;
            } else {
                std::cout << "write data: " << std::to_string(d) << std::endl;
            }
        }
    });

    std::this_thread::sleep_for(std::chrono::milliseconds(3000));

    while(true) {
        int numFds = epoll_wait(epfd, events, 32, -1);
        for (int i = 0; i < numFds; i++) {
            if (events[i].events & EPOLLIN) {
                uint64_t d;
                int numBytes = read(events[i].data.fd, &d, sizeof(uint64_t));

                if (numBytes != sizeof(uint64_t)) {
                    std::cout << "invalid read" << std::endl;
                } else {
                    std::cout << "read data: " << std::to_string(d) << std::endl;
                }
            }
        }
    }

    return 0;
}*/