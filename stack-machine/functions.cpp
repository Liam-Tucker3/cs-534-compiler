#include <stdio.h>
#include <string>
#include <cstring>
#include <iostream>

/* TODO
* 1. Modify implemented functions to access stack relative to stackTop and stackPointer
* 2. Implement CALL, RET, RETV to handle stack pointer appropriately
X 3. Implement LABEL function to iterate through stack, find label, return index
* 4. Overwrite CALL function to handle labels
X 4. Overwrite BRT, BRZ, and JUMP functions to handle labels
* 5. Implement helper functions to call appropriate functions from function name, parameter
* 6. Modify execInstruction to parse and call helper function
* 7. Write main function to allow for running program
* 8. Modify provided assembly to work with my code
* 9. Test program
* 10. PUSH TO GITHUB, SUBMIT
* 11. Write test cases
* 12. PUSH TO GITHUB
* 13. Implement tracing
* 14. PUSH TO GITHUB
*/


class Operation {
private:
    // Attributes
    int gpr; // General purpose register

    int stack[1024];
    int stackTop; // Top available slot in memory
    int stackPointer; // Current frame

    int programCounter;
    std::string instructions[1024];

    // Helper functions

    /* Returns index of label, -1 if does not exist*/
    int LABEL(char* label) {
        for (int i = 0; i < 1024; i++) {
            if (instructions[i] == label) return i;
        }
        return -1;
    }

    /* Put top value from the stack onto general purpose register without altering stack*/
    void PEEK() {
        this->gpr = stack[stackTop - 1];
    }

public:
    /* Empty constructor */
    Operation() {
        gpr = 0;
        stackTop = 0;
        stackPointer = 0;
        programCounter = 0;
        for (int i = 0; i < 1024; i++) {
            stack[i] = -1;
            instructions[i] = "";
        }
    }

    /* Constructor with one parameter: filename, a file containing the programs to be executed*/
    Operation(std::string& filename) {
        gpr = 0;
        stackTop = 0;
        stackPointer = 0;
        programCounter = 0;
        for (int i = 0; i < 1024; i++) {
            stack[i] = -1;
            instructions[i] = "";
        }

        FILE* file = fopen(filename.c_str(), "r");
        if (file != nullptr) {
            char line[32];
            int index = 0;
            while (fgets(line, sizeof(line), file) && index < 1024) {
                instructions[index] = std::string(line);
                index++;
            }
            fclose(file);
        }
    }

    /* Parses instruction, executes appropriate */
    void execInstruction(std::string instruction) {

        // Finding parantheses
        size_t openParen = instruction.find('(');
        size_t closeParen = instruction.find(')');
        if (openParen != std::string::npos && closeParen != std::string::npos) {
            std::string functionName = instruction.substr(0, openParen);
            std::string params = instruction.substr(openParen + 1, closeParen - openParen - 1);

            if (!params.empty()) {
                if (params[0] == '"' && params[params.length() - 1] == '"') { // Case: String parameter
                    // Parsing string parameter
                    char* param = new char[params.length() - 1];
                    strncpy(param, params.c_str() + 1, params.length() - 2);
                    param[params.length() - 2] = '\0';
                    std::cout << "Function: " << functionName << ", Parameter: " << param << " (char*)" << std::endl;
                    delete[] param;
                } else { // Case: Integer parameter
                    // Parameter is an integer
                    int param = std::stoi(params);
                    std::cout << "Function: " << functionName << ", Parameter: " << param << " (int)" << std::endl;
                }
            } else { // Case: No Parameters
                std::cout << "Function: " << functionName << ", No parameters" << std::endl;
            }
        }
        else {
            // This line is a label
            return;
        }

    }

    /* FUNCTIONS */

    void CALL() {
        // pass
    }

    void RET() {
        // pass
    }

    void RETV() {
        // pass
    }

    /* Puts value from general purpose register onto stack*/
    void PUSH() {
        stack[stackTop] = this->gpr;
        stackTop += 1;
    }

    /* PUSH OVERLOAD: Puts specified value onto stack */
    void PUSH(int value) {
        stack[stackTop] = value;
        stackTop += 1;
    }

    /* Removes top value from stack, places it on general purpose register*/
    int POP() {
        int val = stack[stackTop - 1];
        stackTop -= 1;
        this->gpr = val;
    }

    /* Duplicates value on top of stack*/
    void DUP() {
        PEEK();
        PUSH();
    }

    /* Loading value from specified location in memory into gpr*/
    void LOAD() {
        this->POP(); // Loading address into gpr
        this->gpr = stack[this->gpr]; // Accessing value from specified address in stack
        this->PUSH(); // Pushing value onto stack
    }

    /* Saves value to specified spot in memory while leaving value on top of stack*/
    /* Note: Address must be at top of stack; value should be second on stack*/
    void SAVE() {
        int address = this->POP();
        this->PEEK();
        stack[address] = this->gpr;

        // pass
    }

    /* Saves value to specified spot in memory while removing value from top of stack*/
    /* Note: Address must be at top of stack; value should be second on stack*/
    void STORE() {
        int address = this->POP();
        this->POP();
        stack[address] = this->gpr;
    }

