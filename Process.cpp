#include "Process.h"

int Process::counter = -1;

Process::Process(std::string name, std::vector<std::string> lines, int ppid,
                 int startingPoint) {
    this->value = 0;
    this->name = name;
    this->pc = 0;
    this->ppid = ppid;
    this->startingPoint = startingPoint;
    this->pid = counter++;
    this->blockedCounter = 0;
    this->ticksBlocked = 0;
    this->lines = lines;
    this->endPoint = -1;
}

std::string Process::getLine() {
    if (this->pc < this->lines.size()) {
        return this->lines[this->pc++];
    } else {
        return "";
    }
}

void Process::print() {
    std::string totalTime =
        this->endPoint != -1
            ? std::to_string(this->endPoint - this->startingPoint)
            : "not finished";
    std::cout << this->name << "\t" << this->pid << "\t" << this->ppid << "\t"
              << this->pc << "\t\t" << this->ticksBlocked << "\t\t"
              << this->startingPoint << "\t\t" << this->value << "\t"
              << totalTime << std::endl;
}
