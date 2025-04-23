#include "codeGenerator.h"
#include <iostream>
#include <sstream>
#include <fstream>

CodeGenerator::CodeGenerator(SymbolTable& st) : 
    symbolTable(st), 
    labelCounter(0), 
    localVarCount(0) {}

// Generating labels
std::string CodeGenerator::generateLabel() {
    return "L" + std::to_string(labelCounter++);
}

std::string CodeGenerator::getOpString(OpCode op) const {
    switch (op) {
        case OpCode::PUSH: return "PUSH";
        case OpCode::POP: return "POP";
        case OpCode::LOAD: return "LOAD";
        case OpCode::STORE: return "STORE";
        case OpCode::SAVE: return "SAVE";
        case OpCode::DUP: return "DUP";
        case OpCode::ADD: return "ADD";
        case OpCode::SUB: return "SUB";
        case OpCode::MUL: return "MUL";
        case OpCode::DIV: return "DIV";
        case OpCode::REM: return "REM";
        case OpCode::EQ: return "EQ";
        case OpCode::NE: return "NEQ";
        case OpCode::LT: return "LT";
        case OpCode::GT: return "GT";
        case OpCode::LE: return "LE";
        case OpCode::GE: return "GE";
        case OpCode::BRT: return "BRT";
        case OpCode::BRZ: return "BRZ";
        case OpCode::LABEL: return "LABEL";
        case OpCode::JUMP: return "JMP";
        case OpCode::CALL: return "CALL";
        case OpCode::RET: return "RET";
        case OpCode::RETV: return "RETV";
        case OpCode::PRINT: return "PRINT";
        case OpCode::READ: return "READ";
        case OpCode::END: return "END";
        default: return "UNKNOWN"; // Should never run
    }
}

// Helper function to add a variable to the frame tracking
void CodeGenerator::addVariableToFrame(const std::string& varName, bool isArray, int arraySize) {
    if (frameVariables.find(varName) == frameVariables.end()) {
        VariableInfo info;
        info.stackOffset = localVarCount++;
        info.isArray = isArray;
        info.arraySize = arraySize;
        frameVariables[varName] = info;
    }
}

// Helper function to get a variable's offset in the current frame
int CodeGenerator::getVariableOffset(const std::string& varName) {
    if (frameVariables.find(varName) != frameVariables.end()) {
        return frameVariables[varName].stackOffset;
    }
    
    std::cerr << "Warning: Variable '" << varName << "' not found in frame" << std::endl;
    return -1; // Error value
}

// Clear all variables at the end of a function
void CodeGenerator::clearFrameVariables() {
    frameVariables.clear();
    localVarCount = 0;
}

// Main: Called to begin generation
void CodeGenerator::generate(ASTNode* root) {
    std::cout << "Calling generate" << std::endl;
    if (!root) return;
    
    switch (root->type) {
        case ASTNodeType::PROGRAM:
            generateProgram(root);
            break;
        default:
            std::cerr << "Unexpected root node type in Main: " << getNodeTypeName(root->type) << std::endl;
            break;
    }
}

// Rule 1: program := declaration-list | epsilon
void CodeGenerator::generateProgram(ASTNode* node) {
    std::cout << "Calling generateProgram" << std::endl;
    if (!node) return;

    // Add a jump to the main function at the start of the program
    instructions.push_back(Instruction(OpCode::JUMP, "main"));
    
    // Process the declaration list
    if (node->children->size() > 0) {
        generateDeclarationList(node->children->at(0));
    }
    
    // Adding END instruction to end of program
    instructions.push_back(Instruction(OpCode::END));
}

// Rule 2: declaration-list := declaration-list type-specifier ID declaration | type-specifier ID declaration
void CodeGenerator::generateDeclarationList(ASTNode* node) {
    std::cout << "Calling generateDeclarationList" << std::endl;
    if (!node) return;
    
    // Process each declaration
    for (ASTNode* child : *node->children) {
        generateDeclaration(child);
    }
}

