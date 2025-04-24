#include "ast.h"

void ASTNode::printNode(){
    std::cout << "Node Type: " << getNodeTypeName(type);
    std::cout << " | Token Type: " << tokenType;
    std::cout << " | Token Value: " << tokenValue;
    std::cout << " | Token Int Value: " << tokenIntValue;
    std::cout << " | Token Line: " << tokenLine;
    std::cout << " | Token Index: " << tokenIndex;
    std::cout << " | Is Float: " << (isFloat ? "true" : "false") << std::endl;
}

// Utility function to get AST node type name
std::string getNodeTypeName(ASTNodeType type) {
    static const std::unordered_map<ASTNodeType, std::string> nodeNames = {
        {ASTNodeType::PROGRAM, "PROGRAM"},
        {ASTNodeType::DECLARATION_LIST, "DECLARATION_LIST"},
        {ASTNodeType::DECLARATION, "DECLARATION"},
        {ASTNodeType::VAR_DECLARATION, "VAR_DECLARATION"},
        {ASTNodeType::TYPE_SPECIFIER, "TYPE_SPECIFIER"},
        {ASTNodeType::FUN_DECLARATION, "FUN_DECLARATION"},
        {ASTNodeType::PARAMS, "PARAMS"},
        {ASTNodeType::PARAM_LIST, "PARAM_LIST"},
        {ASTNodeType::PARAM, "PARAM"},
        {ASTNodeType::COMPOUNT_STMT, "COMPOUNT_STMT"},
        {ASTNodeType::LOCAL_DECLARATIONS, "LOCAL_DECLARATIONS"},
        {ASTNodeType::STATEMENT_LIST, "STATEMENT_LIST"},
        {ASTNodeType::STATEMENT, "STATEMENT"},
        {ASTNodeType::EXPRESSION_STMT, "EXPRESSION_STMT"},
        {ASTNodeType::SELECTION_STMT, "SELECTION_STMT"},
        {ASTNodeType::ITERATION_STMT, "ITERATION_STMT"},
        {ASTNodeType::RETURN_STMT, "RETURN_STMT"},
        {ASTNodeType::IO_STMT, "IO_STMT"},                 // New
        {ASTNodeType::INPUT_STMT, "INPUT_STMT"},           // New
        {ASTNodeType::OUTPUT_STMT, "OUTPUT_STMT"},         // New
        {ASTNodeType::EXPRESSION, "EXPRESSION"},
        {ASTNodeType::VAR, "VAR"},
        {ASTNodeType::SIMPLE_EXPRESSION, "SIMPLE_EXPRESSION"},
        {ASTNodeType::REL_OP, "REL_OP"},
        {ASTNodeType::ADDITIVE_EXPR, "ADDITIVE_EXPR"},
        {ASTNodeType::ADD_OP, "ADD_OP"},
        {ASTNodeType::TERM, "TERM"},
        {ASTNodeType::MULOP, "MULOP"},
        {ASTNodeType::FACTOR, "FACTOR"},
        {ASTNodeType::CALL, "CALL"},
        {ASTNodeType::ARGS, "ARGS"},
        {ASTNodeType::ARG_LIST, "ARG_LIST"}
    };

    auto it = nodeNames.find(type);
    if (it != nodeNames.end()) {
        return it->second;
    }
    return "UNKNOWN";
}

// ASTNode implementation
ASTNode::ASTNode(ASTNodeType t, Token* tok, bool thisIsFloat) {
    type = t;
    
    // Storing data directly
    if (tok) {
        tokenType = tok->getToken();
        tokenValue = tok->getStrVal();
        tokenIntValue = tok->getIntVal();
        tokenLine = tok->getLine();
        tokenIndex = tok->getIndex();
        isFloat = thisIsFloat;
    } else {
        tokenType = UNKNOWN;
        tokenValue = "";
        tokenIntValue = 0;
        tokenLine = -1;
        tokenIndex = -1;
        isFloat = false;
    }
    
    children = new std::vector<ASTNode*>();
    dataType = getNodeTypeName(t);
}

// Destructor
ASTNode::~ASTNode() {
    if (children) {
        for (auto child : *children) {
            delete child;
        }
        delete children;
    }
}


void ASTNode::addChild(ASTNode* child) {
    if (child != nullptr) {
        children->push_back(child);
    }
}

std::string ASTNode::getTokenString()  {
    if (this->tokenType == NUM) return "num";
    else if (this->tokenType == FLOAT_TYPE) return "float";
    else if (this->tokenType == VOID) return "void";
    else return tokenValue;
}

// Prints AST Node and children to standard output
void ASTNode::print(int indent) const {
    std::string indentation;
    for (int i = 0; i < indent; i++) {
        indentation += "| ";
    }
    
    std::cout << indentation << getNodeTypeName(type);
    
    // Use stored token data instead of token pointer
    if (tokenType != UNKNOWN) {
        std::cout << " [" << tokenValue << "]";
    }
    
    if (!dataType.empty()) {
        std::cout << " (Type: " << dataType << ")";
    }
    
    if (!tokenValue.empty() && tokenType == ID) {
        std::cout << " = " << tokenValue;
    }
    
    std::cout << std::endl;
    
    // Printing children
    for (const auto& child : *children) {
        if (child) { // Add null check for safety
            child->print(indent + 1);
        } else {
            std::cout << indentation << "| NULL CHILD" << std::endl;
        }
    }
}

