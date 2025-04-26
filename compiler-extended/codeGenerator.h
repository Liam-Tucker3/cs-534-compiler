#ifndef CODE_GENERATOR_H
#define CODE_GENERATOR_H

#include <vector>
#include <string>
#include <unordered_map>
#include <stack>
#include "ast.h"

// Forward declaration
class SymbolTable;

// Stack machine instruction opcodes
enum class OpCode {
    CALL, RET, RETV, // Call and return
    PUSH, POP, // Add, remove from stack top
    DUP, // Duplicate
    LOAD, SAVE, STORE, // Add, remove from stack 
    ADD, SUB, MUL, DIV, REM, // Arithmetic    // Push value onto stack
    EQ, NE, LE, GE, LT, GT, // Comparisons
    BRT, BRZ, JUMP, // Branching
    PRINT, READ, READF, // IO operations - added READF for float input
    LABEL, // Labels
    INT, FLOAT, // Type conversion operations
    END // End program
};

// Instruction structure for stack machine
struct Instruction {
    OpCode op;
    std::string arg;  // Could be a value, variable name, or label

    Instruction(OpCode op, const std::string& arg = "") : op(op), arg(arg) {}
};

// Structure to track variable information for the stack machine
struct VariableInfo {
    int stackOffset;  // Offset from the current frame's stack pointer
    bool isArray;     // Whether this is an array variable
    int arraySize;    // Size of the array (if isArray is true)
    bool isFloat;     // Whether this is a float variable
};

class CodeGenerator {
private:
    std::vector<Instruction> instructions;
    SymbolTable& symbolTable;
    int labelCounter;
    std::stack<std::string> breakLabels;     // For break statements
    std::stack<std::string> continueLabels;  // For continue statements
    
    // Map to track variable locations in the stack frame
    std::unordered_map<std::string, VariableInfo> frameVariables; // Could (Should?) be part of symbol table
    int localVarCount;  // Counter for local variables in the current function frame

    // Helper methods
    std::string generateLabel();
    std::string getOpString(OpCode op) const;

    // Adds a variable to the frame mapping
    void addVariableToFrame(const std::string& varName, bool isArray = false, int arraySize = -1, bool isFloat = false);
    
    // Gets variable offset in the current frame
    int getVariableOffset(const std::string& varName);
    
    // Checks if a variable is a float type
    bool isVariableFloat(const std::string& varName);


public:
    CodeGenerator(SymbolTable& st);
    
    // Generate code from AST
    void generate(ASTNode* root);
    
    // Node type specific code generation methods
    void generateProgram(ASTNode* node);                // 1
    void generateDeclarationList(ASTNode* node); 
    void generateDeclaration(ASTNode* node);
    void generateVarDeclaration(ASTNode* node);                 // Modified to track frame variables
    void generateTypeSpecifier(ASTNode* node);                  // Empty function
    void generateFunDeclaration(ASTNode* node);         // 6
    void generateParams(ASTNode* node);                         // Modified to track parameters
    void generateParamList(ASTNode* node);                      // Modified to track parameters
    void generateParam(ASTNode* node);                          // Modified to track parameters
    void generateCompoundStmt(ASTNode* node);
    void generateLocalDeclarations(ASTNode* node);     // 11    // Modified to track local variables
    void generateStatementList(ASTNode* node);
    void generateStatement(ASTNode* node);
    void generateIOStmt(ASTNode* node);
    void generateInputStmt(ASTNode* node);
    void generateOutputStmt(ASTNode* node);            // 16
    void generateExpressionStmt(ASTNode* node);
    void generateSelectionStmt(ASTNode* node);
    void generateIterationStmt(ASTNode* node);
    void generateReturnStmt(ASTNode* node);
    void generateExpression(ASTNode* node);            // 21
    void generateVar(ASTNode* node, bool isStore = false);   // Modified for stack offsets
    void generateSimpleExpression(ASTNode* node);
    void generateRelOp(ASTNode* node);                          // Empty function
    void generateAdditiveExpression(ASTNode* node);     // 25 and 26
    void generateTerm(ASTNode* node);
    void generateFactor(ASTNode* node);
    void generateCall(ASTNode* node);
    void generateArgs(ASTNode* node);                   // 31   // Empty function
    void generateArgList(ASTNode* node);                       // Empty function
    void generateArrayInitExpression(ASTNode* node, const std::string& arrayName);
    void generateArrayElements(ASTNode* node, const std::string& arrayName, int baseOffset);
    void generateArrayOperation(ASTNode* node, ASTNode* varNode);
    void generateArrayOp(ASTNode* node);                //36     Empty function
    
    // Clear variables at the end of a function
    void clearFrameVariables();
    
    // Get the generated instructions
    std::vector<Instruction> getInstructions() const;
    
    // Convert instructions to text
    std::vector<std::string> getCode() const;

    // Print the generated code to a file
    void printStackMachineCodeToFile(std::string filename, std::vector<std::string> code);
};

#endif // CODE_GENERATOR_H