#ifndef PROCESS_H
#define PROCESS_H

#include <iostream>
#include <string>
#include <vector>

class Process {
   public:
    static int counter;  // for tracking the pid/ppid - "global"
    int value;           // holds the current value of the running process
    int pc;              //  program counter of the process
    int pid;             // holds the  pid of the running process
    int ppid;            // holds the  ppid of the running process
    int startingPoint;
    int blockedCounter;  // > 0 if blocked
    int ticksBlocked;    // how many ticks the process was blocked
    int endPoint;  // the cpu step when the process ended -> Measure the total
                   // time of the process
    std::string name;

    std::string getLine();

    Process(std::string name, std::vector<std::string> lines, int ppid,
            int startingPoint);
    void print();

   private:
    std::vector<std::string> lines;
};

#endif