// Prints AST Node and children to a file
void ASTNode::printToFile(std::ofstream& file, int indent) {
    std::string indentation;
    for (int i = 0; i < indent; i++) {
        indentation += "| ";
    }
    
    file << indentation << getNodeTypeName(type);
    
    // Use stored token data instead of token pointer
    if (tokenType != UNKNOWN) {
        file << " [" << tokenValue << "]";
    }
    
    if (!dataType.empty()) {
        file << " (Type: " << dataType << ")";
    }
    
    if (!tokenValue.empty() && tokenType == ID) {
        file << " = " << tokenValue;
    }
    
    file << std::endl;
    
    // Printing children
    for (const auto& child : *children) {
        if (child) { // Add null check for safety
            child->printToFile(file, indent + 1);
        } else {
            file << indentation << "| NULL CHILD" << std::endl;
        }
    }
}

/* Symbol Implementation*/
// Constructor
Symbol::Symbol(std::string n, SymbolType t, std::string dt, int scope, int arr) {
    name = n;
    type = t;
    dataType = dt;
    scopeLevel = scope;
    arrSize = arr; // -1 if the symbol is not an array
}

/* SymbolTable implementation */
// Constructor
SymbolTable::SymbolTable() {
    symbols = std::vector<Symbol>();
    currentScope = 0;
}

// Scope incrementor
void SymbolTable::enterScope() {
    currentScope++;
}

// Remove all symbols from the current scope, decrements scope counter
void SymbolTable::exitScope() {
    // Remove all symbols from the current scope
    auto it = symbols.begin();
    while (it != symbols.end()) {
        if (it->scopeLevel == currentScope) {
            it = symbols.erase(it);
        } else {
            ++it;
        }
    }
    currentScope--;
}

// Adds symbol to symbol table
// Returns true if symbol was added, false if symbol already exists
bool SymbolTable::addSymbol(const Symbol& symbol) {
    // Check if symbol already exists in the current scope
    for (const auto& sym : symbols) {
        if (sym.name == symbol.name && sym.scopeLevel == currentScope) {
            return false;  // Symbol already exists
        }
    }
    
    symbols.push_back(symbol);
    return true;
}

// Finds symbol in symbol table
// Returns pointer to symbol if found, nullptr if not found
Symbol* SymbolTable::findSymbol(const std::string& name) {
   
    // Look for symbol in current and outer scopes
    for (int s = currentScope; s >= 0; s--) {
        for (auto& sym : symbols) {
            if (sym.name == name && sym.scopeLevel <= s) {
                return &sym;
            }
        }
    }
    
    // Not found - print the current symbol table for debugging
    // std::cout << "Symbol not found. Current symbol table:" << std::endl;
    // for (const auto& sym : symbols) {
    //     std::cout << "Name: '" << sym.name << "', Scope: " << sym.scopeLevel << std::endl;
    // }
    
    return nullptr;  // Not found
}

int SymbolTable::getCurrentScope() const {
    return currentScope;
}

// Prints symbol statement for debugging purposes
void SymbolTable::print() const {
    std::cout << "Symbol Table:\n";
    std::cout << "*** SYMBOL TABLE ***\n";
    std::cout << "Name\tType\tDataType\tScope\tArraySize\n";
    for (const auto& sym : symbols) {
        std::cout << sym.name << "\t";
        
        switch (sym.type) {
            case SymbolType::SYMBOL_VARIABLE: std::cout << "VAR\t"; break;
            case SymbolType::SYMBOL_FUNCTION: std::cout << "FUNC\t"; break;
            case SymbolType::SYMBOL_PARAMETER: std::cout << "PARAM\t"; break;
        }
        
        std::cout << sym.dataType << "\t" << sym.scopeLevel << "\t" << sym.arrSize << "\n";
        
        std::cout << std::endl;
    }
}

/* Parser implementation */
Parser::Parser(std::vector<Token> t) {
    tokens = t;
    currentTokenIndex = 0;
    st = SymbolTable();
}

// Empty constructor, shouldn't be used
Parser::Parser() {
    tokens = std::vector<Token>();
    currentTokenIndex = 0;
    st = SymbolTable();
}

// Gets current token
Token Parser::currentToken() {
    if (currentTokenIndex < tokens.size()) {
        return tokens[currentTokenIndex];
    }
    return Token(UNKNOWN, -1); // Return a default token if we're past the end
}

// Checks if current token matches expected type
// Increment currentTokenIndex iff returning true
bool Parser::match(TokenType expectedType) {
    if (currentToken().token == expectedType) {
        nextToken();
        return true;
    }
    return false;
}

// Returns next token and increments currentTokenIndex
Token Parser::nextToken() {
    currentTokenIndex++;
    return currentToken();
}

//Generates syntax error, exits
void Parser::syntaxError() {
    Token thisToken = currentToken();
    thisToken.printError();
    exit(-1);
}

// Entry point into parsing process
ASTNode* Parser::parse() {
    return parseProgram();
}

// Rule 1: program := declaration-list | epsilon
ASTNode* Parser::parseProgram() {
    ASTNode* node = new ASTNode(ASTNodeType::PROGRAM);
    
    // Adding declaration list if this is not the end of the file
    if (tokens.size() > 0 && currentToken().token != TokenType::END_OF_FILE) {
        node->addChild(parseDeclarationList());
    }
    
    return node;
}

