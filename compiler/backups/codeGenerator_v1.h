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
    PRINT, READ, // IO operations
    LABEL, // Labels
    END // End program
};

// Instruction structure for stack machine
struct Instruction {
    OpCode op;
    std::string arg;  // Could be a value, variable name, or label

    Instruction(OpCode op, const std::string& arg = "") : op(op), arg(arg) {}
};

class CodeGenerator {
private:
    std::vector<Instruction> instructions;
    SymbolTable& symbolTable;
    int labelCounter;
    std::stack<std::string> breakLabels;     // For break statements
    std::stack<std::string> continueLabels;  // For continue statements

    // Helper methods
    std::string generateLabel();
    std::string getOpString(OpCode op) const;

public:
    CodeGenerator(SymbolTable& st);
    
    // Generate code from AST
    void generate(ASTNode* root);
    
    // Node type specific code generation methods
    void generateProgram(ASTNode* node);                // 1
    void generateDeclarationList(ASTNode* node); 
    void generateDeclaration(ASTNode* node);
    void generateVarDeclaration(ASTNode* node);                 // Empty function
    void generateTypeSpecifier(ASTNode* node);                  // Empty  function
    void generateFunDeclaration(ASTNode* node);         // 6
    void generateParams(ASTNode* node);                         // Empty function
    void generateParamList(ASTNode* node);                      // Empty function
    void generateParam(ASTNode* node);                          // Empty function
    void generateCompoundStmt(ASTNode* node);
    void generateLocalDeclarations(ASTNode* node);     // 11    // Empty function
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
    void generateVar(ASTNode* node, bool isStore = false);
    void generateSimpleExpression(ASTNode* node);
    void generateRelOp(ASTNode* node);                          // Empty function
    void generateAdditiveExpression(ASTNode* node);
    void generateTerm(ASTNode* node);
    void generateFactor(ASTNode* node);
    void generateCall(ASTNode* node);
    void generateArgs(ASTNode* node);                          // Empty function
    void generateArgList(ASTNode* node);                       // Empty function
    
    // Get the generated instructions
    std::vector<Instruction> getInstructions() const;
    
    // Convert instructions to text
    std::vector<std::string> getCode() const;

    // Print the generated code to a file
    void printStackMachineCodeToFile(std::string filename, std::vector<std::string> code);
};

#endif // CODE_GENERATOR_H