#include "codeGenerator.h"
#include <iostream>
#include <sstream>
#include <fstream>

// Code generator constructor
CodeGenerator::CodeGenerator(SymbolTable& st) : 
    symbolTable(st), 
    labelCounter(0), 
    localVarCount(0) {}

// Generating labels
std::string CodeGenerator::generateLabel() {
    return "L" + std::to_string(labelCounter++);
}

// Returns text of Stack Machine code corresponding to the opcode
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
        case OpCode::JUMP: return "JUMP";
        case OpCode::CALL: return "CALL";
        case OpCode::RET: return "RET";
        case OpCode::RETV: return "RETV";
        case OpCode::PRINT: return "PRINT";
        case OpCode::READ: return "READ";
        case OpCode::READF: return "READF";
        case OpCode::INT: return "INT";
        case OpCode::FLOAT: return "FLOAT";
        case OpCode::END: return "END";
        default: return "UNKNOWN"; // Should never run
    }
}

// Tracking variables in stack frame
// Used for tracking addresses for load, save, and store calls
void CodeGenerator::addVariableToFrame(const std::string& varName, bool isArray, int arraySize, bool isFloat) {
    if (frameVariables.find(varName) == frameVariables.end()) {
        VariableInfo info;
        info.stackOffset = localVarCount;
        info.isArray = isArray;
        info.arraySize = arraySize;
        info.isFloat = isFloat;
        
        // Store the parameter name and its information
        frameVariables[varName] = info;
        
        // Debug output - add this temporarily for debugging
        // std::cout << "Added variable '" << varName << "' to frame. isArray: " 
        //           << (isArray ? "true" : "false") << ", arraySize: " << arraySize 
        //           << ", offset: " << info.stackOffset << std::endl;
        
        // Increment localVarCount based on variable size
        if (isArray) {
            localVarCount += arraySize; // Each array element gets its own offset
        } else {
            localVarCount += 1; // Scalar variable only needs one slot
        }
    }
}

// Helper function to get a variable's offset in the current frame
int CodeGenerator::getVariableOffset(const std::string& varName) {
    auto it = frameVariables.find(varName);
    if (it != frameVariables.end()) {
        return it->second.stackOffset;
    }
    
    std::cerr << "Warning: Variable '" << varName << "' not found in frame" << std::endl;
    // Print all variables in the frame for debugging
    std::cerr << "Current frame variables:" << std::endl;
    for (const auto& pair : frameVariables) {
        std::cerr << "  " << pair.first << " (isArray: " << (pair.second.isArray ? "true" : "false") 
                  << ", offset: " << pair.second.stackOffset << ")" << std::endl;
    }
    
    return -1;
}

// Helper function to check if a variable is a float type
bool CodeGenerator::isVariableFloat(const std::string& varName) {
    if (frameVariables.find(varName) != frameVariables.end()) {
        return frameVariables[varName].isFloat;
    }
    
    std::cerr << "Warning: Variable '" << varName << "' not found in frame" << std::endl;
    return false; // Default to int
}

// Clear all variables at the end of a function
void CodeGenerator::clearFrameVariables() {
    frameVariables.clear();
    localVarCount = 0;
}

// Entry point for code generation
void CodeGenerator::generate(ASTNode* root) {
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
    if (!node) return;
    
    // Process each declaration
    for (ASTNode* child : *node->children) {
        generateDeclaration(child);
    }
}

