#include "CPU.h"

CPU::CPU(std::string init_filename, ScheduleMode mode) {
    // ctor
    this->mode = mode;
    this->quantum = 2;
    this->value = 0;
    this->pc = 0;
    this->stepCounter = 0;
    this->processCounter = 0;
    this->mode = mode;
    this->quantumTimer = 0;

    this->init_filename = init_filename;
    this->running = createProcess("init");
    if (this->running.startingPoint == -1) {
        this->running.startingPoint = this->pc;
    }
}

std::string CPU::basePath = "test-programs/";

CPU::~CPU() {
    // dtor
}

Process CPU::createProcess(std::string filename) {
    std::ifstream file;
    file.open(basePath + filename);
    if (file.is_open()) {
        std::vector<std::string> lines;
        std::string line;
        while (std::getline(file, line)) {
            lines.push_back(line);
        }
        file.close();
        Process process;
        process.setLines(lines);
        process.setPpid(this->processCounter);
        return process;
    } else {
        std::cout << "Unable to open file" << std::endl;
        throw "Unable to open file";
    }
}

// set running to front of waitingQueue
void CPU::setRunning() {
    this->running = this->waitingQueue.front();
    if (this->running.startingPoint == -1) {
        this->running.startingPoint = this->pc;
    }
}

// Unblock first process
void CPU::unblockFirstProcess() {
    if (this->blockedQueue.empty()) {
        return;
    }

    if (this->waitingQueue.size() > 1000) {
        std::queue<Process> temp;
        this->waitingQueue = temp;
    }

    this->waitingQueue.push(this->blockedQueue.front());
    this->blockedQueue.pop();
}

// put running on waitingQueue queue and get front of waitingQueue
void CPU::getNextProcess() {
    if (this->running.startingPoint != -1) {
        this->waitingQueue.push(this->running);
    }
    setRunning();
    this->waitingQueue.pop();
    this->quantumTimer = 0;
    this->stepCounter = this->running.pc;
}

// Dump the current state of the CPU
void CPU::printAll() {
    // print out the running process
    std::cout << std::endl;
    std::cout << "RUNNING:" << std::endl;
    std::cout << "PID"
              << "\t"
              << "PPID"
              << "\t"
              << "PRIO"
              << "\t"
              << "Time used"
              << "\t"
              << "Starting Point"
              << "\t"
              << "VALUE" << std::endl;
    this->running.print();

    // print out the waitingQueue processes
    std::cout << std::endl;
    std::cout << "waitingQueue:" << std::endl;
    std::cout << "PID"
              << "\t"
              << "PPID"
              << "\t"
              << "PRIO"
              << "\t"
              << "Time used"
              << "\t"
              << "Starting Point"
              << "\t"
              << "VALUE" << std::endl;

    std::queue<Process> temp;
    if (this->waitingQueue.size() < 1000) {
        temp = this->waitingQueue;
        while (!this->waitingQueue.empty()) {
            this->waitingQueue.front().print();
            this->waitingQueue.pop();
        }
        this->waitingQueue = temp;
    }
    // print out the blockedQueue processes
    std::cout << std::endl;
    std::cout << "BLOCKED:" << std::endl;
    std::cout << "PID"
              << "\t"
              << "PPID"
              << "\t"
              << "PRIO"
              << "\t"
              << "Time used"
              << "\t"
              << "Starting Point"
              << "\t"
              << "VALUE" << std::endl;

    temp = this->blockedQueue;
    while (!this->blockedQueue.empty()) {
        this->blockedQueue.front().print();
        this->blockedQueue.pop();
    }
    this->blockedQueue = temp;

    // print out the doneQueue processes
    std::cout << std::endl;
    std::cout << "DONE:" << std::endl;
    std::cout << "PID"
              << "\t"
              << "PPID"
              << "\t"
              << "PRIO"
              << "\t"
              << "Time used"
              << "\t"
              << "Starting Point"
              << "\t"
              << "VALUE" << std::endl;
    temp = this->doneQueue;
    while (!this->doneQueue.empty()) {
        this->doneQueue.front().print();
        this->doneQueue.pop();
    }
    this->doneQueue = temp;
}

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

    std::cout << "instruction: " << *instruction << std::endl;
    std::cout << "parameter: " << *parameter << std::endl;
}

// executes next line from the running process
void CPU::interpretInstruction() {
    // check if a process runs under quantum
    if (this->quantum <= this->quantumTimer && this->mode == ScheduleMode::RR) {
        // put running on waiting queue and get front of waiting
        getNextProcess();
    }
    std::string instruction;
    std::string parameter;
    // Stop if no process is running

    // print running.pc
    std::string line = this->running.getLine(this->stepCounter);

    if (line == "") {
        // fix later
        // std::cout << "no lines" << std::endl;
        return;
    }

    splitInstruction(line, &instruction, &parameter);
    // increment pc all relevant pcs
    this->pc++;
    this->stepCounter++;
    this->quantumTimer++;

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
            break;
            std::cout << "Peripheral" << std::endl;
            // Block running process for 4 cycles
            // this->blockedQueue.push(this->running);

            // case 'B':
            std::cout << "Block" << std::endl;
            this->running.pc = this->stepCounter;
            this->running.value = this->value;

            this->blockedQueue.push(this->running);
            setRunning();
            this->stepCounter = this->running.pc;
            this->waitingQueue.pop();
            break;

        case 'X':
            std::cout << "Execute" << std::endl;
            this->waitingQueue.push(createProcess(parameter));
            break;

        case 'Z':
            std::cout << "End" << std::endl;
            this->doneQueue.push(this->running);
            setRunning();
            this->stepCounter = this->running.pc;
            this->value = this->running.value;
            this->waitingQueue.pop();
            break;

        default:
            std::cout << "Invalid instruction" << std::endl;
            break;
    }
    this->running.value = this->value;
    this->running.pc = this->stepCounter;
    // Sleep for 100 ms to simulate a cycle
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
}