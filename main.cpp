#include<iostream>

// Call by ref
char askForScheduleMode( char &mode )
{
    do{
        std::cout << "Please enter the scheduling mode you would like to use: " << std::endl;
        std::cout << "1. (FIFI) First in First out" << std::endl;
        std::cout << "2. (RR) Round Robin" << std::endl;
        std::cout << "3. (SJF) Shortest Job First" << std::endl;
        std::cin >> mode;
    } while ( mode != '1' && mode != '2' && mode != '3' );
    return mode;

}

int main(const int argc, const char* argv[])
{
    bool debugMode = false;
    if (argc == 2)
    {
        if (argv[1] == "-d")
        {
            debugMode = true;
        }
    }
    char mode;
    askForScheduleMode(mode);
    std::cout << mode << std::endl;


}