// Rule 3: declaration := var-declaration | fun-declaration
void CodeGenerator::generateDeclaration(ASTNode* node) {
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
void CodeGenerator::generateVarDeclaration(ASTNode* node) {
    if (!node) return;
    
    // Get variable name from token
    std::string varName = node->tokenValue;
    
    // Check if this is an array declaration
    Symbol* varSymbol = symbolTable.findSymbol(varName);
    if (!varSymbol) {
        std::cerr << "Error: Symbol '" << varName << "' not found in symbol table" << std::endl;
        
        // Print out the contents of the symbol table to standard out for debugging
        std::cerr << "Symbol table contents:" << std::endl;
        symbolTable.print();
        
        return;
    }
    
    bool isArray = (varSymbol->arrSize > 0);
    int arraySize = varSymbol->arrSize;
    bool isFloat = (varSymbol->dataType == "float");
    
    // Debug output - after symbol lookup
    // std::cout << "Found symbol '" << varName << "', isArray: " 
    //           << (isArray ? "true" : "false") << ", arraySize: " << arraySize 
    //           << ", isFloat: " << (isFloat ? "true" : "false") << std::endl;
    
    // Add variable to the frame tracking
    addVariableToFrame(varName, isArray, arraySize, isFloat);
    
    // Initialize variable(s)
    if (isArray && arraySize > 0) {
        // For arrays, initialize each element with 0
        for (int i = 0; i < arraySize; i++) {
            instructions.push_back(Instruction(OpCode::PUSH, "0"));
            if (isFloat) {
                instructions.push_back(Instruction(OpCode::FLOAT));
            }
            
            // Calculate the array element offset
            int elementOffset = getVariableOffset(varName) + i;
            instructions.push_back(Instruction(OpCode::PUSH, std::to_string(elementOffset)));
            instructions.push_back(Instruction(OpCode::STORE));
        }
    } else {
        // For scalar variables, initialize with 0
        instructions.push_back(Instruction(OpCode::PUSH, "0"));
        if (isFloat) {
            instructions.push_back(Instruction(OpCode::FLOAT));
        }
        
        // Store the initialization value
        instructions.push_back(Instruction(OpCode::PUSH, std::to_string(getVariableOffset(varName))));
        instructions.push_back(Instruction(OpCode::STORE));
    }
}

// Rule 5: type-specifier := int | void | float
// EMPTY
void CodeGenerator::generateTypeSpecifier(ASTNode* node) {
    // Does not directly generate any stack machine code
}

// Rule 6: fun-declaration := ( params ) compound-stmt
void CodeGenerator::generateFunDeclaration(ASTNode* node) {
    if (!node || node->children->size() < 3) return;
    
    // Clear any existing frame variables when entering a new function
    clearFrameVariables();

    symbolTable.enterScope(); // Enter a new scope for the function
    
    std::string funcName = node->tokenValue;
    
    // Add function label (lowercase for consistency with stack machine)
    instructions.push_back(Instruction(OpCode::LABEL, funcName.size() > 0 ? funcName : "unknown_function"));
    
    // Process parameters
    if (node->children->size() >= 2) {
        generateParams(node->children->at(1));
    }
    
    // Process function body
    generateCompoundStmt(node->children->at(2));
    
    // Adding return instruction if not previously included
    if (funcName != "main" && instructions.back().op != OpCode::RET && instructions.back().op != OpCode::RETV) {
        instructions.push_back(Instruction(OpCode::RET));
    }

    symbolTable.exitScope(); // Exit the function scope
    
    // Clear frame variables after function is done
    clearFrameVariables();
}

// Rule 7: params := param-list | empty
void CodeGenerator::generateParams(ASTNode* node) {
    if (!node) return;
    
    // Process parameter list if there is one
    if (!node->children->empty()) {
        generateParamList(node->children->at(0));
    }
}

// Rule 8: param-list := param-list , param | param
void CodeGenerator::generateParamList(ASTNode* node) {
    if (!node) return;
    
    // Process each parameter
    for (ASTNode* paramNode : *node->children) {
        generateParam(paramNode);
    }
}

// Rule 9: param := type-specifier ID | type-specifier ID [ ]
void CodeGenerator::generateParam(ASTNode* node) {
    if (!node) return;
    
    // Get parameter name
    std::string paramName = node->tokenValue;
    
    // Check if this is an array parameter
    bool isArray = false;
    for (ASTNode* child : *node->children) {
        if (child->type == ASTNodeType::TYPE_SPECIFIER) {
            isArray = node->children->size() > 1; // Simple check
        }
    }

    // Check if this is a float parameter
    bool isFloat = node->isFloat;
    bool isFLoat = isVariableFloat(paramName);
    
    // Add parameter to frame tracking
    addVariableToFrame(paramName, isArray, isArray ? 0 : -1, isFloat);
}

// Rule 10: compound-stmt := { local-declarations statement-list }
void CodeGenerator::generateCompoundStmt(ASTNode* node) {
    if (!node || node->children->size() < 2) return;
    
    // Process local declarations
    generateLocalDeclarations(node->children->at(0));
    
    // Process statement list
    generateStatementList(node->children->at(1));
}

// Rule 11: local-declarations := local-declarations var-declaration | var-declaration
void CodeGenerator::generateLocalDeclarations(ASTNode* node) {
    if (!node) return;
    
    // Process all local variable declarations
    for (ASTNode* child : *node->children) {
        generateVarDeclaration(child);
    }
}

// Rule 12: statement-list := statement-list statement | statement
void CodeGenerator::generateStatementList(ASTNode* node) {
    if (!node) return;
    
    // Process each statement
    for (ASTNode* child : *node->children) {
        generateStatement(child);
    }
}

// Rule 13: statement := expression-stmt | compound-stmt | selection-stmt | iteration-stmt | return-stmt | io-stmt
void CodeGenerator::generateStatement(ASTNode* node) {
    if (!node || node->children->empty()) return;
    
    ASTNode* stmtChild = node->children->at(0);
    
    // Process each type of statement
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
    if (!node || node->children->empty()) return;
    
    ASTNode* ioChild = node->children->at(0);
    
    // Process each type of IO statement
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
    if (!node || node->children->empty()) return;
    
    // Get the string prompt if available
    if (node->children->at(0)->type == ASTNodeType::FACTOR) {
        std::string prompt = node->children->at(0)->tokenValue;
        
        // Push the prompt string
        instructions.push_back(Instruction(OpCode::PRINT, prompt)); 
    }
    
    // Check if reading into float variable
    Symbol* varSymbol = nullptr;
    if (node->children->size() > 1 && node->children->at(1)->type == ASTNodeType::VAR) {
        std::string varName = node->children->at(1)->tokenValue;
        varSymbol = symbolTable.findSymbol(varName);
    }
    
    // Use appropriate read instruction based on variable type
    if (varSymbol && varSymbol->dataType == "float") {
        instructions.push_back(Instruction(OpCode::READF)); // Read float
    } else {
        instructions.push_back(Instruction(OpCode::READ)); // Read int
    }
}

// Rule 16: output-stmt := output ( STRING ) ; | output ( expression ) ;
void CodeGenerator::generateOutputStmt(ASTNode* node) {
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
        // instructions.push_back(Instruction(OpCode::POP)); // Remove value just added to stack
    }
}

