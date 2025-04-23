#include <stdio.h>
#include <string>
#include <cstring>
#include <iostream>
#include <algorithm>
#include <fstream>

class Operation {
private:
    // Attributes
    int gpr; // General purpose register

    int stack[1024];
    int stackTop; // Top available slot in memory; last value added at stack[stackTop - 1]
    int stackPointer; // Current frame; memory slot 0 is in stack[stackPointer + 0]

    int programCounter; // Current instruction being executed
    std::string instructions[1024];

    // Helper functions

    /* Returns index of label, -1 if does not exist*/
    int LABEL(std::string label) {
        for (int i = 0; i < 1024; i++) {
            // Trim any whitespace from the instruction
            std::string instr = instructions[i];
            instr.erase(0, instr.find_first_not_of(" \t\n\r"));
            instr.erase(instr.find_last_not_of(" \t\n\r") + 1);
            
            // Check if this is the label we're looking for
            if (instr == label) {
                return i;
            }
        }
        return -1;
    }

    /* Put top value from the stack onto general purpose register without altering stack*/
    int PEEK() {
        this->gpr = stack[stackTop - 1];
        return this->gpr;
    }

    /* Calls appropriate function based on parameters provided */
    /* std::string f: function name*/
    /* std::string s: string parameter */
    /* int i: int parameter */
    void execINSTRUCTION(std::string f, std::string s, int i) {
        // std::cout << f << " | " << s << " | " << i << std::endl; // For loggin

        // Functions with no parameters
        if (s == "" and i == -1) {
            if (f == "CALL") CALL();
            else if (f == "RET") RET();
            else if (f == "RETV") RETV();
            else if (f == "PUSH") PUSH();
            else if (f == "POP") POP();
            else if (f == "DUP") DUP();
            else if (f == "LOAD") LOAD();
            else if (f == "SAVE") SAVE();
            else if (f == "STORE") STORE();
            else if (f == "ADD") ADD();
            else if (f == "SUB") SUB();
            else if (f == "MUL") MUL();
            else if (f == "DIV") DIV();
            else if (f == "REM") REM();
            else if (f == "EQ") EQ();
            else if (f == "NE") NE();
            else if (f == "LE") LE();
            else if (f == "GE") GE();
            else if (f == "LT") LT();
            else if (f == "GT") GT();
            else if (f == "BRT") BRT();
            else if (f == "BRZ") BRZ();
            else if (f == "JUMP") JUMP();
            else if (f == "PRINT") PRINT();
            else if (f == "READ") READ();
            else if (f == "END") END();
        }

        // Functions with string parameters
        else if (s != "" and i == -1) {
            if (f == "PRINT") PRINT(s);
            if (f == "BRT") BRT(s);
            if (f == "BRZ") BRZ(s);
            if (f == "JUMP") JUMP(s);
            if (f == "CALL") CALL(s);
        } 

        // Functions with int parameters
        else if (s == "" and i != -1) {
            if (f == "PUSH") PUSH(i);
            if (f == "BRT") BRT(i);
            if (f == "BRZ") BRZ(i);
            if (f == "JUMP") JUMP(i);
        }
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

    /* Runs stack machine*/
    void run() {
        // Creating a log file to store debug information
        std::ofstream logFile("debuglog.txt", std::ios::app);

        while (programCounter < 1024) {
            // Checking for OOB
            if (instructions[programCounter].empty()) {
                break;
            }

            // Logging current state
            logFile << "Current stack: " << std::endl;
            for (int i = 0; i < stackTop; i++) {
                logFile << stack[i] << " ";
            }
            logFile << std::endl;
            logFile << "Executing instruction: " << programCounter << "|" << instructions[programCounter];
            
            // Executing instruction
            int execPC = programCounter;
            programCounter++;
            
            parseInstruction(instructions[execPC]);
            logFile << "Stack Top: " << stackTop << "|" << stack[stackTop - 1] << std::endl << std::endl;
        }
    }

    /* Parses instruction, executes appropriate */
    void parseInstruction(std::string instruction) {

        // Ignoring everything after a semicolon
        size_t semicolonPos = instruction.find(';');
        if (semicolonPos != std::string::npos) {
            instruction = instruction.substr(0, semicolonPos);
        }

        // Removing whitespace outside of quotes
        bool inQuotes = false;
        for (size_t i = 0; i < instruction.size(); ++i) {
            if (instruction[i] == '"') {
            inQuotes = !inQuotes;
            } else if (!inQuotes && std::isspace(instruction[i])) {
            instruction.erase(i--, 1);
            }
        }

        // instruction.erase(std::remove_if(instruction.begin(), instruction.end(), ::isspace), instruction.end());
        // if (instruction.empty()) {
        //     return;
        // }

        // Finding parantheses, potential parameter
        size_t openParen = instruction.find('(');
        size_t closeParen = instruction.find(')');
        if (openParen != std::string::npos && closeParen != std::string::npos) {
            std::string functionName = instruction.substr(0, openParen);
            std::string params = instruction.substr(openParen + 1, closeParen - openParen - 1);

            if (!params.empty()) {
                if (params[0] == '"' && params[params.length() - 1] == '"') { // Case: String parameter
                    // Parsing string parameter
                    std::string param = params.substr(1, params.length() - 2);
                    execINSTRUCTION(functionName, param, -1);
                } else { // Case: Integer parameter
                    int param = std::stoi(params);
                    execINSTRUCTION(functionName, "", param);
                }
            } else { // Case: No Parameters
                execINSTRUCTION(functionName, "", -1);
            }
        }
        else {
            // This line is a label
            return;
        }

    }

    /* FUNCTIONS */

    /* Calls function. Places return address on stack, updates stack pointer */
    /* Top of stack: Function address */
    /* Second on stack: number of parameters*/
    void CALL() {
        int address = this->POP(); // Get function address from stack
        int numParams = this->PEEK(); // Get number of parameters from stack; leaves numParams on stack

        // Moving numParams to behind the params
        for (int i = 1; i <= numParams; i++) {
            stack[stackTop - i] = stack[stackTop - i - 1];
        }
        stack[stackTop - numParams - 1] = numParams;

        // Get current values of stackPointer, programCounter
        int currStackPointer = stackPointer;
        int currProgramCounter = programCounter;
        
        // Save the current stackPointer and programCounter at the top of stack
        stack[stackTop] = currStackPointer;
        stack[stackTop + 1] = currProgramCounter; // currProgramCounter already incremented in this.run()
        stackTop += 2;
        
        // Update stack pointer to this new frame
        stackPointer = stackTop - 2 - numParams; // stack[stackPointer] should point to the first parameter
                                                 // stack[stackPointer - 1] points to numParams
        
        // Jump to the function address
        programCounter = address;
    }

    /* CALL OVERLOAD: Sets program counter to specified label. Handles stack and frame accordingly */
    /* Note: Accepts a string parameter for the label name */
    void CALL(std::string label) {
        // Find the address of the label
        int address = this->LABEL(label);
        if (address == -1) {
            std::cerr << "Error: Label '" << label << "' not found" << std::endl;
            exit(1);  // Exit with error code
        }
        
        // Get number of parameters from stack
        int numParams = this->PEEK(); // Leaves numParams on stack

        // Moving numParams to behind the params
        for (int i = 1; i <= numParams; i++) {
            stack[stackTop - i] = stack[stackTop - i - 1];
        }
        stack[stackTop - numParams - 1] = numParams;
        
        // Get current values of stackPointer, programCounter
        int currStackPointer = stackPointer;
        int currProgramCounter = programCounter;
        
        // First, save the current stackPointer and programCounter at the top of stack
        stack[stackTop] = currStackPointer;
        stack[stackTop + 1] = currProgramCounter; // currProgramCounter already incremented in this.run()
        stackTop += 2;
        
        // Update stack pointer to this new frame
        stackPointer = stackTop - 2 - numParams; // stack[stackPointer] should point to the first parameter
                                                  // stack[stackPointer - 1] points to numParams

        // Jump to the function address
        programCounter = address;
    }

    /* Return from function without a value
    * Pre Stack: current frame
    * Post Stack: previous frame
    * Side Effect: Stack pointer gets new frame reference.
    * Description: returns from subroutine. Clears current frame. Stack pointer is reset to calling frame.
    */
    void RET() {
        // Get numParams (if not main)
        int numParams = 0;
        if (stackPointer != 0) numParams = stack[stackPointer - 1];

        // Get previous stack pointer from current frame
        int prevStackPointer = stack[stackPointer + numParams];
        
        // Get return address from current frame (saved next to stack pointer)
        int returnAddress = stack[stackPointer + numParams + 1];
        
        // Reset stack top to current stack pointer
        stackTop = stackTop - numParams - 2 - int(numParams != 0); // numParams is stored on stack for all functions except main
        
        // Restore stack pointer to previous frame
        stackPointer = prevStackPointer;
        
        // Jump to return address
        programCounter = returnAddress;
    }

    /* Return from function with a value
    * Pre Stack: current frame (with return value on top)
    * Post Stack: previous frame and return value
    * Side Effect: Stack pointer gets new frame reference. General purpose register is set to return value.
    * Description: Returns from subroutine with a value. Clears current frame. Stack pointer is reset to calling frame.
    * Return value is pushed to the memory stack. General purpose register is set to return value.
    */
    void RETV() {
        // Save the return value from top of stack
        int returnValue = this->POP();
        
        // Get numParams (if not main)
        int numParams = 0;
        if (stackPointer != 0) numParams = stack[stackPointer - 1];

        // Get previous stack pointer from current frame
        int prevStackPointer = stack[stackPointer + numParams];
        
        // Get return address from current frame (saved next to stack pointer)
        int returnAddress = stack[stackPointer + numParams + 1];
        
        // Reset stack top to current stack pointer
        stackTop = stackTop - numParams - 2 - int(numParams != 0); // numParams is stored on stack for all functions except main
        
        // Restore stack pointer to previous frame
        stackPointer = prevStackPointer;
        
        // Jump to return address
        programCounter = returnAddress;
        
        // Push the return value to the stack
        this->gpr = returnValue;
        this->PUSH();
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
    /* Could be changed to void if every call immediately accessed gpr*/
    int POP() {
        int val = stack[stackTop - 1];
        stackTop -= 1;
        this->gpr = val;
        return val; 
    }

    /* Duplicates value on top of stack*/
    void DUP() {
        PEEK();
        PUSH();
    }

    /* Loads value from specified location in memory into top cell of stack*/
    void LOAD() {
        int address = this->POP(); // Get the address from top of stack
        // The address is relative to the current frame (stackPointer)
        this->gpr = stack[stackPointer + address]; 
        this->PUSH(); // Push the loaded value back to stack
    }

    /* Saves element on stack to specified location without removing element*/
    /* Note: second value on stack is element; first value on stack is address*/
    void SAVE() { 
        int address = this->POP();
        this->PEEK();
        stack[stackPointer + address] = this->gpr;
    }

    /* Saves element on stack to specified location while removing element*/
    /* Note: second value on stack is element; first value on stack is address*/
    void STORE() {
        int address = this->POP();
        this->POP();
        stack[stackPointer + address] = this->gpr;
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
        int b = this->POP();
        int a = this->POP();
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
    void BRT(std::string label) {
        this->POP(); // Sets gpr to value
        if (this->gpr != 0) programCounter = this->LABEL(label); // Sets program counter to label
    }

    /* Updates program counter to specified location if value is not 0*/
    /* Note: top element on stack is value; removes value*/
    void BRT(int loc) {
        this->POP(); // Sets gpr to value
        if (this->gpr != 0) programCounter = loc; // Sets program counter to specified value
    }

    /* Updates program counter to specified location if value is 0*/
    /* Note: top element on stack is value; second element is location. Removes both elements*/
    void BRZ() {
        int val = this->POP();
        this->POP(); // Sets gpr to value
        if (val == 0) programCounter = this->gpr;
    }

    /* Updates program counter to specified location if value is 0*/
    /* Note: top element on stack is value; removes value*/
    void BRZ(std::string label) {
        this->POP(); // Sets gpr to value
        if (this->gpr == 0) programCounter = this->LABEL(label); // Sets program counter to label
    }

    /* Updates program counter to specified location if value is 0*/
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
    void JUMP(std::string label) {
        programCounter = this->LABEL(label);
    }

    /* Sets program counter to specified location. Stack remains unchanged*/
    void JUMP(int loc) {
        programCounter = loc;
    }

    /* Prints top value from stack*/
    void PRINT() {
        std::cout << stack[stackTop-1] << std::endl;
    }

    /* PRINT OVERLOAD: Prints message passed as parameter*/
    void PRINT(std::string message) {
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
