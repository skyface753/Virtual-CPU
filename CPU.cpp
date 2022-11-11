#include "CPU.h"

CPU::CPU(int mode)
{
    //ctor
    this->mode = mode;
    createProcess("init");
}

CPU::~CPU()
{
    //dtor
}

Process CPU::createProcess(std::string filename)
{
   std::ifstream file;
    file.open(filename);
    if (file.is_open())
    {
        std::vector<std::string> lines;
        std::string line;
        while (std::getline(file, line))
        {
            lines.push_back(line);
        }
        file.close();
        Process process(lines);
        return process;
    }
    else
    {
        std::cout << "Unable to open file" << std::endl;
    }
}