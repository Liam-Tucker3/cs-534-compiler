#include <stdio.h>
#include <string>
#include <cstring>
#include <iostream>
#include <algorithm>
#include <fstream>
#include <tuple>

class Operation {
private:
    // Attributes
    int iGpr; // General purpose register for int
    float fGpr; // General purpose register for float
    bool tGpr; // Type of GP register (false for int, true for float)

    int intStack[1024];
    float floatStack[1024];
    bool typeStack[1024]; // false for int, true for float
    int stackTop; // Top available slot in memory; last value added at intStack[stackTop - 1]
    int stackPointer; // Current frame; memory slot 0 is in intStack[stackPointer + 0]

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

    /* Returns top value from stacks without modifying values*/
    std::tuple<int, float, bool> PEEK() {
        std::tuple<int, float, bool> retVal = std::make_tuple(intStack[stackTop - 1], floatStack[stackTop - 1], typeStack[stackTop - 1]);
        this->iGpr = std::get<0>(retVal);
        this->fGpr = std::get<1>(retVal);
        this->tGpr = std::get<2>(retVal); // Set general purpose register to peeked value
        return retVal;
    }

    /* Calls appropriate function based on parameters provided */
    /* std::string f: function name*/
    /* std::string s: string parameter */
    /* int i: int parameter */
    /* float fl: float parameter */
    void execINSTRUCTION(std::string f, std::string s = "", int i = -1, float fl = -1.0f) {
        // Functions with no parameters
        if (s == "" && i == -1 && fl == -1.0f) {
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
            else if (f == "READF") READF();
            else if (f == "END") END();
            else if (f == "INT") INT();
            else if (f == "FLOAT") FLOAT();
        }

        // Functions with string parameters
        else if (s != "" && i == -1 && fl == -1.0f) {
            if (f == "PRINT") PRINT(s);
            else if (f == "BRT") BRT(s);
            else if (f == "BRZ") BRZ(s);
            else if (f == "JUMP") JUMP(s);
            else if (f == "CALL") CALL(s);
        } 

        // Functions with int parameters
        else if (s == "" && i != -1 && fl == -1.0f) {
            if (f == "PUSH") PUSH(i);
            else if (f == "BRT") BRT(i); // BRT, BRZ, JUMP param is index, not variable
            else if (f == "BRZ") BRZ(i);
            else if (f == "JUMP") JUMP(i);
        }
        
        // Functions with float parameters
        else if (s == "" && i == -1 && fl != -1.0f) {
            if (f == "PUSH") PUSH(fl); // For all other instructions, type is inferred from stack
        }
    }

public:
    /* Empty constructor */
    // Should never be called
    Operation() {
        iGpr = 0;
        fGpr = 0.0f;
        tGpr = false; // false for int, true for float
        stackTop = 0;
        stackPointer = 0;
        programCounter = 0;
        for (int i = 0; i < 1024; i++) {
            intStack[i] = 0;
            floatStack[i] = 0.0f;
            typeStack[i] = false; // false for int, true for float
            instructions[i] = "";
        }
    }