// Rule 3: declaration := var-declaration | fun-declaration
void CodeGenerator::generateDeclaration(ASTNode* node) {
    std::cout << "Calling generateDeclaration" << std::endl;
    if (!node || node->children->empty()) return;
    
    ASTNode* declChild = node->children->at(0); // Should only have one child
    
    switch (declChild->type) {
        case ASTNodeType::VAR_DECLARATION:
            generateVarDeclaration(declChild);
            break;
        case ASTNodeType::FUN_DECLARATION:
            generateFunDeclaration(declChild);
            break;
        default:
            std::cerr << "Unexpected declaration type in Rule 3: " << getNodeTypeName(declChild->type) << std::endl;
            break;
    }
}

// Rule 4: var-declaration := ; | [ NUM ] ;
// Modified to track variables in the frame
void CodeGenerator::generateVarDeclaration(ASTNode* node) {
    std::cout << "Calling generateVarDeclaration" << std::endl;
    if (!node) return;
    
    // Get variable name and whether it's an array
    std::string varName = node->tokenValue;
    bool isArray = false;
    int arraySize = -1;
    
    // Check if this is an array declaration
    Symbol* varSymbol = symbolTable.findSymbol(varName);
    if (varSymbol && varSymbol->arrSize > 0) {
        isArray = true;
        arraySize = varSymbol->arrSize;
    }
    
    // Add variable to the frame tracking
    addVariableToFrame(varName, isArray, arraySize);
    
    // Initialize variable with 0 (optional but good practice)
    instructions.push_back(Instruction(OpCode::PUSH, "0"));
    
    // For arrays, we would create space for each element, but for simplicity
    // in this implementation we're treating them as a single variable
}

// Rule 5: type-specifier := int | void 
// EMPTY
void CodeGenerator::generateTypeSpecifier(ASTNode* node) {
    std::cout << "Calling generateTypeSpecifier" << std::endl;
    // Does not directly generate any stack machine code
}

// Rule 6: fun-declaration := ( params ) compound-stmt
void CodeGenerator::generateFunDeclaration(ASTNode* node) {
    std::cout << "Calling generateFunDeclaration" << std::endl;
    if (!node || node->children->size() < 3) return;
    
    // Clear any existing frame variables when entering a new function
    clearFrameVariables();
    
    std::string funcName = node->tokenValue;
    std::cout << "FuncName: " << funcName << std::endl;
    
    // Add function label (lowercase for consistency with stack machine)
    instructions.push_back(Instruction(OpCode::LABEL, funcName.size() > 0 ? funcName : "unknown_function"));
    
    // Process parameters
    if (node->children->size() >= 2) {
        generateParams(node->children->at(1));
    }
    
    // Process function body
    generateCompoundStmt(node->children->at(2));
    
    // Adding return instruction if not previously included
    if (instructions.back().op != OpCode::RET && instructions.back().op != OpCode::RETV) {
        instructions.push_back(Instruction(OpCode::RET));
    }
    
    // Clear frame variables after function is done
    clearFrameVariables();
}

// Rule 7: params := param-list | empty
void CodeGenerator::generateParams(ASTNode* node) {
    std::cout << "Calling generateParams" << std::endl;
    if (!node) return;
    
    // Process parameter list if there is one
    if (!node->children->empty()) {
        generateParamList(node->children->at(0));
    }
}

// Rule 8: param-list := param-list , param | param
void CodeGenerator::generateParamList(ASTNode* node) {
    std::cout << "Calling generateParamList" << std::endl;
    if (!node) return;
    
    // Process each parameter
    for (ASTNode* paramNode : *node->children) {
        generateParam(paramNode);
    }
}