// Rule 17: expression-stmt := expression ; | ;
void CodeGenerator::generateExpressionStmt(ASTNode* node) {
    if (!node) return;
    
    // If there's an expression, generate code for it
    if (!node->children->empty()) {
        generateExpression(node->children->at(0));
    }
}

// Rule 18: selection-stmt := if ( simple-expression ) statement | if ( simple-expression ) statement else statement
void CodeGenerator::generateSelectionStmt(ASTNode* node) {
    if (!node || node->children->size() < 2) return;
    
    // Generate code for the condition
    generateSimpleExpression(node->children->at(0));
    
    // Generating labels for branching
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
    if (!node || node->children->size() < 2) return;
    
    // Generating labels for loop control
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
    if (!node) return;
    
    // If there's a return value, generate code for it
    if (!node->children->empty()) {
        generateExpression(node->children->at(0));
        
        // MISSING: Handling type conversion if needed
        // Will be handled if function returns to a variable
        // Only leads to incorrect results in case of output(function());
        
        instructions.push_back(Instruction(OpCode::RETV));
    } else {
        // Return without a value
        instructions.push_back(Instruction(OpCode::RET));
    }
}

// Rule 21: expression := var = array-init-expression | var = simple-expression | simple-expression
void CodeGenerator::generateExpression(ASTNode* node) {
    if (!node || node->children->empty()) return;
    
    // Check if it's an assignment or a simple expression
    if (node->children->size() >= 2 && node->children->at(0)->type == ASTNodeType::VAR) {
        // Assignment
        ASTNode* varNode = node->children->at(0);
        ASTNode* exprNode = node->children->at(1);
        
        std::string varName = varNode->tokenValue;
        bool isVarFloat = isVariableFloat(varName);
        
        // Check if this is an array initialization
        if (exprNode->type == ASTNodeType::ARRAY_INIT_EXPRESSION) {
            // Generate array initialization code
            generateArrayInitExpression(exprNode, varName);
        }
        // Check if this is an array operation
        else if (exprNode->type == ASTNodeType::ARRAY_OPERATION) {
            // Generate array operation code (with assignment flag set to true)
            generateArrayOperation(exprNode, varNode);
        
        }
        else {
            // Generate code for regular assignment
            generateSimpleExpression(exprNode);
            
            // Handle type conversion if needed
            if (isVarFloat) {
                // If variable is float but expression might be int, convert to float
                instructions.push_back(Instruction(OpCode::FLOAT));
            } else {
                // If variable is int but expression might be float, convert to int
                instructions.push_back(Instruction(OpCode::INT));
            }
            
            // Store the result in the variable
            generateVar(varNode, true);  // true indicates store operation
        }
    } else if (node->children->size() == 1) {
        // Simple expression
        ASTNode* childNode = node->children->at(0);
        
        // Check if this is an array operation not part of an assignment
        if (childNode->type == ASTNodeType::ARRAY_OPERATION) {
            std::cerr << "Warning: Array operation without assignment" << std::endl;
            generateArrayOperation(childNode, nullptr); // No assignment, so pass nullptr
        } else {
            generateSimpleExpression(childNode);
        }
    }
}

