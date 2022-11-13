#ifndef CPU_H
#define CPU_H

#include <fstream>
#include <iostream>
#include <queue>
#include <string>
#include <thread>
#include <vector>

#include "Process.h"

enum ScheduleMode { FCFS, RR };

class CPU {
   public:
    CPU(std::string init_filename, ScheduleMode mode);
    ~CPU();

    void interpretInstruction();
    void printAll();

   protected:
    static std::string basePath;

   private:
    void getNextProcess();
    void unblockFirstProcess();
    void setRunning();
    std::string init_filename;
    std::queue<Process> waitingQueue;
    std::queue<Process> blockedQueue;
    std::queue<Process> doneQueue;
    // holds the current value of the running process
    int value;
    // global programm counter
    int pc;
    // for counting the steps made per process
    int stepCounter;
    // for tracking the pid/ppid
    int processCounter;
    // for counting the steps while in quantum
    int quantumTimer;
    // defines the quantum size
    int quantum;
    // holds the current running process
    Process running;
    // holds the schedule mode
    ScheduleMode mode;  // FCFS or RR

    Process createProcess(std::string filename);
};

#endif