// Rule 2: declaration-list := declaration-list type-specifier ID declaration | type-specifier ID declaration
ASTNode* Parser::parseDeclarationList() {
    ASTNode* node = new ASTNode(ASTNodeType::DECLARATION_LIST);
    
    // Iteration handles left recursion
    while (currentToken().token == TokenType::INT || currentToken().token == TokenType::VOID || currentToken().token == TokenType::FLOAT_TYPE) {

        // Calling type-specifier parsing
        ASTNode* typeSpecNode = parseTypeSpecifier();
        
        // Confirming that next variable is an ID, as expected
        if (!match(TokenType::ID)) {
            std::cerr << "Expected identifier after type specifier in Rule 2" << std::endl;
            syntaxError();
        }
        Token idToken = tokens.at(currentTokenIndex - 1);
        
        // Adding node for child declaration
        node->addChild(parseDeclaration(typeSpecNode, idToken));
    }
    
    return node;
}

// Rule 3: declaration := var-declaration | fun-declaration
ASTNode* Parser::parseDeclaration(ASTNode* typeSpecNode, Token idToken) {
    ASTNode* node = new ASTNode(ASTNodeType::DECLARATION);
    
    // Checking if this is a variable or function declaration
    if (currentToken().token == TokenType::SEMICOLON || currentToken().token == TokenType::OBRACKET) { // Case: var
        node->addChild(parseVarDeclaration(typeSpecNode, idToken));
    } else if (currentToken().token == TokenType::OPARENTHESES) { // Case: fun
        node->addChild(parseFunDeclaration(typeSpecNode, idToken));
    } else {
        std::cerr << "SYNTAX ERROR: Expected ;, (, [ in Rule 3" << std::endl;
        syntaxError(); // Error
    }
    
    return node;
}

// Rule 4: var-declaration := ; | [ NUM ] ;
ASTNode* Parser::parseVarDeclaration(ASTNode* typeSpecNode, Token idToken) {
    ASTNode* node = new ASTNode(ASTNodeType::VAR_DECLARATION, &idToken);
    
    // Adding type specifier as child of var declaration, because it describes var
    node->addChild(typeSpecNode);
    
    // Case: array declaration
    int arraySize = -1;
    if (match(TokenType::OBRACKET)) { // Getting open bracket
        if (!match(TokenType::NUM)) { // Getting number
            std::cerr << "SYNTAX ERROR: Expected a number after [ in Rule 4" << std::endl;
            syntaxError();
        }
        
        Token t = tokens.at(currentTokenIndex - 1); // -1 because match just updated
        arraySize = t.getIntVal();
        
        if (!match(TokenType::CBRACKET)) { // Getting close bracket
            std::cerr << "SYNTAX ERROR: Expected a ] after number in Rule 4" << std::endl;
            syntaxError();
        }
    }
    
    // Require a semicolon next in either case
    if (!match(TokenType::SEMICOLON)) {
        std::cerr << "SYNTAX ERROR: Expected a ; after variable declaration in Rule 4" << std::endl;
        syntaxError();
    }
    
    // Add variable to symbol table
    std::string dataType = typeSpecNode->getTokenString();
    st.addSymbol(Symbol(idToken.getStrVal(), SymbolType::SYMBOL_VARIABLE, dataType, st.getCurrentScope(), arraySize));
    node->isFloat = (dataType == "float"); // Set isFloat flag based on data type
    
    return node;
}

// Rule 5: type-specifier := int | void | float
ASTNode* Parser::parseTypeSpecifier() {
    Token currentTok = this->currentToken();
    ASTNode* node = new ASTNode(ASTNodeType::TYPE_SPECIFIER, &currentTok);

    // Checking if the current token is a type specifier
    if (match(TokenType::INT) || match(TokenType::VOID)) {
        node->isFloat = false; // Set isFloat to false for int and void types
        return node;
    } else if (match(TokenType::FLOAT_TYPE)) {
        // If it's a float type, we need to set the isFloat flag
        node->isFloat = true;
        return node;
    }
    else {
        std::cerr << "Expected type specifier ('int' or 'void' or 'float') in Rule 5" << std::endl;
        syntaxError();
        return nullptr;  // Unreachable, just to satisfy compiler
    }
}

// Updated Rule 6: fun-declaration := ( params ) compound-stmt
ASTNode* Parser::parseFunDeclaration(ASTNode* typeSpecNode, Token idToken) {
    ASTNode* node = new ASTNode(ASTNodeType::FUN_DECLARATION, &idToken);

    // Adding type specifier as child
    node->addChild(typeSpecNode);
    
    // Add function to symbol table in current scope
    std::string returnType = typeSpecNode->getTokenString();
    st.addSymbol(Symbol(
        idToken.getStrVal(), SymbolType::SYMBOL_FUNCTION, returnType, st.getCurrentScope(), -1));
    
    // Check for open parenthesis
    if (!match(TokenType::OPARENTHESES)) {
        std::cerr << "SYNTAX ERROR: Expected ( after function name in Rule 6" << std::endl;
        syntaxError();
    }
    
    // Enter a new scope for the function parameters and body
    st.enterScope();
    
    // Process parameters
    node->addChild(parseParams());
    
    // Check for close parentheses
    if (!match(TokenType::CPARENTHESES)) {
        std::cerr << "Expected ')' after function parameters in Rule 6" << std::endl;
        syntaxError();
    }
    
    // Process compound statement - NOTE: compound-stmt will enter and exit its own scope,
    // but we want parameters to be visible inside the function body, so we DON'T exit the
    // function scope until after the compound statement is parsed
    node->addChild(parseCompoundStmt());
    
    // Exit the function scope
    st.exitScope();
    
    return node;
}