// Rule 22: var := ID | ID [ expression ]
void CodeGenerator::generateVar(ASTNode* node, bool isStore) {
    if (!node) return;
    
    std::string varName = node->tokenValue;
    
    // Check if this variable exists in the frame
    auto it = frameVariables.find(varName);
    if (it == frameVariables.end()) {
        std::cerr << "Error: Variable '" << varName << "' not found in frame" << std::endl;
        return;
    }
    
    int varOffset = it->second.stackOffset;
    bool isArray = it->second.isArray;
    
    // Check if this is an array access
    if (!node->children->empty() && node->children->at(0)->type == ASTNodeType::SIMPLE_EXPRESSION) {
        // Check if this is actually an array
        if (!isArray) {
            std::cerr << "Error: Variable '" << varName << "' is not an array but is accessed as one" << std::endl;
            return;
        }
        
        // Generate code for the index expression
        generateSimpleExpression(node->children->at(0));
        
        // Convert index to int if needed
        instructions.push_back(Instruction(OpCode::INT));
        
        // Add base offset of the array
        instructions.push_back(Instruction(OpCode::PUSH, std::to_string(varOffset)));
        instructions.push_back(Instruction(OpCode::ADD));
        
        if (isStore) {
            // For store, we use the calculated offset
            instructions.push_back(Instruction(OpCode::STORE));
        } else {
            // For load, we load from the calculated offset
            instructions.push_back(Instruction(OpCode::LOAD));
        }
    } else {
        // Simple variable access (not array)
        if (isArray) {
            // If this is an array but accessed without an index, use the base address
            std::cerr << "Warning: Array variable '" << varName << "' accessed without index" << std::endl;
        }
        
        if (isStore) {
            // Store operation - value is already on stack
            instructions.push_back(Instruction(OpCode::PUSH, std::to_string(varOffset)));
            instructions.push_back(Instruction(OpCode::STORE));
        } else {
            // Load operation
            instructions.push_back(Instruction(OpCode::PUSH, std::to_string(varOffset)));
            instructions.push_back(Instruction(OpCode::LOAD));
        }
    }
}

