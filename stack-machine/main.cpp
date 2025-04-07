#include <iostream>
#include <string>
#include "functions.cpp"

int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " <filename>" << std::endl;
        return 1;
    }

    std::string filename = argv[1];
    
    Operation stackMachine(filename);
    stackMachine.run();
    std::cout << "Program execution completed successfully." << std::endl;
    
    /*
    try {
        Operation stackMachine(filename);
        stackMachine.run();
        std::cout << "Program execution completed successfully." << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "Error executing program: " << e.what() << std::endl;
        return 1;
    }
        */
    
    return 0;
}