// Rule 7: params := param-list | void | empty
ASTNode* Parser::parseParams() {
    ASTNode* node = new ASTNode(ASTNodeType::PARAMS);
    
    // Check if params is "void" or empty
    if (currentToken().token == TokenType::VOID) {
        Token voidToken = currentToken();
        
        // Copying token details
        node->tokenType = voidToken.getToken();
        node->tokenValue = voidToken.getStrVal();
        node->tokenIntValue = voidToken.getIntVal();
        node->tokenLine = voidToken.getLine();
        node->tokenIndex = voidToken.getIndex();
        
        match(TokenType::VOID);
        
        // Check if it's "void" followed by more parameters, which would be a param-list starting with void type
        if (currentToken().token == TokenType::ID) {
            // In this case, "void" was a type specifier for a parameter, not a standalone void
            currentTokenIndex--; // Move back to reprocess the void token
            node->addChild(parseParamList());
        }
    } else if (currentToken().token == TokenType::INT || currentToken().token == TokenType::FLOAT_TYPE) {
        // We have a param-list starting with an int type or float type
        node->addChild(parseParamList());
    }
    // else: empty parameter list
    
    return node;
}

// Updated Rule 8: param-list := param-list , type-specifier ID param | type-specifier ID param 
ASTNode* Parser::parseParamList() {
    ASTNode* node = new ASTNode(ASTNodeType::PARAM_LIST);
    
    // First parameter
    ASTNode* typeSpecNode = parseTypeSpecifier();
    
    if (!match(TokenType::ID)) {
        std::cerr << "SYNTAX ERROR: Expected identifier after type specifier in Rule 8" << std::endl;
        syntaxError();
    }
    Token idToken = tokens.at(currentTokenIndex - 1);
    
    // Create param node
    ASTNode* paramNode = new ASTNode(ASTNodeType::PARAM, &idToken);
    paramNode->addChild(typeSpecNode);
    
    // Check for array brackets
    bool isArray = false;
    if (match(TokenType::OBRACKET)) {
        if (!match(TokenType::CBRACKET)) {
            std::cerr << "SYNTAX ERROR: Expected ] after [ in parameter declaration in Rule 8" << std::endl;
            syntaxError();
        }
        isArray = true;
    }
    
    // Add parameter to symbol table - make sure we're using getStrVal() and not toString()
    std::string dataType = typeSpecNode->getTokenString();
    st.addSymbol(Symbol(idToken.getStrVal(), SymbolType::SYMBOL_PARAMETER, dataType, st.getCurrentScope(), isArray ? 0 : -1));
    
    // Add param to param-list
    node->addChild(paramNode);
    
    // Process additional parameters if any (comma-separated)
    while (match(TokenType::COMMA)) {
        typeSpecNode = parseTypeSpecifier();
        
        if (!match(TokenType::ID)) {
            std::cerr << "SYNTAX ERROR: Expected identifier after type specifier in comma-separated parameter list in Rule 8" << std::endl;
            syntaxError();
        }
        idToken = tokens.at(currentTokenIndex - 1);
        
        // Create param node
        paramNode = new ASTNode(ASTNodeType::PARAM, &idToken);
        paramNode->addChild(typeSpecNode);
        
        // Check for array brackets
        isArray = false;
        if (match(TokenType::OBRACKET)) {
            if (!match(TokenType::CBRACKET)) {
                std::cerr << "SYNTAX ERROR: Expected ] after [ in parameter declaration in Rule 8" << std::endl;
                syntaxError();
            }
            isArray = true;
        }
        
        // Add parameter to symbol table
        dataType = typeSpecNode->getTokenString();
        st.addSymbol(Symbol(idToken.getStrVal(), SymbolType::SYMBOL_PARAMETER, dataType, st.getCurrentScope(), isArray ? 0 : -1));
        
        // Add param to param-list
        node->addChild(paramNode);
    }
    
    return node;
}

// Updated Rule 10: compound-stmt := { local-declarations statement-list }
ASTNode* Parser::parseCompoundStmt() {
    ASTNode* node = new ASTNode(ASTNodeType::COMPOUNT_STMT);
    
    // Check for opening curly brace
    if (!match(TokenType::OCURLY)) {
        std::cerr << "SYNTAX ERROR: Expected { at start of compound statement in Rule 10" << std::endl;
        syntaxError();
    }
    
    // Parse local declarations
    node->addChild(parseLocalDeclarations());
    
    // Parse statement list
    node->addChild(parseStatementList());
    
    // Check for closing curly brace
    if (!match(TokenType::CCURLY)) {
        std::cerr << "SYNTAX ERROR: Expected } at end of compound statement in Rule 10" << std::endl;
        syntaxError();
    }
    
    return node;
}