// Rule 9: param := type-specifier ID | type-specifier ID [ ]
void CodeGenerator::generateParam(ASTNode* node) {
    std::cout << "Calling generateParam" << std::endl;
    if (!node) return;
    
    // Get parameter name
    std::string paramName = node->tokenValue;
    
    // Check if this is an array parameter
    bool isArray = node->children->size() > 1; // Simplified check for this example
    
    // Add parameter to frame tracking
    addVariableToFrame(paramName, isArray, isArray ? 0 : -1);
}

// Rule 10: compound-stmt := { local-declarations statement-list }
void CodeGenerator::generateCompoundStmt(ASTNode* node) {
    std::cout << "Calling generateCompoundStmt" << std::endl;
    if (!node || node->children->size() < 2) return;
    
    // Process local declarations
    generateLocalDeclarations(node->children->at(0));
    
    // Process statement list
    generateStatementList(node->children->at(1));
}

// Rule 11: local-declarations := local-declarations var-declaration | var-declaration
void CodeGenerator::generateLocalDeclarations(ASTNode* node) {
    std::cout << "Calling generateLocalDeclarations" << std::endl;
    if (!node) return;
    
    // Process all local variable declarations
    for (ASTNode* child : *node->children) {
        generateVarDeclaration(child);
    }
}

// Rule 12: statement-list := statement-list statement | statement
void CodeGenerator::generateStatementList(ASTNode* node) {
    std::cout << "Calling generateStatementList" << std::endl;
    if (!node) return;
    
    // Process each statement
    for (ASTNode* child : *node->children) {
        generateStatement(child);
    }
}

// Rule 13: statement := expression-stmt | compound-stmt | selection-stmt | iteration-stmt | return-stmt | io-stmt
void CodeGenerator::generateStatement(ASTNode* node) {
    std::cout << "Calling generateStatement" << std::endl;
    if (!node || node->children->empty()) return;
    
    ASTNode* stmtChild = node->children->at(0);
    
    switch (stmtChild->type) {
        case ASTNodeType::EXPRESSION_STMT:
            generateExpressionStmt(stmtChild);
            break;
        case ASTNodeType::COMPOUNT_STMT:
            generateCompoundStmt(stmtChild);
            break;
        case ASTNodeType::SELECTION_STMT:
            generateSelectionStmt(stmtChild);
            break;
        case ASTNodeType::ITERATION_STMT:
            generateIterationStmt(stmtChild);
            break;
        case ASTNodeType::RETURN_STMT:
            generateReturnStmt(stmtChild);
            break;
        case ASTNodeType::IO_STMT:
            generateIOStmt(stmtChild);
            break;
        default:
            std::cerr << "Unexpected statement type in Rule 13: " << getNodeTypeName(stmtChild->type) << std::endl;
            break;
    }
}

// Rule 14: io-stmt := input-stmt | output-stmt
void CodeGenerator::generateIOStmt(ASTNode* node) {
    std::cout << "Calling generateIOStmt" << std::endl;
    if (!node || node->children->empty()) return;
    
    ASTNode* ioChild = node->children->at(0);
    
    switch (ioChild->type) {
        case ASTNodeType::INPUT_STMT:
            generateInputStmt(ioChild);
            break;
        case ASTNodeType::OUTPUT_STMT:
            generateOutputStmt(ioChild);
            break;
        default:
            std::cerr << "Unexpected IO statement type: " << getNodeTypeName(ioChild->type) << std::endl;
            break;
    }
}

// Rule 15: input-stmt := input ( STRING ) ;
void CodeGenerator::generateInputStmt(ASTNode* node) {
    std::cout << "Calling generateInputStmt" << std::endl;
    if (!node || node->children->empty()) return;
    
    // Get the string prompt if available
    if (node->children->at(0)->type == ASTNodeType::FACTOR) {
        std::string prompt = node->children->at(0)->tokenValue;
        
        // Push the prompt string
        instructions.push_back(Instruction(OpCode::PRINT, prompt));
    }
    
    // Read input
    instructions.push_back(Instruction(OpCode::READ));
}