    /* Constructor with one parameter: filename, a file containing the programs to be executed*/
    Operation(std::string& filename) {
        iGpr = 0;
        fGpr = 0.0f;
        tGpr = false;
        stackTop = 0;
        stackPointer = 0;
        programCounter = 0;
        for (int i = 0; i < 1024; i++) {
            intStack[i] = 0;
            floatStack[i] = 0.0f;
            typeStack[i] = false;
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
                if (typeStack[i] == false) { // Integer value
                    logFile << intStack[i] << " ";
                } else { // Float value
                    logFile << floatStack[i] << " ";
                }
            }
            logFile << std::endl;
            logFile << "Executing instruction: " << programCounter << "|" << instructions[programCounter];
            
            // Executing instruction
            int execPC = programCounter;
            programCounter++;
            
            parseInstruction(instructions[execPC]);
            
            logFile << "Stack Top: " << stackTop << "|";
            if (stackTop > 0) {
                if (typeStack[stackTop - 1] == false) { // Integer value
                    logFile << intStack[stackTop - 1] << "(int)" << std::endl;
                } else { // Float value
                    logFile << floatStack[stackTop - 1] << "(float)" << std::endl;
                }
            } else {
                logFile << "Stack empty" << std::endl;
            }
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
                    execINSTRUCTION(functionName, param, -1, -1.0f);
                } else { // Case: Numeric parameter
                    // Check if it's a float (contains a decimal point)
                    if (params.find('.') != std::string::npos) {
                        float param = std::stof(params);
                        execINSTRUCTION(functionName, "", -1, param);
                    } else { // Integer
                        int param = std::stoi(params);
                        execINSTRUCTION(functionName, "", param, -1.0f);
                    }
                }
            } else { // Case: No Parameters
                execINSTRUCTION(functionName);
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
        std::tuple<int, float, bool> addressVal = this->POP(); // Get function address from stack
        int address = std::get<0>(addressVal); // Address should always be int
        
        std::tuple<int, float, bool> numParamsVal = this->PEEK(); // Get number of parameters from stack
        int numParams = std::get<0>(numParamsVal); // Num params should always be int

        // Moving numParams to behind the params
        for (int i = 1; i <= numParams; i++) {
            intStack[stackTop - i] = intStack[stackTop - i - 1];
            floatStack[stackTop - i] = floatStack[stackTop - i - 1];
            typeStack[stackTop - i] = typeStack[stackTop - i - 1];
        }
        
        intStack[stackTop - numParams - 1] = std::get<0>(numParamsVal);
        floatStack[stackTop - numParams - 1] = std::get<1>(numParamsVal);
        typeStack[stackTop - numParams - 1] = std::get<2>(numParamsVal);

        // Get current values of stackPointer, programCounter
        int currStackPointer = stackPointer;
        int currProgramCounter = programCounter;
        
        // Save the current stackPointer and programCounter at the top of stack
        intStack[stackTop] = currStackPointer;
        floatStack[stackTop] = 0.0f;
        typeStack[stackTop] = false;
        
        intStack[stackTop + 1] = currProgramCounter;
        floatStack[stackTop + 1] = 0.0f;
        typeStack[stackTop + 1] = false;
        
        stackTop += 2;
        
        // Update stack pointer to this new frame
        stackPointer = stackTop - 2 - numParams;
        
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
        std::tuple<int, float, bool> numParamsVal = this->PEEK(); 
        int numParams = std::get<0>(numParamsVal); // numParams should always be int

        // Moving numParams to behind the params
        for (int i = 1; i <= numParams; i++) {
            intStack[stackTop - i] = intStack[stackTop - i - 1];
            floatStack[stackTop - i] = floatStack[stackTop - i - 1];
            typeStack[stackTop - i] = typeStack[stackTop - i - 1];
        }
        
        intStack[stackTop - numParams - 1] = std::get<0>(numParamsVal);
        floatStack[stackTop - numParams - 1] = std::get<1>(numParamsVal);
        typeStack[stackTop - numParams - 1] = std::get<2>(numParamsVal);
        
        // Get current values of stackPointer, programCounter
        int currStackPointer = stackPointer;
        int currProgramCounter = programCounter;
        
        // Save the current stackPointer and programCounter at the top of stack
        intStack[stackTop] = currStackPointer;
        floatStack[stackTop] = 0.0f;
        typeStack[stackTop] = false;
        
        intStack[stackTop + 1] = currProgramCounter;
        floatStack[stackTop + 1] = 0.0f;
        typeStack[stackTop + 1] = false;
        
        stackTop += 2;
        
        // Update stack pointer to this new frame
        stackPointer = stackTop - 2 - numParams;
        
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
        if (stackPointer != 0) {
            std::tuple<int, float, bool> numParamsVal;
            numParamsVal = std::make_tuple(intStack[stackPointer - 1], floatStack[stackPointer - 1], typeStack[stackPointer - 1]);
            numParams = std::get<0>(numParamsVal); // numParams should always be int
        }

        // Get previous stack pointer from current frame
        int prevStackPointer = intStack[stackPointer + numParams];
        
        // Get return address from current frame (saved next to stack pointer)
        int returnAddress = intStack[stackPointer + numParams + 1];
        
        // Reset stack top to current stack pointer
        stackTop = stackPointer - int(stackPointer != 0); // If not in main, subtract 1 for numParams
        
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
        std::tuple<int, float, bool> returnValue = this->POP();
        
        // Get numParams (if not main)
        int numParams = 0;
        if (stackPointer != 0) {
            std::tuple<int, float, bool> numParamsVal;
            numParamsVal = std::make_tuple(intStack[stackPointer - 1], floatStack[stackPointer - 1], typeStack[stackPointer - 1]);
            numParams = std::get<0>(numParamsVal); // Address should always be int
        }

        // Get previous stack pointer from current frame
        int prevStackPointer = intStack[stackPointer + numParams];
        
        // Get return address from current frame
        int returnAddress = intStack[stackPointer + numParams + 1];
        
        // Reset stack top to current stack pointer
        stackTop = stackPointer - int(stackPointer != 0); // If not in main, subtract 1 for numParams
        
        // Restore stack pointer to previous frame
        stackPointer = prevStackPointer;
        
        // Jump to return address
        programCounter = returnAddress;
        
        // Set GPR to return value and push it to stack
        this->iGpr = std::get<0>(returnValue);
        this->fGpr = std::get<1>(returnValue);
        this->tGpr = std::get<2>(returnValue);
        this->PUSH();
    }

    /* Puts value from general purpose register onto stack*/
    void PUSH() {
        intStack[stackTop] = this->iGpr;
        floatStack[stackTop] = this->fGpr;
        typeStack[stackTop] = this->tGpr;
        stackTop += 1;
    }

    /* PUSH OVERLOAD: Puts specified integer value onto stack */
    void PUSH(int value) {
        intStack[stackTop] = value;
        floatStack[stackTop] = 0.0f; // Zero out float value
        typeStack[stackTop] = false; // Set type to int
        stackTop += 1;
    }
    
    /* PUSH OVERLOAD: Puts specified float value onto stack */
    void PUSH(float value) {
        intStack[stackTop] = 0; // Zero out int value
        floatStack[stackTop] = (float) value;
        typeStack[stackTop] = true; // Set type to float
        stackTop += 1;
    }

    /* Removes top value from stack, places it on general purpose register*/
    std::tuple<int, float, bool> POP() {
        std::tuple<int, float, bool> retVal = std::make_tuple(intStack[stackTop - 1], floatStack[stackTop - 1], typeStack[stackTop - 1]);
        stackTop -= 1;
        this->iGpr = std::get<0>(retVal);
        this->fGpr = std::get<1>(retVal);
        this->tGpr = std::get<2>(retVal); // Set general purpose register to popped value
        return retVal; 
    }

    /* Duplicates value on top of stack*/
    void DUP() {
        std::tuple<int, float, bool> val = this->PEEK();
        this->iGpr = std::get<0>(val);
        this->fGpr = std::get<1>(val);
        this->tGpr = std::get<2>(val);
        this->PUSH();
    }

    /* Loads value from specified location in memory into top cell of stack*/
    void LOAD() {
        std::tuple<int, float, bool> vals = this->POP();
        int address = std::get<0>(vals); // Address should always be int
        
        // The address is relative to the current frame (stackPointer)
        // Updating GPRs
        this->iGpr = intStack[stackPointer + address]; 
        this->fGpr = floatStack[stackPointer + address]; 
        this->tGpr = typeStack[stackPointer + address]; 

        this->PUSH(); // Push the loaded value back to stack
    }

    /* Saves element on stack to specified location without removing element*/
    /* Note: second value on stack is element; first value on stack is address*/
    void SAVE() { 
        std::tuple<int, float, bool> addressVal = this->POP();
        int address = std::get<0>(addressVal); // Address should always be int
        
        std::tuple<int, float, bool> value = this->PEEK();
        
        intStack[stackPointer + address] = std::get<0>(value);
        floatStack[stackPointer + address] = std::get<1>(value);
        typeStack[stackPointer + address] = std::get<2>(value);
    }

    /* Saves element on stack to specified location while removing element*/
    /* Note: second value on stack is element; first value on stack is address*/
    void STORE() {
        std::tuple<int, float, bool> addressVal = this->POP();
        int address = std::get<0>(addressVal); // Address should always be int
        
        std::tuple<int, float, bool> value = this->POP();
        
        intStack[stackPointer + address] = std::get<0>(value);
        floatStack[stackPointer + address] = std::get<1>(value);
        typeStack[stackPointer + address] = std::get<2>(value);
    }

    /* Pops two values from stack and pushes their sum onto stack*/
    void ADD() {
        std::tuple<int, float, bool> b = this->POP();
        std::tuple<int, float, bool> a = this->POP();
        
        bool typeA = std::get<2>(a);
        bool typeB = std::get<2>(b);
        
        if (typeA && typeB) { // Both floats
            this->fGpr = std::get<1>(a) + std::get<1>(b);
            this->tGpr = true;
        } else if (typeA) { // A is float, B is int
            this->fGpr = std::get<1>(a) + static_cast<float>(std::get<0>(b));
            this->tGpr = true;
        } else if (typeB) { // A is int, B is float
            this->fGpr = static_cast<float>(std::get<0>(a)) + std::get<1>(b);
            this->tGpr = true;
        } else { // Both ints
            this->iGpr = std::get<0>(a) + std::get<0>(b);
            this->tGpr = false;
        }
        
        this->PUSH();
    }

    /* Pops two values from stack and pushes their difference onto stack*/
    /* Note: top of stack is subtracted from second value on stack*/
    void SUB() {
        std::tuple<int, float, bool> b = this->POP();
        std::tuple<int, float, bool> a = this->POP();
        
        bool typeA = std::get<2>(a);
        bool typeB = std::get<2>(b);
        
        if (typeA && typeB) { // Both floats
            this->fGpr = std::get<1>(a) - std::get<1>(b);
            this->tGpr = true;
        } else if (typeA) { // A is float, B is int
            this->fGpr = std::get<1>(a) - static_cast<float>(std::get<0>(b));
            this->tGpr = true;
        } else if (typeB) { // A is int, B is float
            this->fGpr = static_cast<float>(std::get<0>(a)) - std::get<1>(b);
            this->tGpr = true;
        } else { // Both ints
            this->iGpr = std::get<0>(a) - std::get<0>(b);
            this->tGpr = false;
        }
        
        this->PUSH();
    }

    /* Pops two values from stack and pushes their product onto stack*/
    void MUL() {
        std::tuple<int, float, bool> b = this->POP();
        std::tuple<int, float, bool> a = this->POP();
        
        bool typeA = std::get<2>(a);
        bool typeB = std::get<2>(b);
        
        if (typeA && typeB) { // Both floats
            this->fGpr = std::get<1>(a) * std::get<1>(b);
            this->tGpr = true;
        } else if (typeA) { // A is float, B is int
            this->fGpr = std::get<1>(a) * static_cast<float>(std::get<0>(b));
            this->tGpr = true;
        } else if (typeB) { // A is int, B is float
            this->fGpr = static_cast<float>(std::get<0>(a)) * std::get<1>(b);
            this->tGpr = true;
        } else { // Both ints
            this->iGpr = std::get<0>(a) * std::get<0>(b);
            this->tGpr = false;
        }
        
        this->PUSH();
    }

    /* Pops two values from stack and pushes their quotient onto stack*/
    /* Note: second value on stack is divided by first value on stack*/
    void DIV() {
        std::tuple<int, float, bool> b = this->POP();
        std::tuple<int, float, bool> a = this->POP();
        
        bool typeA = std::get<2>(a);
        bool typeB = std::get<2>(b);
        
        if (typeA && typeB) { // Both floats
            this->fGpr = std::get<1>(a) / std::get<1>(b);
            this->tGpr = true;
        } else if (typeA) { // A is float, B is int
            this->fGpr = std::get<1>(a) / static_cast<float>(std::get<0>(b));
            this->tGpr = true;
        } else if (typeB) { // A is int, B is float
            this->fGpr = static_cast<float>(std::get<0>(a)) / std::get<1>(b);
            this->tGpr = true;
        } else { // Both ints
            // Only case of integer division
            this->iGpr = std::get<0>(a) / std::get<0>(b);
            this->tGpr = false;
        }
        this->PUSH();
    }

    /* Pops two values from stack and pushes the remainder onto stack*/
    /* Note: calculates second value on stack modulus first value on stack*/
    void REM() {
        std::tuple<int, float, bool> b = this->POP();
        std::tuple<int, float, bool> a = this->POP();
        
        // REM is only defined for integers, so we'll convert to int if needed
        // This should never be called on floats
        int aVal = std::get<2>(a) ? static_cast<int>(std::get<1>(a)) : std::get<0>(a);
        int bVal = std::get<2>(b) ? static_cast<int>(std::get<1>(b)) : std::get<0>(b);
        
        this->iGpr = aVal % bVal;
        this->tGpr = false; // Result is always an int
        
        this->PUSH();
    }

    /* Pops two values from stack and pushes 1 if values are equal, 0 otherwise*/
    // Should only be callde for the same type
    void EQ() {
        std::tuple<int, float, bool> b = this->POP();
        std::tuple<int, float, bool> a = this->POP();
        
        bool equal = false;
        bool typeA = std::get<2>(a);
        bool typeB = std::get<2>(b);
        
        if (typeA && typeB) { // Both floats
            equal = std::get<1>(a) == std::get<1>(b);
        } else if (typeA) { // A is float, B is int
            equal = std::get<1>(a) == static_cast<float>(std::get<0>(b));
        } else if (typeB) { // A is int, B is float
            equal = static_cast<float>(std::get<0>(a)) == std::get<1>(b);
        } else { // Both ints
            equal = std::get<0>(a) == std::get<0>(b);
        }
        
        this->iGpr = equal ? 1 : 0;
        this->tGpr = false; // Result is always an int
        
        this->PUSH();
    }

    /* Pops two values from stack and pushes 0 if values are equal, 1 otherwise*/
    void NE() {
        std::tuple<int, float, bool> b = this->POP();
        std::tuple<int, float, bool> a = this->POP();
        
        bool equal = false;
        bool typeA = std::get<2>(a);
        bool typeB = std::get<2>(b);
        
        if (typeA && typeB) { // Both floats
            equal = std::get<1>(a) == std::get<1>(b);
        } else if (typeA) { // A is float, B is int
            equal = std::get<1>(a) == static_cast<float>(std::get<0>(b));
        } else if (typeB) { // A is int, B is float
            equal = static_cast<float>(std::get<0>(a)) == std::get<1>(b);
        } else { // Both ints
            equal = std::get<0>(a) == std::get<0>(b);
        }
        
        this->iGpr = equal ? 0 : 1;
        this->tGpr = false; // Result is always an int
        
        this->PUSH();
    }

    /* Pops two values from stack and pushes 1 if second-top is <= first-top, 0 otherwise*/
    void LE() {
        std::tuple<int, float, bool> b = this->POP();
        std::tuple<int, float, bool> a = this->POP();
        
        bool result = false;
        bool typeA = std::get<2>(a);
        bool typeB = std::get<2>(b);
        
        if (typeA && typeB) { // Both floats
            result = std::get<1>(a) <= std::get<1>(b);
        } else if (typeA) { // A is float, B is int
            result = std::get<1>(a) <= static_cast<float>(std::get<0>(b));
        } else if (typeB) { // A is int, B is float
            result = static_cast<float>(std::get<0>(a)) <= std::get<1>(b);
        } else { // Both ints
            result = std::get<0>(a) <= std::get<0>(b);
        }
        
        this->iGpr = result ? 1 : 0;
        this->tGpr = false; // Result is always an int
        
        this->PUSH();
    }

    /* Pops two values from stack and pushes 1 if second-top is >= first-top, 0 otherwise*/
    void GE() {
        std::tuple<int, float, bool> b = this->POP();
        std::tuple<int, float, bool> a = this->POP();
        
        bool result = false;
        bool typeA = std::get<2>(a);
        bool typeB = std::get<2>(b);
        
        if (typeA && typeB) { // Both floats
            result = std::get<1>(a) >= std::get<1>(b);
        } else if (typeA) { // A is float, B is int
            result = std::get<1>(a) >= static_cast<float>(std::get<0>(b));
        } else if (typeB) { // A is int, B is float
            result = static_cast<float>(std::get<0>(a)) >= std::get<1>(b);
        } else { // Both ints
            result = std::get<0>(a) >= std::get<0>(b);
        }
        
        this->iGpr = result ? 1 : 0;
        this->tGpr = false; // Result is always an int
        
        this->PUSH();
    }

    /* Pops two values from stack and pushes 1 if second-top is < first-top, 0 otherwise*/
    void LT() {
        std::tuple<int, float, bool> b = this->POP();
        std::tuple<int, float, bool> a = this->POP();
        
        bool result = false;
        bool typeA = std::get<2>(a);
        bool typeB = std::get<2>(b);
        
        if (typeA && typeB) { // Both floats
            result = std::get<1>(a) < std::get<1>(b);
        } else if (typeA) { // A is float, B is int
            result = std::get<1>(a) < static_cast<float>(std::get<0>(b));
        } else if (typeB) { // A is int, B is float
            result = static_cast<float>(std::get<0>(a)) < std::get<1>(b);
        } else { // Both ints
            result = std::get<0>(a) < std::get<0>(b);
        }
        
        this->iGpr = result ? 1 : 0;
        this->tGpr = false; // Result is always an int
        
        this->PUSH();
    }

    /* Pops two values from stack and pushes 1 if second-top is > first-top, 0 otherwise*/
    void GT() {
        std::tuple<int, float, bool> b = this->POP();
        std::tuple<int, float, bool> a = this->POP();
        
        bool result = false;
        bool typeA = std::get<2>(a);
        bool typeB = std::get<2>(b);
        
        if (typeA && typeB) { // Both floats
            result = std::get<1>(a) > std::get<1>(b);
        } else if (typeA) { // A is float, B is int
            result = std::get<1>(a) > static_cast<float>(std::get<0>(b));
        } else if (typeB) { // A is int, B is float
            result = static_cast<float>(std::get<0>(a)) > std::get<1>(b);
        } else { // Both ints
            result = std::get<0>(a) > std::get<0>(b);
        }
        
        this->iGpr = result ? 1 : 0;
        this->tGpr = false; // Result is always an int
        
        this->PUSH();
    }

    /* Updates program counter to specified location if value is not 0*/
    /* Note: top element on stack is value; second element is location. Removes both elements*/
    void BRT() {
        std::tuple<int, float, bool> condition = this->POP();
        std::tuple<int, float, bool> destination = this->POP();
        
        // Get the boolean value for condition
        bool cond;
        if (std::get<2>(condition)) { // Float
            cond = std::get<1>(condition) != 0.0f;
        } else { // Int
            cond = std::get<0>(condition) != 0;
        }
        
        // Get the address
        int address;
        if (std::get<2>(destination)) { // Float
            address = static_cast<int>(std::get<1>(destination));
        } else { // Int
            address = std::get<0>(destination); // Should always be int
        }
        
        if (cond) {
            programCounter = address;
        }
    }

    /* Updates program counter to specified location if value is not 0*/
    /* Note: top element on stack is value; removes value*/
    void BRT(std::string label) {
        std::tuple<int, float, bool> condition = this->POP();
        
        // Get the boolean value for condition
        bool cond;
        if (std::get<2>(condition)) { // Float
            cond = std::get<1>(condition) != 0.0f;
        } else { // Int
            cond = std::get<0>(condition) != 0; // Should always be int
        }
        
        if (cond) {
            programCounter = this->LABEL(label);
        }
    }

    /* Updates program counter to specified location if value is not 0*/
    /* Note: top element on stack is value; removes value*/
    void BRT(int loc) {
        std::tuple<int, float, bool> condition = this->POP();
        
        // Get the boolean value for condition
        bool cond;
        if (std::get<2>(condition)) { // Float
            cond = std::get<1>(condition) != 0.0f;
        } else { // Int
            cond = std::get<0>(condition) != 0; // Should always be int
        }
        
        if (cond) {
            programCounter = loc;
        }
    }

    /* Updates program counter to specified location if value is 0*/
    /* Note: top element on stack is value; second element is location. Removes both elements*/
    void BRZ() {
        std::tuple<int, float, bool> condition = this->POP();
        std::tuple<int, float, bool> destination = this->POP();
        
        // Get the boolean value for condition
        bool cond;
        if (std::get<2>(condition)) { // Float
            cond = std::get<1>(condition) == 0.0f;
        } else { // Int
            cond = std::get<0>(condition) == 0; // Should always be int
        }
        
        // Get the address
        int address;
        if (std::get<2>(destination)) { // Float
            address = static_cast<int>(std::get<1>(destination));
        } else { // Int
            address = std::get<0>(destination); // Address should always be int
        }
        
        if (cond) {
            programCounter = address;
        }
    }

    /* Updates program counter to specified location if value is 0*/
    /* Note: top element on stack is value; removes value*/
    void BRZ(std::string label) {
        std::tuple<int, float, bool> condition = this->POP();
        
        // Get the boolean value for condition
        bool cond;
        if (std::get<2>(condition)) { // Float
            cond = std::get<1>(condition) == 0.0f;
        } else { // Int
            cond = std::get<0>(condition) == 0; // Should always be int
        }
        
        if (cond) {
            programCounter = this->LABEL(label);
        }
    }

    /* Updates program counter to specified location if value is 0*/
    /* Note: top element on stack is value; removes value*/
    void BRZ(int loc) {
        std::tuple<int, float, bool> condition = this->POP();
        
        // Get the boolean value for condition
        bool cond;
        if (std::get<2>(condition)) { // Float
            cond = std::get<1>(condition) == 0.0f;
        } else { // Int
            cond = std::get<0>(condition) == 0; // Should always be int
        }
        
        if (cond) {
            programCounter = loc;
        }
    }

    /* Sets program counter to top value from stack, removes top value from stack*/
    void JUMP() {
        std::tuple<int, float, bool> destination = this->POP();
        
        // Get the address
        int address;
        if (std::get<2>(destination)) { // Float
            address = static_cast<int>(std::get<1>(destination));
        } else { // Int
            address = std::get<0>(destination); // Address should always be int
        }
        
        programCounter = address;
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
        if (typeStack[stackTop-1]) {
            std::cout << floatStack[stackTop-1] << std::endl;
        } else {
            std::cout << intStack[stackTop-1] << std::endl;
        }
    }

    /* PRINT OVERLOAD: Prints message passed as parameter*/
    void PRINT(std::string message) {
        std::cout << message << std::endl;
    }

    /* Reads integer input value, adds to top of stack*/
    void READ() {
        int temp;
        std::cin >> temp;
        this->iGpr = temp;
        this->tGpr = false;
        this->PUSH();
    }

    /* Reads float input value, adds to top of stack*/
    void READF() {
        float temp;
        std::cin >> temp;
        this->fGpr = temp;
        this->tGpr = true;
        this->PUSH();
    }

    /* Converts the top value on the stack to an INT*/
    void INT() {
        if (typeStack[stackTop-1] == 0) return; // Already an int, no need to convert

        // Need to convert to int
        std::tuple<int, float, bool> val = this->POP();
        this->iGpr = (int) std::get<1>(val);
        this->fGpr = 0.0f; // Zero out float value
        this->tGpr = false;
        this->PUSH();
    }

    /* Converts the top value on the stack to a FLOAT*/
    void FLOAT() {
        if (typeStack[stackTop-1] == 1) return; // Already a float, no need to convert
        
        // Need to convert to float
        std::tuple<int, float, bool> val = this->POP();
        this->fGpr = (float) std::get<0>(val);
        this->iGpr = 0; // Zero out int value
        this->tGpr = true;
        this->PUSH();
    }

    /* Ends execution of program*/
    void END() {
        exit(0);
    }
};