// Rule 11: local-declarations := local-declarations var-declaration | var-declaration
ASTNode* Parser::parseLocalDeclarations() {
    ASTNode* node = new ASTNode(ASTNodeType::LOCAL_DECLARATIONS);
    
    // Process all variable declarations
    while (currentToken().token == TokenType::INT || currentToken().token == TokenType::VOID || currentToken().token == TokenType::FLOAT_TYPE) {
        // Parse type specifier
        ASTNode* typeSpecNode = parseTypeSpecifier();
        
        // Check for ID
        if (!match(TokenType::ID)) {
            std::cerr << "SYNTAX ERROR: Expected identifier after type specifier in local declarations in Rule 11" << std::endl;
            syntaxError();
        }
        Token idToken = tokens.at(currentTokenIndex - 1);
        
        // Parse var declaration
        node->addChild(parseVarDeclaration(typeSpecNode, idToken));
    }
    
    return node;
}

// Rule 12: statement-list := statement-list statement | statement
ASTNode* Parser::parseStatementList() {
    ASTNode* node = new ASTNode(ASTNodeType::STATEMENT_LIST);
    
    // Parse statements until we reach the end of the block (})
    while (currentToken().token != TokenType::CCURLY && 
           currentToken().token != TokenType::END_OF_FILE) {
        node->addChild(parseStatement());
    }
    
    return node;
}

// Rule 13: statement := expression-stmt | compound-stmt | selection-stmt | iteration-stmt | return-stmt | io-stmt
ASTNode* Parser::parseStatement() {
    ASTNode* node = new ASTNode(ASTNodeType::STATEMENT);
    
    switch (currentToken().token) {
        case TokenType::SEMICOLON:
        case TokenType::ID:
        case TokenType::NUM:
        case TokenType::FLOAT_VAL:
        case TokenType::OPARENTHESES:
            // These tokens can start an expression statement
            node->addChild(parseExpressionStmt());
            break;
            
        case TokenType::OCURLY:
            // Compound statement
            node->addChild(parseCompoundStmt());
            break;
            
        case TokenType::IF:
            // Selection statement
            node->addChild(parseSelectionStmt());
            break;
            
        case TokenType::WHILE:
            // Iteration statement
            node->addChild(parseIterationStmt());
            break;
            
        case TokenType::RETURN:
            // Return statement
            node->addChild(parseReturnStmt());
            break;
            
        case TokenType::INPUT:
        case TokenType::OUTPUT:
            // IO statement
            node->addChild(parseIOStmt());
            break;
            
        default:
            std::cerr << "SYNTAX ERROR: Unexpected token in statement in Rule 13" << std::endl;
            syntaxError();
    }
    
    return node;
}

// Rule 14: io-stmt := input-stmt | output-stmt
ASTNode* Parser::parseIOStmt() {
    ASTNode* node = new ASTNode(ASTNodeType::IO_STMT);
    
    if (currentToken().token == TokenType::INPUT) {
        node->addChild(parseInputStmt());
    } else if (currentToken().token == TokenType::OUTPUT) {
        node->addChild(parseOutputStmt());
    } else {
        std::cerr << "SYNTAX ERROR: Expected 'input' or 'output' in IO statement in Rule 14" << std::endl;
        syntaxError();
    }
    
    return node;
}

// Rule 15: input-stmt := input ( STRING ) ;
ASTNode* Parser::parseInputStmt() {
    Token inputToken = currentToken();
    ASTNode* node = new ASTNode(ASTNodeType::INPUT_STMT, &inputToken);
    
    // Match 'input'
    if (!match(TokenType::INPUT)) {
        std::cerr << "SYNTAX ERROR: Expected 'input' at start of input statement in Rule 15" << std::endl;
        syntaxError();
    }
    
    // Match '('
    if (!match(TokenType::OPARENTHESES)) {
        std::cerr << "SYNTAX ERROR: Expected ( after 'input' in Rule 15" << std::endl;
        syntaxError();
    }
    
    // Match STRING
    if (!match(TokenType::STRING)) {
        std::cerr << "SYNTAX ERROR: Expected string literal in input statement in Rule 15" << std::endl;
        syntaxError();
    }
    
    // Save STRING token
    Token stringToken = tokens.at(currentTokenIndex - 1);
    ASTNode* stringNode = new ASTNode(ASTNodeType::FACTOR, &stringToken);
    node->addChild(stringNode);
    
    // Match ')'
    if (!match(TokenType::CPARENTHESES)) {
        std::cerr << "SYNTAX ERROR: Expected ) after string in input statement in Rule 15" << std::endl;
        syntaxError();
    }
    
    return node;
}

// Rule 16: output-stmt := output ( STRING ) ; | output ( expression ) ;
ASTNode* Parser::parseOutputStmt() {
    Token outputToken = currentToken();
    ASTNode* node = new ASTNode(ASTNodeType::OUTPUT_STMT, &outputToken);
    
    // Match 'output'
    if (!match(TokenType::OUTPUT)) {
        std::cerr << "SYNTAX ERROR: Expected 'output' at start of output statement in Rule 16" << std::endl;
        syntaxError();
    }
    
    // Match '('
    if (!match(TokenType::OPARENTHESES)) {
        std::cerr << "SYNTAX ERROR: Expected ( after 'output' in Rule 16" << std::endl;
        syntaxError();
    }
    
    // Check if the next token is a STRING
    if (currentToken().token == TokenType::STRING) {
        // Match STRING
        match(TokenType::STRING);
        
        // Save STRING token
        Token stringToken = tokens.at(currentTokenIndex - 1);
        ASTNode* stringNode = new ASTNode(ASTNodeType::FACTOR, &stringToken);
        node->addChild(stringNode);
    } else {
        // Parse expression
        node->addChild(parseExpression());
    }
    
    // Match ')'
    if (!match(TokenType::CPARENTHESES)) {
        std::cerr << "SYNTAX ERROR: Expected ) after expression or string in output statement in Rule 16" << std::endl;
        syntaxError();
    }
    
    return node;
}