    /* Pops two values from stack and pushes their sum onto stack*/
    void ADD() {
        int a = this->POP();
        int b = this->POP();
        this->gpr = a + b;
        this->PUSH();
    }

    /* Pops two values from stack and pushes their sum onto stack*/
    /* Note: top of stack is subtracted from second value on stack*/
    void SUB() {
        int b = this->POP();
        int a = this->POP();
        this->gpr = a - b;
        this->PUSH();
    }

    /* Pops two values from stack and pushes their product onto stack*/
    void MUL() {
        int a = this->POP();
        int b = this->POP();
        this->gpr = a * b;
        this->PUSH();
    }

    /* Pops two values from stack and pushes their sum onto stack*/
    /* Note: second value on stack is divided by first value on stack*/
    void DIV() {
        int b = this->POP();
        int a = this->POP();
        this->gpr = a / b;
        this->PUSH();
    }

    /* Pops two values from stack and pushes the remainder onto stack*/
    /* Note: calculates second value on stack modulus first value on stack*/
    void REM() {
        int a = this->POP();
        int b = this->POP();
        this->gpr = a % b;
        this->PUSH();
    }

    /* Pops two values from stack and pushes 1 if values are equal, 0 otherwise*/
    void EQ() {
        int a = this->POP();
        int b = this->POP();
        if (a == b) this->gpr = 1;
        else this->gpr = 0;
        this->PUSH();
    }

    /* Pops two values from stack and pushes 0 if valeus are equal, 1 otherwise*/
    void NE() {
        int a = this->POP();
        int b = this->POP();
        if (a == b) this->gpr = 0;
        else this->gpr = 1;
        this->PUSH();
    }

    /* Pops two values from stack and pushes 1 if second-top is <= first-top, 0 otherwise*/
    void LE() {
        int b = this->POP();
        int a = this->POP();
        if (a <= b) this->gpr = 1;
        else this->gpr = 0;
        this->PUSH();
    }

    /* Pops two values from stack and pushes 1 if second-top is >= first-top, 0 otherwise*/
    void GE() {
        int b = this->POP();
        int a = this->POP();
        if (a >= b) this->gpr = 1;
        else this->gpr = 0;
        this->PUSH();
    }

    /* Pops two values from stack and pushes 1 if second-top is < first-top, 0 otherwise*/
    void LT() {
        int b = this->POP();
        int a = this->POP();
        if (a < b) this->gpr = 1;
        else this->gpr = 0;
        this->PUSH();
    }

    /* Pops two values from stack and pushes 1 if second-top is > first-top, 0 otherwise*/
    void GT() {
        int b = this->POP();
        int a = this->POP();
        if (a > b) this->gpr = 1;
        else this->gpr = 0;
        this->PUSH();
    }

    /* Updates program counter to specified location if value is not 0*/
    /* Note: top element on stack is value; second element is location. Removes both elements*/
    void BRT() {
        int val = this->POP();
        this->POP(); // Sets gpr to value
        if (val != 0) programCounter = this->gpr;
    }

    /* Updates program counter to specified location if value is not 0*/
    /* Note: top element on stack is value; removes value*/
    void BRT(char* label) {
        this->POP(); // Sets gpr to value
        if (this->gpr != 0) programCounter = this->LABEL(label); // Sets program counter to label
    }

    /* Updates program counter to specified location if value is not 0*/
    /* Note: top element on stack is value; removes value*/
    void BRT(int loc) {
        this->POP(); // Sets gpr to value
        if (this->gpr != 0) programCounter = loc; // Sets program counter to specified value
    }

    /* Updates program counter to specified location if value is not 0*/
    /* Note: top element on stack is value; second element is location. Removes both elements*/
    void BRZ() {
        int val = this->POP();
        this->POP(); // Sets gpr to value
        if (val == 0) programCounter = this->gpr;
    }

    /* Updates program counter to specified location if value is not 0*/
    /* Note: top element on stack is value; removes value*/
    void BRZ(char* label) {
        this->POP(); // Sets gpr to value
        if (this->gpr == 0) programCounter = this->LABEL(label); // Sets program counter to label
    }

    /* Updates program counter to specified location if value is not 0*/
    /* Note: top element on stack is value; removes value*/
    void BRZ(int loc) {
        this->POP(); // Sets gpr to value
        if (this->gpr == 0) programCounter = loc; // Sets program counter to specified value
    }

    /* Sets program counter to top value from stack, removes top value from stack*/
    void JUMP() {
        this->POP();
        programCounter = this->gpr;
    }

    /* Sets program counter to specified location. Stack remains unchanged*/
    void JUMP(char* label) {
        programCounter = this->LABEL(label);
    }

    /* Sets program counter to specified location. Stack remains unchanged*/
    void JUMP(int loc) {
        programCounter = loc;
    }

    /* Prints top value from stack*/
    void PRINT() {
        std::cout << stack[stackPointer] << std::endl;
    }

    /* PRINT OVERLOAD: Prints message passed as parameter*/
    void PRINT(char* message) {
        std::cout << message << std::endl;
    }

    /* Reads input value, adds to top of stack*/
    void READ() {
        int temp;
        std::cin >> temp;
        this->gpr = temp;
        this->PUSH();
    }

    /* Ends execution of program*/
    void END() {
        exit(0);
    }
};