// Rule 16: output-stmt := output ( STRING ) ; | output ( expression ) ;
void CodeGenerator::generateOutputStmt(ASTNode* node) {
    std::cout << "Calling generateOutputStmt" << std::endl;
    if (!node || node->children->empty()) return;
    
    ASTNode* outExpr = node->children->at(0);
    
    if (outExpr->type == ASTNodeType::FACTOR && outExpr->tokenType == STRING) {
        // Push the string literal
        instructions.push_back(Instruction(OpCode::PRINT, outExpr->tokenValue));
    } else {
        // Generate code for the expression
        generateExpression(outExpr);
        
        // Print the result
        instructions.push_back(Instruction(OpCode::PRINT));
    }
}

// Rule 17: expression-stmt := expression ; | ;
void CodeGenerator::generateExpressionStmt(ASTNode* node) {
    std::cout << "Calling generateExpressionStmt" << std::endl;
    if (!node) return;
    
    // If there's an expression, generate code for it
    if (!node->children->empty()) {
        generateExpression(node->children->at(0));
        
        // Pop the result if it's not used (expression statement)
        instructions.push_back(Instruction(OpCode::POP));
    }
}

// Rule 18: selection-stmt := if ( simple-expression ) statement | if ( simple-expression ) statement else statement
void CodeGenerator::generateSelectionStmt(ASTNode* node) {
    std::cout << "Calling generateSelectionStmt" << std::endl;
    if (!node || node->children->size() < 2) return;
    
    // Generate code for the condition
    generateSimpleExpression(node->children->at(0));
    
    std::string elseLabel = generateLabel();
    std::string endLabel = generateLabel();
    
    // Jump to else part if condition is false
    instructions.push_back(Instruction(OpCode::BRZ, elseLabel));
    
    // Generate code for the 'then' part
    generateStatement(node->children->at(1));
    
    // Jump to end after 'then' part
    instructions.push_back(Instruction(OpCode::JUMP, endLabel));
    
    // Else part
    instructions.push_back(Instruction(OpCode::LABEL, elseLabel));
    
    // If there's an 'else' clause
    if (node->children->size() >= 3) {
        generateStatement(node->children->at(2));
    }
    
    // End of if-else
    instructions.push_back(Instruction(OpCode::LABEL, endLabel));
}

// Rule 19: iteration-stmt := while ( simple-expression ) statement
void CodeGenerator::generateIterationStmt(ASTNode* node) {
    std::cout << "Calling generateIterationStmt" << std::endl;
    if (!node || node->children->size() < 2) return;
    
    std::string startLabel = generateLabel();
    std::string endLabel = generateLabel();
    
    // Save the break and continue labels
    breakLabels.push(endLabel);
    continueLabels.push(startLabel);
    
    // Start of loop
    instructions.push_back(Instruction(OpCode::LABEL, startLabel));
    
    // Generate code for the condition
    generateExpression(node->children->at(0));
    
    // Jump to end if condition is false
    instructions.push_back(Instruction(OpCode::BRZ, endLabel));
    
    // Generate code for the loop body
    generateStatement(node->children->at(1));
    
    // Jump back to start
    instructions.push_back(Instruction(OpCode::JUMP, startLabel));
    
    // End of loop
    instructions.push_back(Instruction(OpCode::LABEL, endLabel));
    
    // Restore labels
    breakLabels.pop();
    continueLabels.pop();
}

// Rule 20: return-stmt := return ; | return expression ;
void CodeGenerator::generateReturnStmt(ASTNode* node) {
    std::cout << "Calling generateReturnStmt" << std::endl;
    if (!node) return;
    
    // If there's a return value, generate code for it
    if (!node->children->empty()) {
        generateExpression(node->children->at(0));
        instructions.push_back(Instruction(OpCode::RETV));
    } else {
        // Return without a value
        instructions.push_back(Instruction(OpCode::RET));
    }
}