// Rule 17: expression-stmt := expression ; | ;
ASTNode* Parser::parseExpressionStmt() {
    ASTNode* node = new ASTNode(ASTNodeType::EXPRESSION_STMT);
    
    // Check if it's just a semicolon
    if (match(TokenType::SEMICOLON)) {
        return node; // Empty expression statement
    }
    
    // Otherwise, it's an expression followed by a semicolon
    node->addChild(parseExpression());
    
    if (!match(TokenType::SEMICOLON)) {
        std::cerr << "SYNTAX ERROR: Expected ; after expression in Rule 17" << std::endl;
        syntaxError();
    }
    
    return node;
}

// Rule 18: selection-stmt := if ( simple-expression ) statement | if ( simple-expression ) statement else statement
ASTNode* Parser::parseSelectionStmt() {
    ASTNode* node = new ASTNode(ASTNodeType::SELECTION_STMT);
    
    // Match 'if'
    if (!match(TokenType::IF)) {
        std::cerr << "SYNTAX ERROR: Expected 'if' at start of selection statement in Rule 18" << std::endl;
        syntaxError();
    }
    
    // Match '('
    if (!match(TokenType::OPARENTHESES)) {
        std::cerr << "SYNTAX ERROR: Expected ( after 'if' in Rule 18" << std::endl;
        syntaxError();
    }
    
    // Parse simple-expression instead of expression
    node->addChild(parseSimpleExpression());
    
    // Match ')'
    if (!match(TokenType::CPARENTHESES)) {
        std::cerr << "SYNTAX ERROR: Expected ) after expression in 'if' statement in Rule 18" << std::endl;
        syntaxError();
    }
    
    // Parse 'then' statement
    node->addChild(parseStatement());
    
    // Check for 'else'
    if (match(TokenType::ELSE)) {
        // Parse 'else' statement
        node->addChild(parseStatement());
    }
    
    return node;
}

// Rule 19: iteration-stmt := while ( expression ) statement
ASTNode* Parser::parseIterationStmt() {
    ASTNode* node = new ASTNode(ASTNodeType::ITERATION_STMT);
    
    // Match 'while'
    if (!match(TokenType::WHILE)) {
        std::cerr << "SYNTAX ERROR: Expected 'while' at start of iteration statement in Rule 19" << std::endl;
        syntaxError();
    }
    
    // Match '('
    if (!match(TokenType::OPARENTHESES)) {
        std::cerr << "SYNTAX ERROR: Expected ( after 'while' in Rule 19" << std::endl;
        syntaxError();
    }
    
    // Parse expression
    node->addChild(parseExpression());
    
    // Match ')'
    if (!match(TokenType::CPARENTHESES)) {
        std::cerr << "SYNTAX ERROR: Expected ) after expression in 'while' statement in Rule 19" << std::endl;
        syntaxError();
    }
    
    // Parse loop body statement
    node->addChild(parseStatement());
    
    return node;
}

// Rule 20: return-stmt := return ; | return expression ;
ASTNode* Parser::parseReturnStmt() {
    ASTNode* node = new ASTNode(ASTNodeType::RETURN_STMT);
    
    // Match 'return'
    if (!match(TokenType::RETURN)) {
        std::cerr << "SYNTAX ERROR: Expected 'return' at start of return statement in Rule 20" << std::endl;
        syntaxError();
    }
    
    // Check if it's a return with expression or just a return
    if (currentToken().token != TokenType::SEMICOLON) {
        node->addChild(parseExpression());
    }
    
    // Match semicolon
    if (!match(TokenType::SEMICOLON)) {
        std::cerr << "SYNTAX ERROR: Expected ; after return statement in Rule 20" << std::endl;
        syntaxError();
    }
    
    return node;
}

// Rule 21: expression := var = simple-expression | simple-expression
ASTNode* Parser::parseExpression() {
    ASTNode* node = new ASTNode(ASTNodeType::EXPRESSION);
    
    // Look ahead to see if this is a variable assignment
    if (currentToken().token == TokenType::ID) {
        // Save current token index
        int savedIndex = currentTokenIndex;
        
        // Try to parse a var
        ASTNode* varNode = parseVar();
        
        // Check if next token is '='
        if (match(TokenType::EQUALS)) {
            // This is an assignment expression
            node->addChild(varNode);
            
            // Parse the right side of the assignment
            node->addChild(parseSimpleExpression());
        } else {
            // This is a simple expression, so reset and parse as simple-expression
            currentTokenIndex = savedIndex;
            node->addChild(parseSimpleExpression());
        }
    } else {
        // This is a simple expression
        node->addChild(parseSimpleExpression());
    }
    
    return node;
}

