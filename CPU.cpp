#include "CPU.h"

int CPU::defaultTimeToBlock = 4;  // Ticks to block process

CPU::CPU(std::string init_filename, ScheduleMode mode) {
    this->begin = std::chrono::steady_clock::now();
    // ctor
    this->mode = mode;
    this->quantum = 2;
    this->value = 0;
    this->pc = 0;
    this->stepCounter = 0;
    this->mode = mode;
    this->quantumTimer = 0;

    this->init_filename = init_filename;  // Defaullt = "init"

    std::vector<std::string> lines = parseFile(init_filename);
    Process* p = new Process(init_filename, lines, -1, this->pc);
    this->running = p;
}

std::string CPU::basePath = "test-programs/";  // Path to test programs

CPU::~CPU() {
    this->end = std::chrono::steady_clock::now();
    std::cout << "Time elapsed: "
              << std::chrono::duration_cast<std::chrono::milliseconds>(
                     this->end - this->begin)
                     .count()
              << " ms" << std::endl;
    try {
        // dtor
        while (!this->waitingQueue.empty()) {
            delete this->waitingQueue.front();
            this->waitingQueue.pop();
        }
        while (!this->blockedQueue.empty()) {
            delete this->blockedQueue.front();
            this->blockedQueue.pop();
        }
        while (!this->doneQueue.empty()) {
            delete this->doneQueue.front();
            this->doneQueue.pop();
        }
        // delete this->running;
    } catch (...) {
        std::cout << "Error in destructor" << std::endl;
    }
}

std::vector<std::string> CPU::parseFile(std::string filename) {
    std::ifstream file;
    file.open(basePath + filename);
    if (file.is_open()) {
        std::vector<std::string> lines;
        std::string line;
        while (std::getline(file, line)) {
            lines.push_back(line);
        }
        file.close();
        return lines;
    } else {
        std::cout << "Unable to open file" << std::endl;
        throw "Unable to open file";
    }
}

void CPU::getNextProcess() {
    if (this->waitingQueue.size() > 0) {
        if (this->running != nullptr) {
            this->waitingQueue.push(this->running);
        }
        this->running = this->waitingQueue.front();
        this->waitingQueue.pop();
        this->quantumTimer = 0;
        this->value = this->running->value;
    }
}

// set running to front of waitingQueue
// void CPU::setRunning() {
//     if (waitingQueue.size() > 0) {
//         this->running = &waitingQueue.front();
//         waitingQueue.pop();
//     }

//     // this->running = this->waitingQueue.front();
//     if (this->running->startingPoint == -1) {
//         this->running->startingPoint = this->pc;
//     }
// }

// // put running on waitingQueue queue and get front of waitingQueue
// void CPU::getNextProcess() {
//     if (this->running->startingPoint != -1) {
//         this->waitingQueue.push(*this->running);
//     }
//     setRunning();
//     // this->waitingQueue.pop();
//     this->quantumTimer = 0;
//     this->stepCounter = this->running->pc;
// }

// splits line at whitespace to instruction and parameter
void splitInstruction(const std::string line, std::string* instruction,
                      std::string* parameter) {
    /*
    L 9999
P
X test_a
X test_c
X test_a
Z
*/
    std::string temp = line;
    std::string delimiter = " ";
    size_t pos = 0;
    std::string token;
    int i = 0;
    while ((pos = temp.find(delimiter)) != std::string::npos) {
        token = temp.substr(0, pos);
        if (i == 0) {
            *instruction = token;
        } else {
            *parameter = token;
        }
        temp.erase(0, pos + delimiter.length());
        i++;
    }
    if (i == 0) {
        *instruction = temp;
    } else {
        *parameter = temp;
    }

    std::cout << "instruction: " << *instruction << " parameter: " << *parameter
              << std::endl;
}

// Decrement blockCounter
void CPU::processBlocked() {
    // If timeToBlock == 0, move to waitingQueue
    std::queue<Process*> tempQueue;
    while (!this->blockedQueue.empty()) {
        Process* p = this->blockedQueue.front();
        this->blockedQueue.pop();
        p->blockedCounter--;
        p->ticksBlocked++;
        if (p->blockedCounter == 0) {
            this->waitingQueue.push(p);
        } else {
            tempQueue.push(p);
        }
    }
    this->blockedQueue = tempQueue;
}