// Rule 21: expression := var = simple-expression | simple-expression
void CodeGenerator::generateExpression(ASTNode* node) {
    std::cout << "Calling generateExpression" << std::endl;
    if (!node || node->children->empty()) return;
    
    // Check if it's an assignment or a simple expression
    if (node->children->size() >= 2 && node->children->at(0)->type == ASTNodeType::VAR) {
        // Assignment
        ASTNode* varNode = node->children->at(0);
        ASTNode* exprNode = node->children->at(1);
        
        // Generate code for the right-hand side expression
        generateSimpleExpression(exprNode);
        
        // Store the result in the variable
        generateVar(varNode, true);  // true indicates store operation
    } else if (node->children->size() == 1) {
        // Simple expression
        generateSimpleExpression(node->children->at(0));
    }
}

// Rule 22: var := ID | ID [ expression ]
// MODIFIED to use stack offsets
void CodeGenerator::generateVar(ASTNode* node, bool isStore) {
    std::cout << "Calling generateVar" << std::endl;
    if (!node) return;
    
    std::string varName = node->tokenValue;
    int varOffset = getVariableOffset(varName);
    
    if (varOffset == -1) {
        std::cerr << "Error: Variable '" << varName << "' not found in frame" << std::endl;
        return;
    }
    
    if (node->children->empty()) {
        // Simple variable (not array)
        if (isStore) {
            // Store operation - value is already on the stack, so push the offset
            instructions.push_back(Instruction(OpCode::PUSH, std::to_string(varOffset)));
            instructions.push_back(Instruction(OpCode::STORE));
        } else {
            // Load operation - push the offset, then load
            instructions.push_back(Instruction(OpCode::PUSH, std::to_string(varOffset)));
            instructions.push_back(Instruction(OpCode::LOAD));
        }
    } else {
        // Array variable with index
        // First check if this variable exists and is an array
        auto it = frameVariables.find(varName);
        if (it == frameVariables.end() || !it->second.isArray) {
            std::cerr << "Error: Variable '" << varName << "' is not a valid array" << std::endl;
            return;
        }
        
        // Generate code for the index expression
        generateSimpleExpression(node->children->at(0));
        
        // Add base offset of the array
        instructions.push_back(Instruction(OpCode::PUSH, std::to_string(varOffset)));
        instructions.push_back(Instruction(OpCode::ADD));
        
        if (isStore) {
            // For store, we have [value, index+offset] on stack
            // Need to swap and store
            // TODO: Implement array indexing properly
            instructions.push_back(Instruction(OpCode::STORE));
        } else {
            // For load, we have [index+offset] on stack
            instructions.push_back(Instruction(OpCode::LOAD));
        }
    }
}

// Rule 23: simple-expression := additive-expression relop additive-expression | additive-expression
void CodeGenerator::generateSimpleExpression(ASTNode* node) {
    std::cout << "Calling generateSimpleExpression" << std::endl;
    if (!node || node->children->empty()) return;
    
    // Generate code for the first additive expression
    generateAdditiveExpression(node->children->at(0));
    
    // If there's a relational operator
    if (node->children->size() >= 3) {
        ASTNode* relopNode = node->children->at(1);
        ASTNode* rightExpr = node->children->at(2);
        
        // Generate code for the second additive expression
        generateAdditiveExpression(rightExpr);
        
        // Apply the relational operator
        std::string relOp = relopNode->tokenValue;
        
        if (relOp == "<=" || relOp == "LE") {
            instructions.push_back(Instruction(OpCode::LE));
        } else if (relOp == "<" || relOp == "LT") {
            instructions.push_back(Instruction(OpCode::LT));
        } else if (relOp == ">" || relOp == "GT") {
            instructions.push_back(Instruction(OpCode::GT));
        } else if (relOp == ">=" || relOp == "GE") {
            instructions.push_back(Instruction(OpCode::GE));
        } else if (relOp == "==" || relOp == "EE") {
            instructions.push_back(Instruction(OpCode::EQ));
        } else if (relOp == "!=" || relOp == "NE") {
            instructions.push_back(Instruction(OpCode::NE));
        }
    }
}