// Updated Rule 22: var := ID | ID [ simple-expression ]
ASTNode* Parser::parseVar() {
    // Check for ID
    if (!match(TokenType::ID)) {
        std::cerr << "SYNTAX ERROR: Expected identifier at start of variable in Rule 22" << std::endl;
        syntaxError();
    }
    
    // Save the ID token for the var node
    Token idToken = tokens.at(currentTokenIndex - 1);
    
    // Check if the variable exists in the symbol table - use getStrVal() consistently
    Symbol* varSymbol = st.findSymbol(idToken.getStrVal());
    if (!varSymbol) {
        std::cerr << "SEMANTIC ERROR: Undeclared variable '" << idToken.getStrVal() << "' in Rule 22" << std::endl;
        syntaxError();
    }
    
    ASTNode* node = new ASTNode(ASTNodeType::VAR, &idToken);
    
    // Check if this is an array access
    if (match(TokenType::OBRACKET)) {
        // Make sure the variable is actually an array
        if (varSymbol->arrSize == -1) {
            std::cerr << "SEMANTIC ERROR: Variable '" << idToken.getStrVal() << "' is not an array in Rule 22" << std::endl;
            syntaxError();
        }
        
        // Parse the index simple-expression
        node->addChild(parseSimpleExpression());
        
        // Match closing bracket
        if (!match(TokenType::CBRACKET)) {
            std::cerr << "SYNTAX ERROR: Expected ] after array index expression in Rule 22" << std::endl;
            syntaxError();
        }
    } else if (varSymbol->arrSize != -1) {
        // If it's an array but not accessed with [], it might be a semantic error
        // For this compiler, we'll allow it but note it might be an issue
    }
    
    return node;
}

// Rule 23: simple-expression := additive-expression relop additive-expression | additive-expression
ASTNode* Parser::parseSimpleExpression() {
    ASTNode* node = new ASTNode(ASTNodeType::SIMPLE_EXPRESSION);
    
    // Parse the first additive expression
    ASTNode* leftExpr = parseAdditiveExpr();
    node->addChild(leftExpr);
    
    // Check if there's a relational operator
    TokenType currentTok = currentToken().token;
    if (currentTok == TokenType::LE || currentTok == TokenType::LT || 
        currentTok == TokenType::GT || currentTok == TokenType::GE || 
        currentTok == TokenType::EE || currentTok == TokenType::NE) {
        
        // Parse the relational operator
        ASTNode* relopNode = parseRelOp();
        node->addChild(relopNode);
        
        // Parse the right additive expression
        ASTNode* rightExpr = parseAdditiveExpr();
        node->addChild(rightExpr);
    }
    
    return node;
}

// Rule 24: relop := <= | < | > | >= | == | !=
ASTNode* Parser::parseRelOp() {
    Token opToken = currentToken();
    ASTNode* node = new ASTNode(ASTNodeType::REL_OP, &opToken);

    if (match(TokenType::LE)) {
        node->tokenType = TokenType::LE;
        node->tokenValue = opToken.toString();
    } else if (match(TokenType::LT)) {
        node->tokenType = TokenType::LT;
        node->tokenValue = opToken.toString();
    } else if (match(TokenType::GT)) {
        node->tokenType = TokenType::GT;
        node->tokenValue = opToken.toString();
    } else if (match(TokenType::GE)) {
        node->tokenType = TokenType::GE;
        node->tokenValue = opToken.toString();
    } else if (match(TokenType::EE)) {
        node->tokenType = TokenType::EE;
        node->tokenValue = opToken.toString();
    } else if (match(TokenType::NE)) {
        node->tokenType = TokenType::NE;
        node->tokenValue = opToken.toString();
    } else {
        std::cerr << "SYNTAX ERROR: Expected relational operator in Rule 24" << std::endl;
        syntaxError();
        return nullptr; // Unreachable
    }

    return node; // Only returns if match was found

}

// Rule 25: additive-expression := additive-expression addop term | term
ASTNode* Parser::parseAdditiveExpr() {
    ASTNode* node = new ASTNode(ASTNodeType::ADDITIVE_EXPR);
    
    // Parse the first term
    ASTNode* termNode = parseTerm();
    node->addChild(termNode);
    
    // Handle the left recursion by iteration
    while (currentToken().token == TokenType::PLUS || 
           currentToken().token == TokenType::MINUS) {
        
        // Parse the add operator
        ASTNode* addOpNode = parseAddOp();
        node->addChild(addOpNode);
        
        // Parse the next term
        ASTNode* nextTerm = parseTerm();
        node->addChild(nextTerm);
    }
    
    return node;
}

// Rule 26: add-op := + | -
ASTNode* Parser::parseAddOp() {
    Token opToken = currentToken();
    ASTNode* node = new ASTNode(ASTNodeType::ADD_OP, &opToken);
    
    if (match(TokenType::PLUS)) {
        node->tokenType = TokenType::PLUS;
        node->tokenValue = opToken.toString();
        return node;
    } else if (match(TokenType::MINUS)) {
        node->tokenType = TokenType::MINUS;
        node->tokenValue = opToken.toString();
        return node;
    } else {
        std::cerr << "SYNTAX ERROR: Expected additive operator (+ or -) in Rule 26" << std::endl;
        syntaxError();
        return nullptr; // Unreachable
    }
}

// Rule 27: term := term mulop factor | factor
ASTNode* Parser::parseTerm() {
    ASTNode* node = new ASTNode(ASTNodeType::TERM);
    
    // Parse the first factor
    ASTNode* factorNode = parseFactor();
    node->addChild(factorNode);
    
    // Handle the left recursion by iteration
    while (currentToken().token == TokenType::TIMES || 
           currentToken().token == TokenType::DIVIDE) {
        
        // Parse the multiplicative operator
        ASTNode* mulOpNode = parseMulOp();
        node->addChild(mulOpNode);
        
        // Parse the next factor
        ASTNode* nextFactor = parseFactor();
        node->addChild(nextFactor);
    }
    
    return node;
}

