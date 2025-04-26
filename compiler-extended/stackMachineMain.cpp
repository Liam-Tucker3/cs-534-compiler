#include <iostream>
#include <string>
#include "stackMachine.cpp"

int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " <filename>" << std::endl;
        return 1;
    }

    std::string filename = argv[1];
    
    Operation stackMachine(filename);
    stackMachine.run();
    std::cout << "Program execution completed successfully." << std::endl;
    
    return 0;
}