// executes next line from the running process
int CPU::interpretInstruction() {
    processBlocked();

    // check if a process runs under quantum
    if (this->quantum <= this->quantumTimer && this->mode == ScheduleMode::RR) {
        // put running on waiting queue and get front of waiting
        std::cout << "Quantum reached - Switching" << std::endl;
        getNextProcess();
    }

    this->pc++;
    this->stepCounter++;
    this->quantumTimer++;

    if (this->running == nullptr) {  // no process running
        if (this->waitingQueue.size() > 0) {
            getNextProcess();
        } else {
            if (this->blockedQueue.size() > 0) {
                return 0;
            } else {
                std::cout << "No process running, waiting or blocked - END"
                          << std::endl;
                return 2;
            }
            return 0;
        }
        // if (this->waitingQueue.size() == 0 && this->blockedQueue.size() == 0)
        // {
        //     std::cout << "No process running, waiting or blocked" <<
        //     std::endl; return 2;
        // }
        // TODO: Increment
        return 0;
    }

    std::string instruction;
    std::string parameter;
    std::string line = this->running->getLine();

    if (line == "") {
        // fix later
        std::cout << "no lines left" << std::endl;
        return 2;  // no lines
    }

    splitInstruction(line, &instruction, &parameter);

    switch (instruction[0]) {
        case 'L':
            std::cout << "Load" << std::endl;
            this->value = stoi(parameter);
            break;

        case 'A':
            std::cout << "Add" << std::endl;
            this->value += stoi(parameter);
            break;

        case 'S':
            std::cout << "Subtract" << std::endl;
            this->value -= stoi(parameter);
            break;

        case 'P':
            std::cout << "Block" << std::endl;
            this->running->blockedCounter = defaultTimeToBlock;
            this->blockedQueue.push(this->running);
            this->running = nullptr;
            getNextProcess();
            break;

            // put running on blocked queue

            break;

        case 'X': {
            std::cout << "Execute" << std::endl;
            std::vector<std::string> lines = parseFile(parameter);

            Process* newProcess =
                new Process(parameter, lines, this->running->pid, this->pc);
            this->waitingQueue.push(newProcess);

            break;
        }

        case 'Z': {
            std::cout << "End" << std::endl;
            this->running->endPoint = this->pc;
            // put running on done queue
            this->doneQueue.push(this->running);
            this->running = nullptr;
            getNextProcess();

            // setRunning();
            // this->stepCounter = this->running->pc;
            // this->value = this->running->value;
            // this->waitingQueue.pop();
            break;
        }

        default:
            std::cout << "Invalid instruction" << std::endl;
            break;
    }
    if (this->running != nullptr) {
        // this->running->pc = this->stepCounter;
        this->running->value = this->value;
    }
    // this->running->pc = this->stepCounter;
    // this->running->value = this->value;

    return 0;
}

void printTbHead() {
    std::cout << "Name"
              << "\t"
              << "PID"
              << "\t"
              << "PPID"
              << "\t"
              << "Time used"
              << "\t"
              << "Time blocked"
              << "\t"
              << "Starting Point"
              << "\t"
              << "VALUE"
              << "\t"
              << "Total Time" << std::endl;
    // << std::endl;
}

std::string modeToString(ScheduleMode mode) {
    switch (mode) {
        case ScheduleMode::FCFS:
            return "FCFS";
        case ScheduleMode::RR:
            return "RR";
        default:
            return "Invalid";
    }
}

// Dump the current state of the CPU
void CPU::printAll() {
    // print out the running process
    std::cout << std::endl;
    std::cout << "Mode: " << modeToString(this->mode) << std::endl;
    std::cout << std::endl;
    std::cout << "RUNNING:" << std::endl;
    printTbHead();
    if (this->running != nullptr) {
        this->running->print();
    }

    // print out the waitingQueue processes
    std::cout << std::endl;
    std::cout << "waitingQueue:" << std::endl;
    printTbHead();

    std::queue<Process*> temp;
    if (this->waitingQueue.size() < 1000) {
        temp = this->waitingQueue;
        while (!this->waitingQueue.empty()) {
            this->waitingQueue.front()->print();
            this->waitingQueue.pop();
        }
        this->waitingQueue = temp;
    }
    // print out the blockedQueue processes
    std::cout << std::endl;
    std::cout << "BLOCKED:" << std::endl;

    printTbHead();

    temp = this->blockedQueue;
    while (!this->blockedQueue.empty()) {
        this->blockedQueue.front()->print();
        this->blockedQueue.pop();
    }
    this->blockedQueue = temp;

    // print out the doneQueue processes
    std::cout << std::endl;
    std::cout << "DONE:" << std::endl;
    printTbHead();
    temp = this->doneQueue;
    while (!this->doneQueue.empty()) {
        this->doneQueue.front()->print();
        this->doneQueue.pop();
    }
    this->doneQueue = temp;

    // Print out the current state of the CPU
    // std::cout << std::endl;
    // std::cout << "CPU:" << std::endl;
    // std::cout << "PC: " << this->pc << std::endl;
    // std::cout << "VALUE: " << this->value << std::endl;
    // std::cout << "MODE: " << this->mode << std::endl;
    // std::cout << "QUANTUM: " << this->quantum << std::endl;
    // std::cout << "QUANTUM TIMER: " << this->quantumTimer << std::endl;
    // std::cout << "STEP COUNTER: " << this->stepCounter << std::endl;
    // std::cout << "PROCESS COUNTER: " << this->processCounter << std::endl;
    // std::cout << "INIT FILENAME: " << this->init_filename << std::endl;
    // std::cout << std::endl;
}