// Rule 24: relop := <= | < | > | >= | == | !=
// Implementation moved to generateSimpleExpression for simplicity

// Rule 25 & 26: additive-expression := term | additive-expression + term | additive-expression - term
void CodeGenerator::generateAdditiveExpression(ASTNode* node) {
    std::cout << "Calling generateAdditiveExpression" << std::endl;
    if (!node || node->children->empty()) return;
    
    // Generate code for the first term
    generateTerm(node->children->at(0));
    
    // Process additional terms with additive operators
    for (size_t i = 1; i < node->children->size(); i += 2) {
        if (i + 1 < node->children->size()) {
            ASTNode* opNode = node->children->at(i);
            ASTNode* termNode = node->children->at(i + 1);
            
            // Generate code for the term
            generateTerm(termNode);
            
            // Apply the additive operator
            std::string addOp = opNode->tokenValue;
            std::cout << "AddOp TOKEN: " << addOp << std::endl;
            
            if (addOp == "PLUS" || addOp == "+") {
                instructions.push_back(Instruction(OpCode::ADD));
            } else if (addOp == "MINUS" || addOp == "-") {
                instructions.push_back(Instruction(OpCode::SUB));
            }
        }
    }
}

// Rule 27 & 28: term := factor | term * factor | term / factor
void CodeGenerator::generateTerm(ASTNode* node) {
    std::cout << "Calling generateTerm" << std::endl;
    if (!node || node->children->empty()) return;
    
    // Generate code for the first factor
    generateFactor(node->children->at(0));
    
    // Process additional factors with multiplicative operators
    for (size_t i = 1; i < node->children->size(); i += 2) {
        if (i + 1 < node->children->size()) {
            ASTNode* opNode = node->children->at(i);
            ASTNode* factorNode = node->children->at(i + 1);
            
            // Generate code for the factor
            generateFactor(factorNode);
            
            // Apply the multiplicative operator
            std::string mulOp = opNode->tokenValue;
            std::cout << "mulOP Token: " << mulOp << std::endl;
            
            if (mulOp == "TIMES" || mulOp == "*") {
                instructions.push_back(Instruction(OpCode::MUL));
            } else if (mulOp == "DIVIDE" || mulOp == "/") {
                instructions.push_back(Instruction(OpCode::DIV));
            }           
        }
    }
}

// Rule 29: factor := ( simple-expression ) | var | call | input-stmt | NUM
void CodeGenerator::generateFactor(ASTNode* node) {
    std::cout << "Calling generateFactor" << std::endl;
    if (!node) return;
    
    if (node->children->empty() && node->tokenType == NUM) {
        // Number literal
        instructions.push_back(Instruction(OpCode::PUSH, std::to_string(node->tokenIntValue)));
    } else if (!node->children->empty()) {
        ASTNode* child = node->children->at(0);
        
        switch (child->type) {
            case ASTNodeType::SIMPLE_EXPRESSION:  // For parenthesized expressions
                generateSimpleExpression(child);
                break;
            case ASTNodeType::VAR:
                generateVar(child);
                break;
            case ASTNodeType::CALL:
                generateCall(child);
                break;
            case ASTNodeType::INPUT_STMT:
                generateInputStmt(child);
                break;
            default:
                if (child->tokenType == NUM) {
                    // Number literal inside a FACTOR
                    instructions.push_back(Instruction(OpCode::PUSH, std::to_string(child->tokenIntValue)));
                } else {
                    std::cerr << "Unexpected factor type in Rule 29: " << getNodeTypeName(child->type) << std::endl;
                }
                break;
        }
    } else if (node->type == ASTNodeType::VAR) {
        // Variable reference
        generateVar(node);
    } else if (node->type == ASTNodeType::CALL) {
        // Function call
        generateCall(node);
    } else if (node->type == ASTNodeType::INPUT_STMT) {
        // Input statement as expression
        generateInputStmt(node);
    } else {
        std::cerr << "Unexpected factor in Rule 29: " << getNodeTypeName(node->type) << std::endl;
    }
}

