#ifndef PROCESS_H
#define PROCESS_H

#include <iostream>
#include <string>
#include <vector>

// global counter
// int counter = -1;

class Process {
   public:
    static int counter;
    int value;
    int pc;
    int pid;
    int ppid;
    int prio;
    int startingPoint;
    std::vector<std::string> lines;
    void setPpid(int ppid);

    std::string getLine(int cpu_pc);

    Process();
    void setLines(std::vector<std::string> lines);
    void print();

   private:
};

#endif