#ifndef Process_h
#define Process_h

#include <iostream>
#include <string>
#include <vector>
class Process{
public:
Process(std::vector<std::string> lines);
private:
std::vector<std::string> lines;
};

#endif