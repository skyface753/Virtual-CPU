#include "Process.h"

int Process::counter = -1;

Process::Process() {
    this->value = 0;
    this->pc = 0;
    this->prio = 0;
    this->ppid = 0;
    this->startingPoint = -1;
    this->pid = counter++;
}

void Process::setLines(std::vector<std::string> lines) { this->lines = lines; }

std::string Process::getLine(int cpu_pc) {
    if (this->lines.size() > cpu_pc) {
        return lines[cpu_pc];
    }
    return "";
}

void Process::print() {
    std::cout << this->pid << "\t" << this->ppid << "\t" << this->prio << "\t"
              << this->pc << "\t\t" << this->startingPoint << "\t\t"
              << this->value << std::endl;
}

void Process::setPpid(int ppid) { this->ppid = ppid; }