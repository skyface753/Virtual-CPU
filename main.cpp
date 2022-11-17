#include <chrono>
#include <iostream>
#include <thread>

#include "CPU.h"

ScheduleMode convertToEnum(char mode) {
    if (mode == 'F') {
        return FCFS;
    } else if (mode == 'R') {
        return RR;
    }
}

ScheduleMode askForScheduleMode() {
    char mode;
    do {
        std::cout << "Please enter the scheduling mode you would like to use: "
                  << std::endl;
        std::cout << "F. (FCFS) First Come First Serve" << std::endl;
        std::cout << "R. (RR) Round Robin" << std::endl;
        std::cin >> mode;
    } while (mode != 'F' && mode != 'R');
    return convertToEnum(mode);
}

int run(CPU* cpu, bool debugMode) {
    // if (debugMode) {
    //     std::cout << "Debug mode is on" << std::endl;
    // } else {
    while (cpu->interpretInstruction() == 0) {
        cpu->printAll();
        if (debugMode) {
            char input;
            std::cout << "Press enter to continue" << std::endl;
            std::cin.get();
            std::cin.get();
        }

        // Sleep for 100 ms to simulate a cycle
        std::this_thread::sleep_for(std::chrono::milliseconds(300));
    }
    // }
    return 0;
}
int main(const int argc, const char* argv[]) {
    bool debugMode = false;
    // ./cpu.out -d
    if (argc == 2) {
        if (std::string(argv[1]) == "-d") {
            debugMode = true;
        }
    }
    std::cout << "Debug mode is " << debugMode << std::endl;
    ScheduleMode scheduleMode = askForScheduleMode();

    CPU cpu("init", scheduleMode);
    return run(&cpu, debugMode);
}