// Rule 30: call := ID ( args ) | ID ( )
void CodeGenerator::generateCall(ASTNode* node) {
    std::cout << "Calling generateCall" << std::endl;
    if (!node || node->children->empty()) return;
    
    // Getting function name and arguments
    std::string funcName = node->tokenValue;
    ASTNode* argsNode = node->children->at(0);
    
    // Count the number of arguments
    int numArgs = 0;
    if (!argsNode->children->empty()) {
        ASTNode* argListNode = argsNode->children->at(0);
        numArgs = argListNode->children->size();
        
        // Push arguments in reverse order (right to left)
        for (int i = argListNode->children->size() - 1; i >= 0; i--) {
            generateExpression(argListNode->children->at(i));
        }
    }
    
    // Push the number of arguments
    instructions.push_back(Instruction(OpCode::PUSH, std::to_string(numArgs)));
    
    // Call the function
    instructions.push_back(Instruction(OpCode::CALL, funcName));
}

// Rule 31: args := arg-list | empty
void CodeGenerator::generateArgs(ASTNode* node) {
    std::cout << "Calling generateArgs" << std::endl;
    // Arguments handled in generateCall
}

// Rule 32: arg-list := arg-list , expression | expression
void CodeGenerator::generateArgList(ASTNode* node) {
    std::cout << "Calling generateArgList" << std::endl;
    // Arguments handled in generateCall
}

// Return the generated instructions
std::vector<Instruction> CodeGenerator::getInstructions() const {
    std::cout << "Calling getInstructions" << std::endl;
    return instructions;
}

// Convert instructions to strings
std::vector<std::string> CodeGenerator::getCode() const {
    std::cout << "Calling getCode" << std::endl;
    std::vector<std::string> code;
    
    for (const auto& instr : instructions) {
        std::ostringstream oss;
        
        if (instr.op == OpCode::LABEL) {
            oss << instr.arg; // Just the name of the label
        } else {
            oss << getOpString(instr.op); // Instruction
            
            if (!instr.arg.empty()) {
                oss << "("; // Open parentheses
                
                // Lambda function to determine if instr.arg can be converted to an int
                bool canBeInt = [] (const std::string& s) { 
                    try { 
                        size_t p; 
                        std::stoi(s, &p); 
                        return p == s.size(); 
                    } catch (...) { 
                        return false; 
                    } 
                } (instr.arg);
                
                // Adding quotations marks if necessary 
                if (instr.op == OpCode::PRINT || instr.op == OpCode::BRZ || instr.op == OpCode::BRT || instr.op == OpCode::CALL || instr.op == OpCode::JUMP) {
                    if (!canBeInt) {
                        oss << "\"" << instr.arg << "\""; // Add quotes around the string argument
                    } else {
                        oss << instr.arg; // Argument w/o quotes (int) or empty string (no arg)
                    }
                } else {
                    oss << instr.arg; // Argument w/o quotes (int) or empty string (no arg)
                }
                
                oss << ")"; // Close parentheses
            } else {
                oss << "()"; // Empty parentheses for instructions with no arguments
            }
        }
        
        // Writing to the code vector
        code.push_back(oss.str());
    }
    
    return code;
}

// Writes stack machine code to file
void CodeGenerator::printStackMachineCodeToFile(std::string filename, std::vector<std::string> code) {
    std::cout << "Calling printStackMachineCodeToFile" << std::endl;
    // Writing to file
    std::ofstream file(filename);
    for (std::string s : code) file << s << std::endl;
    file.close();
}