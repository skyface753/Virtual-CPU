#ifndef CPU_H  
#define CPU_H

#include <iostream>
#include <queue>
#include "Process.h"
#include <string>
#include <fstream>
#include <vector>

class CPU  
{
    public:
        CPU(int mode);
        ~CPU();
    private:
        std::queue<int> waitingQueue;
        std::queue<int> blockedQueue;
        std::queue<int> doneQueue;
        int mode; // 0 = FIFO, 1 = RR, 2 = SJF
        Process createProcess(std::string filename);
};

#endif