// Rule 28: mulop := * | /
ASTNode* Parser::parseMulOp() {
    Token opToken = currentToken();
    ASTNode* node = new ASTNode(ASTNodeType::MULOP, &opToken);

    if (match(TokenType::TIMES)) {
        node->tokenType = TokenType::TIMES;
        node->tokenValue = opToken.toString();
        return node;
    } else if (match(TokenType::DIVIDE)) {
        node->tokenType = TokenType::DIVIDE;
        node->tokenValue = opToken.toString();
        return node;
    } else {
        std::cerr << "SYNTAX ERROR: Expected multiplicative operator (* or /) in Rule 28" << std::endl;
        syntaxError();
        return nullptr; // Unreachable
    }
}

// Rule 29: factor := ( simple-expression ) | var | call | NUM | FLOAT | input-stmt
ASTNode* Parser::parseFactor() {
    ASTNode* node = new ASTNode(ASTNodeType::FACTOR);
    
    switch (currentToken().token) {
        case TokenType::OPARENTHESES: {
            // ( simple-expression )
            match(TokenType::OPARENTHESES);
            node->addChild(parseSimpleExpression());
            
            if (!match(TokenType::CPARENTHESES)) {
                std::cerr << "SYNTAX ERROR: Expected ) after expression in factor in Rule 29" << std::endl;
                syntaxError();
            }
            break;
        }
        
        case TokenType::ID: {
            // Look ahead to determine if this is a function call or a variable
            int savedIndex = currentTokenIndex;
            match(TokenType::ID);
            
            if (currentToken().token == TokenType::OPARENTHESES) {
                // This is a function call
                currentTokenIndex = savedIndex; // Reset to reprocess the ID
                node->addChild(parseCall());
            } else {
                // This is a variable
                currentTokenIndex = savedIndex; // Reset to reprocess the ID
                node->addChild(parseVar());
            }
            break;
        }
        
        case TokenType::NUM: {
            // NUM
            Token numToken = currentToken();
            match(TokenType::NUM);
            
            // Create a node for the number with its token
            ASTNode* numNode = new ASTNode(ASTNodeType::FACTOR, &numToken);
            node->addChild(numNode);
            break;
        }

        case TokenType::FLOAT_VAL: {
            // FLOAT
            Token floatToken = currentToken();
            match(TokenType::FLOAT_VAL);
            
            // Create a node for the float with its token
            ASTNode* floatNode = new ASTNode(ASTNodeType::FACTOR, &floatToken);
            node->addChild(floatNode);
            break;
        }
        
        case TokenType::INPUT: {
            // input-stmt
            node->addChild(parseInputStmt());
            break;
        }
        
        default:
            std::cerr << "SYNTAX ERROR: Unexpected token in factor in Rule 29" << std::endl;
            syntaxError();
    }
    
    return node;
}

// Rule 30: call := ID ( args )
ASTNode* Parser::parseCall() {
    // Check for function ID
    if (!match(TokenType::ID)) {
        std::cerr << "SYNTAX ERROR: Expected function identifier at start of call in Rule 30" << std::endl;
        syntaxError();
    }
    
    // Save the function ID token
    Token idToken = tokens.at(currentTokenIndex - 1);
    
    // Check if the function exists in the symbol table
    Symbol* funcSymbol = st.findSymbol(idToken.getStrVal());
    if (!funcSymbol || funcSymbol->type != SymbolType::SYMBOL_FUNCTION) {
        std::cerr << "SEMANTIC ERROR: Undeclared function '" << idToken.getStrVal() << "' in Rule 30" << std::endl;
        syntaxError();
    }
    
    ASTNode* node = new ASTNode(ASTNodeType::CALL, &idToken);
    
    // Match opening parenthesis
    if (!match(TokenType::OPARENTHESES)) {
        std::cerr << "SYNTAX ERROR: Expected ( after function identifier in Rule 30" << std::endl;
        syntaxError();
    }
    
    // Parse arguments
    node->addChild(parseArgs());
    
    // Match closing parenthesis
    if (!match(TokenType::CPARENTHESES)) {
        std::cerr << "SYNTAX ERROR: Expected ) after arguments in function call in Rule 30" << std::endl;
        syntaxError();
    }
    
    return node;
}

// Rule 31: args := arg-list | ε
ASTNode* Parser::parseArgs() {
    ASTNode* node = new ASTNode(ASTNodeType::ARGS);
    
    // Check if there are any arguments
    if (currentToken().token != TokenType::CPARENTHESES) {
        // If there are arguments, parse the argument list
        node->addChild(parseArgList());
    }
    // else: empty argument list
    
    return node;
}

// Rule 31: arg-list := arg-list , expression | expression
ASTNode* Parser::parseArgList() {
    ASTNode* node = new ASTNode(ASTNodeType::ARG_LIST);
    
    // Parse the first argument
    node->addChild(parseExpression());
    
    // Process additional arguments (comma-separated)
    while (match(TokenType::COMMA)) {
        node->addChild(parseExpression());
    }
    
    return node;
}