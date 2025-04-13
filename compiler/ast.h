// Updated ast.h
#ifndef AST_H
#define AST_H

#include <iostream>
#include <vector>
#include <string>
#include <unordered_map>
#include <memory>
#include <stdexcept>
#include <ostream>

#include "token.h"

/* Defining AST Node Type */
enum class ASTNodeType {
    PROGRAM, // RULE 1
    DECLARATION_LIST, // RULE 2
    DECLARATION, // RULE 3
    VAR_DECLARATION, // RULE 4
    TYPE_SPECIFIER, // RULE 5
    FUN_DECLARATION, // RULE 6
    PARAMS, // RULE 7
    PARAM_LIST, // RULE 8
    PARAM, // RULE 9
    COMPOUNT_STMT, // RULE 10
    LOCAL_DECLARATIONS, // RULE 11
    STATEMENT_LIST, // RULE 12
    STATEMENT, // RULE 13
    IO_STMT, // RULE 14
    INPUT_STMT, // RULE 15
    OUTPUT_STMT, // RULE 16
    EXPRESSION_STMT, // RULE 17
    SELECTION_STMT, // RULE 18
    ITERATION_STMT, // RULE 19
    RETURN_STMT, // RULE 20
    EXPRESSION, // RULE 21
    VAR, // RULE 22
    SIMPLE_EXPRESSION, // RULE 23
    REL_OP, // RULE 24
    ADDITIVE_EXPR, // RULE 25
    ADD_OP, // RULE 26
    TERM, // RULE 27
    MULOP, // RULE 28
    FACTOR, // RULE 29
    CALL, // RULE 30
    ARGS, // RULE 31
    ARG_LIST // RULE 32
};
 
// Utility function to get AST node type name
std::string getNodeTypeName(ASTNodeType type);

// AST Node Class
class ASTNode {
    public:
        ASTNodeType type;
        
        // Store token data directly instead of pointers
        TokenType tokenType;
        std::string tokenValue;
        int tokenIntValue;
        int tokenLine;
        int tokenIndex;
        
        std::vector<ASTNode*> *children;
        std::string dataType;  // For type checking during semantic analysis

        // Constructor
        ASTNode(ASTNodeType t, Token* tok = nullptr);
        
        // Destructor for proper cleanup
        ~ASTNode();

        /* Adds Child */
        void addChild(ASTNode* child);
        std::string getTokenString();

        // Print the AST for debugging purposes
        void print(int indent = 0) const;
};

// Symbol Tables
enum SymbolType {
    SYMBOL_VARIABLE,
    SYMBOL_FUNCTION,
    SYMBOL_PARAMETER
};

class Symbol {
    public:
        std::string name;
        SymbolType type;
        std::string dataType;  // "int" or "void"
        int scopeLevel;
        int arrSize;

        Symbol(std::string n, SymbolType t, std::string dt, int scope, int arrSize = -1);
};

class SymbolTable {
private:
    std::vector<Symbol> symbols;
    int currentScope;

public:
    SymbolTable();
    
    /* Enter new scope*/
    void enterScope();
    
    /* Remove variables from current scope, end scope*/
    void exitScope();
    
    /* Adds new symbol to scope*/
    bool addSymbol(const Symbol& symbol);
    
    /* Searches for symbol from current to largest scope*/
    Symbol* findSymbol(const std::string& name);
    
    int getCurrentScope() const;
    
    /* Print symbol table */
    void print() const;
};

class Parser {
private:
    // Attributes
    SymbolTable st;
    std::vector<Token> tokens;
    int currentTokenIndex;

    // Support functions
    /* Gets current token*/
    Token currentToken();

    /* Checks if current token is expected type, advances if so*/
    bool match(TokenType expectedType);

    /* Advances and calls current token*/
    Token nextToken();

    /* Throws syntax error*/
    void syntaxError();

public:
    // Constructor
    Parser(std::vector<Token> t);
    Parser();

    // Main parse function
    /* Calls parseProgram, the first rule*/
    ASTNode* parse();

    // Parsing methods for grammar rules
    ASTNode* parseProgram();
    ASTNode* parseDeclarationList();
    ASTNode* parseDeclaration(ASTNode* typeSpecNode, Token idToken);
    ASTNode* parseVarDeclaration(ASTNode* typeSpecNode, Token idToken);
    ASTNode* parseTypeSpecifier();
    ASTNode* parseFunDeclaration(ASTNode* typeSpecNode, Token idToken);
    ASTNode* parseParams();
    ASTNode* parseParamList();
    ASTNode* parseCompoundStmt();
    ASTNode* parseLocalDeclarations();
    ASTNode* parseStatementList();
    ASTNode* parseStatement();
    ASTNode* parseIOStmt();
    ASTNode* parseInputStmt();
    ASTNode* parseOutputStmt();
    ASTNode* parseExpressionStmt();
    ASTNode* parseSelectionStmt();
    ASTNode* parseIterationStmt();
    ASTNode* parseReturnStmt();
    ASTNode* parseExpression();
    ASTNode* parseVar();
    ASTNode* parseSimpleExpression();
    ASTNode* parseRelOp();
    ASTNode* parseAdditiveExpr();
    ASTNode* parseAddOp();
    ASTNode* parseTerm();
    ASTNode* parseMulOp();
    ASTNode* parseFactor();
    ASTNode* parseCall();
    ASTNode* parseArgs();
    ASTNode* parseArgList();
};

#endif // AST_H