// Rule 23: simple-expression := additive-expression relop additive-expression | additive-expression
void CodeGenerator::generateSimpleExpression(ASTNode* node) {
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
            
            // Apply the multiplication operator
            std::string mulOp = opNode->tokenValue;
            
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
    if (!node) return;
    
    if (node->children->empty()) {
        if (node->tokenType == NUM) {
            // Number literal - push as int
            instructions.push_back(Instruction(OpCode::PUSH, std::to_string(node->tokenIntValue)));
        } else if (node->tokenType == FLOAT_VAL) {
            // Float literal - push as float
            instructions.push_back(Instruction(OpCode::PUSH, node->tokenValue));
        }
    } else if (!node->children->empty()) {
        ASTNode* child = node->children->at(0);
        
        // Generating appropriate expression
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
                } else if (child->tokenType == FLOAT_VAL) {
                    // Float literal inside a FACTOR
                    instructions.push_back(Instruction(OpCode::PUSH, child->tokenValue));
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
    if (!node || node->children->empty()) return;
    
    // Getting function name and arguments
    std::string funcName = node->tokenValue;
    ASTNode* argsNode = node->children->at(0);
    
    // Count the number of arguments
    int numArgs = 0;
    if (!argsNode->children->empty()) {
        ASTNode* argListNode = argsNode->children->at(0);
        numArgs = argListNode->children->size();
        
        // Push arguments in normal order (left to right)
        for (int i = 0; i < numArgs; i++) {
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
    // Arguments handled in generateCall
}

// Rule 32: arg-list := arg-list , expression | expression
void CodeGenerator::generateArgList(ASTNode* node) {
    // Arguments handled in generateCall
}

// Rule 33: array-init-expression := { array-elements }
void CodeGenerator::generateArrayInitExpression(ASTNode* node, const std::string& arrayName) {
    if (!node || node->children->empty()) return;
    
    // Find array in frame variables
    auto it = frameVariables.find(arrayName);
    if (it == frameVariables.end() || !it->second.isArray) {
        std::cerr << "Error: Cannot initialize non-array variable '" << arrayName << "'" << std::endl;
        return;
    }
    
    int baseOffset = it->second.stackOffset;
    int arraySize = it->second.arraySize;
    bool isFloat = it->second.isFloat;
    
    // Get array elements node
    ASTNode* elementsNode = node->children->at(0);
    if (!elementsNode || elementsNode->children->empty()) {
        // Empty initialization - set all to 0
        for (int i = 0; i < arraySize; i++) {
            instructions.push_back(Instruction(OpCode::PUSH, "0"));
            if (isFloat) {
                instructions.push_back(Instruction(OpCode::FLOAT));
            }
            instructions.push_back(Instruction(OpCode::PUSH, std::to_string(baseOffset + i)));
            instructions.push_back(Instruction(OpCode::STORE));
        }
        return;
    }
    
    // Initialize array elements from the provided values
    size_t elemCount = elementsNode->children->size();
    size_t initCount = std::min(static_cast<size_t>(arraySize), elemCount);
    
    // Initialize with provided values
    for (size_t i = 0; i < initCount; i++) {
        generateExpression(elementsNode->children->at(i));
        if (isFloat) {
            instructions.push_back(Instruction(OpCode::FLOAT));
        } else {
            instructions.push_back(Instruction(OpCode::INT));
        }
        instructions.push_back(Instruction(OpCode::PUSH, std::to_string(baseOffset + i)));
        instructions.push_back(Instruction(OpCode::STORE));
    }
    
    // Initialize remaining elements with 0
    for (size_t i = initCount; i < static_cast<size_t>(arraySize); i++) {
        instructions.push_back(Instruction(OpCode::PUSH, "0"));
        if (isFloat) {
            instructions.push_back(Instruction(OpCode::FLOAT));
        }
        instructions.push_back(Instruction(OpCode::PUSH, std::to_string(baseOffset + i)));
        instructions.push_back(Instruction(OpCode::STORE));
    }
    
    // Check if we have too many initializers
    if (elemCount > static_cast<size_t>(arraySize)) {
        std::cerr << "Warning: More initializers than array size for '" << arrayName << "'" << std::endl;
    }
}

// Rule 34: array-elements := array-elements , expression | expression
void CodeGenerator::generateArrayElements(ASTNode* node, const std::string& arrayName, int baseOffset) {
    if (!node) return;
    
    auto it = frameVariables.find(arrayName);
    if (it == frameVariables.end()) return;
    
    int arraySize = it->second.arraySize;
    bool isFloat = it->second.isFloat;
    
    // Check if we have too many elements
    if (node->children->size() > static_cast<size_t>(arraySize)) {
        std::cerr << "Warning: More initializers than array size for '" << arrayName << "'" << std::endl;
    }
    
    // Initialize each array element
    for (size_t i = 0; i < node->children->size() && i < static_cast<size_t>(arraySize); i++) {
        // Generate the expression value
        generateExpression(node->children->at(i));
        
        // Convert type if needed
        if (isFloat) {
            instructions.push_back(Instruction(OpCode::FLOAT));
        } else {
            instructions.push_back(Instruction(OpCode::INT));
        }
        
        // Calculate the array index offset
        instructions.push_back(Instruction(OpCode::PUSH, std::to_string(baseOffset + i)));
        
        // Store the value in the array
        instructions.push_back(Instruction(OpCode::STORE));
    }
    
    // Initialize remaining elements with 0 if needed
    for (size_t i = node->children->size(); i < static_cast<size_t>(arraySize); i++) {
        instructions.push_back(Instruction(OpCode::PUSH, "0"));
        
        // Convert to float if needed
        if (isFloat) {
            instructions.push_back(Instruction(OpCode::FLOAT));
        }
        
        // Calculate the array index offset
        instructions.push_back(Instruction(OpCode::PUSH, std::to_string(baseOffset + i)));
        
        // Store the value in the array
        instructions.push_back(Instruction(OpCode::STORE));
    }
}

// Rule 35: array-operation := var array-op expression
void CodeGenerator::generateArrayOperation(ASTNode* node, ASTNode* varNode) {
    if (!node || node->children->size() < 3) {
        std::cerr << "Error: Invalid array operation structure" << std::endl;
        return;
    }

    // Get the nodes for the operation components
    ASTNode* rightArrayNode = node->children->at(0);  // Left array
    ASTNode* opNode = node->children->at(1);         // Operator
    ASTNode* rightExprNode = node->children->at(2);  // Right expression
   
    // Get info about the  array
    std::string arrayName = rightArrayNode->tokenValue;
    auto it = frameVariables.find(arrayName);
    if (it == frameVariables.end() || !it->second.isArray) {
        std::cerr << "Error: Cannot perform array operation on non-array variable '" << arrayName << "'" << std::endl;
        return;
    }

    int baseOffset = it->second.stackOffset;
    int arraySize = it->second.arraySize;
    bool isFloat = it->second.isFloat;

    // Get info about right array
    std::string leftArrayName = varNode->tokenValue;
    auto it2 = frameVariables.find(leftArrayName);
    if (it2 == frameVariables.end() || !it2->second.isArray) {
        std::cerr << "Error: Cannot perform array operation on non-array variable '" << leftArrayName << "'" << std::endl;
        return;
    }
    
    int leftBaseOffset = it2->second.stackOffset;
    int leftArraySize = it2->second.arraySize;
    bool leftIsFloat = it2->second.isFloat;
    
    // Generate code for the scalar expression (right side)
    generateSimpleExpression(rightExprNode);
    
    // Store the scalar value in a temporary location for reuse
    int tempLocation = localVarCount++;  // Allocate a temporary location
    instructions.push_back(Instruction(OpCode::PUSH, std::to_string(tempLocation)));
    instructions.push_back(Instruction(OpCode::STORE));
    
    // Get operator type
    TokenType opType = opNode->tokenType;
    
    // Process each array element
    for (int i = 0; i < arraySize; i++) {
        // Load the current array element
        instructions.push_back(Instruction(OpCode::PUSH, std::to_string(baseOffset + i)));
        instructions.push_back(Instruction(OpCode::LOAD));
        
        // Load the scalar value
        instructions.push_back(Instruction(OpCode::PUSH, std::to_string(tempLocation)));
        instructions.push_back(Instruction(OpCode::LOAD));
        
        // Apply the operation
        switch (opType) {
            case TokenType::PLUS:
                instructions.push_back(Instruction(OpCode::ADD));
                break;
            case TokenType::MINUS:
                instructions.push_back(Instruction(OpCode::SUB));
                break;
            case TokenType::TIMES:
                instructions.push_back(Instruction(OpCode::MUL));
                break;
            case TokenType::DIVIDE:
                instructions.push_back(Instruction(OpCode::DIV));
                break;
            case TokenType::MOD:
                instructions.push_back(Instruction(OpCode::REM));
                break;
            default:
                std::cerr << "Error: Unknown array operation" << std::endl;
                break;
        }
        
        // Convert to the appropriate type if needed
        if (isFloat) {
            instructions.push_back(Instruction(OpCode::FLOAT));
        } else {
            instructions.push_back(Instruction(OpCode::INT));
        }
        
        // Store the result back in the array
        instructions.push_back(Instruction(OpCode::PUSH, std::to_string(leftBaseOffset + i)));
        instructions.push_back(Instruction(OpCode::STORE));
    }
    
    // Free the temporary location by decrementing localVarCount
    localVarCount--;
}

// Rule 36: array-op := + | - | * | /
// Empty function
void CodeGenerator::generateArrayOp(ASTNode* node) {
    // Handled in rule 35
}

// Return the generated instructions
std::vector<Instruction> CodeGenerator::getInstructions() const {
    return instructions;
}

// Convert instructions to strings
std::vector<std::string> CodeGenerator::getCode() const {
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
                
                // Check if it can be converted to a float
                bool canBeFloat = [] (const std::string& s) {
                    try {
                        size_t p;
                        std::stof(s, &p);
                        return p == s.size();
                    } catch (...) {
                        return false;
                    }
                } (instr.arg);
                
                // Adding quotations marks if necessary 
                if (instr.op == OpCode::PRINT || instr.op == OpCode::BRZ || instr.op == OpCode::BRT || instr.op == OpCode::CALL || instr.op == OpCode::JUMP) {
                    if (!canBeInt && !canBeFloat) {
                        oss << "\"" << instr.arg << "\""; // Add quotes around the string argument
                    } else {
                        oss << instr.arg; // Argument w/o quotes (int) or empty string (no arg)
                    }
                } else {
                    oss << instr.arg; // Argument w/o quotes (int) or empty string (no arg)
                }
                
                oss << ");"; // Close parentheses
            } else {
                oss << "();"; // Empty parentheses for instructions with no arguments
            }
        }
        
        // Writing to the code vector
        code.push_back(oss.str());
    }
    
    return code;
}

// Writes stack machine code to file
void CodeGenerator::printStackMachineCodeToFile(std::string filename, std::vector<std::string> code) {
    // Writing to file
    std::ofstream file(filename);
    for (std::string s : code) file << s << std::endl;
    file.close();
}