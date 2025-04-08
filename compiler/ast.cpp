#include "ast.h"

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
ASTNode::ASTNode(ASTNodeType t, Token* tok) {
    type = t;
    token = tok;
    children = new std::vector<ASTNode*>();
    dataType = getNodeTypeName(t);
}

void ASTNode::addChild(ASTNode* child) {
    if (child != nullptr) {
        children->push_back(child);
    }
}

void ASTNode::print(int indent) const {
    std::string indentation(indent * 2, ' ');
    std::cout << indentation << getNodeTypeName(type);
    
    if (token) {
        std::cout << " [" << token->toString() << "]";
    }
    
    if (!dataType.empty()) {
        std::cout << " (Type: " << dataType << ")";
    }
    
    if (token && !token->getStrVal().empty()) {
        std::cout << " = " << token->getStrVal();
    }
    
    std::cout << std::endl;
    
    for (const auto& child : *children) {
        child->print(indent + 1);
    }
}

// Symbol implementation
Symbol::Symbol(std::string n, SymbolType t, std::string dt, int scope, int arr) {
    name = n;
    type = t;
    dataType = dt;
    scopeLevel = scope;
    arrSize = arr; // -1 if the symbol is not an array
}

// SymbolTable implementation
SymbolTable::SymbolTable() {
    symbols = std::vector<Symbol>();
    currentScope = 0;
}

void SymbolTable::enterScope() {
    currentScope++;
}

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

Symbol* SymbolTable::findSymbol(const std::string& name) {
    // Debug output to see what's happening
    std::cout << "Looking for symbol: '" << name << "' in scopes from " << currentScope << " down to 0" << std::endl;
    
    // Look for symbol in current and outer scopes
    for (int s = currentScope; s >= 0; s--) {
        for (auto& sym : symbols) {
            if (sym.name == name && sym.scopeLevel <= s) {
                std::cout << "Found symbol '" << name << "' in scope " << sym.scopeLevel << std::endl;
                return &sym;
            }
        }
    }
    
    // Not found - print the current symbol table for debugging
    std::cout << "Symbol not found. Current symbol table:" << std::endl;
    for (const auto& sym : symbols) {
        std::cout << "Name: '" << sym.name << "', Scope: " << sym.scopeLevel << std::endl;
    }
    
    return nullptr;  // Not found
}

int SymbolTable::getCurrentScope() const {
    return currentScope;
}

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

// Parser implementation
Parser::Parser(std::vector<Token> t) {
    tokens = t;
    currentTokenIndex = 0;
    st = SymbolTable();
}

Parser::Parser() {
    tokens = std::vector<Token>();
    currentTokenIndex = 0;
    st = SymbolTable();
}

Token Parser::currentToken() {
    if (currentTokenIndex < tokens.size()) {
        return tokens[currentTokenIndex];
    }
    return Token(UNKNOWN, -1); // Return a default token if we're past the end
}

bool Parser::match(TokenType expectedType) {
    if (currentToken().token == expectedType) {
        nextToken();
        return true;
    }
    return false;
}

Token Parser::nextToken() {
    currentTokenIndex++;
    return currentToken();
}

void Parser::syntaxError() {
    Token thisToken = currentToken();
    thisToken.printError();
    exit(-1);
}

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
    while (currentToken().token == TokenType::INT || currentToken().token == TokenType::VOID) {

        // Calling type-specifier parsing
        ASTNode* typeSpecNode = parseTypeSpecifier();
        
        // Confirming that next variable is an ID, as expected
        if (!match(TokenType::ID)) {
            std::cerr << "Expected identifier after type specifier in Rule 2" << std::endl;
            syntaxError();
        }
        Token idToken = tokens.at(currentTokenIndex - 1);
        
        // Adding node for child
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
    std::string dataType = getNodeTypeName(typeSpecNode->type);
    st.addSymbol(Symbol(idToken.toString(), SymbolType::SYMBOL_VARIABLE, dataType, st.getCurrentScope(), arraySize));
    
    return node;
}

// Rule 5: type-specifier := int | void
ASTNode* Parser::parseTypeSpecifier() {
    Token currentTok = this->currentToken();
    ASTNode* node = new ASTNode(ASTNodeType::TYPE_SPECIFIER, &currentTok);

    // Checking if the current token is a type specifier
    if (match(TokenType::INT) || match(TokenType::VOID)) {
        return node;
    } else {
        std::cerr << "Expected type specifier ('int' or 'void') in Rule 5" << std::endl;
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
    std::string returnType = typeSpecNode->token->toString();
    st.addSymbol(Symbol(
        idToken.getStrVal(), SymbolType::SYMBOL_FUNCTION, returnType, st.getCurrentScope()));
    
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
        node->token = &voidToken;
        match(TokenType::VOID);
        
        // Check if it's "void" followed by more parameters, which would be a param-list starting with void type
        if (currentToken().token == TokenType::ID) {
            // In this case, "void" was a type specifier for a parameter, not a standalone void
            currentTokenIndex--; // Move back to reprocess the void token
            node->addChild(parseParamList());
        }
    } else if (currentToken().token == TokenType::INT) {
        // We have a param-list starting with an int type
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
    std::string dataType = typeSpecNode->token->toString();
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
        dataType = typeSpecNode->token->toString();
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
    while (currentToken().token == TokenType::INT || currentToken().token == TokenType::VOID) {
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

// Rule 13: statement := expression-stmt | compound-stmt | selection-stmt | iteration-stmt | return-stmt
ASTNode* Parser::parseStatement() {
    ASTNode* node = new ASTNode(ASTNodeType::STATEMENT);
    
    switch (currentToken().token) {
        case TokenType::SEMICOLON:
        case TokenType::ID:
        case TokenType::NUM:
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
            
        default:
            std::cerr << "SYNTAX ERROR: Unexpected token in statement in Rule 13" << std::endl;
            syntaxError();
    }
    
    return node;
}

// Rule 14: expression-stmt := expression ; | ;
ASTNode* Parser::parseExpressionStmt() {
    ASTNode* node = new ASTNode(ASTNodeType::EXPRESSION_STMT);
    
    // Check if it's just a semicolon
    if (match(TokenType::SEMICOLON)) {
        return node; // Empty expression statement
    }
    
    // Otherwise, it's an expression followed by a semicolon
    node->addChild(parseExpression());
    
    if (!match(TokenType::SEMICOLON)) {
        std::cerr << "SYNTAX ERROR: Expected ; after expression in Rule 14" << std::endl;
        syntaxError();
    }
    
    return node;
}

// Rule 15: selection-stmt := if ( simple-expression ) statement | if ( simple-expression ) statement else statement
ASTNode* Parser::parseSelectionStmt() {
    ASTNode* node = new ASTNode(ASTNodeType::SELECTION_STMT);
    
    // Match 'if'
    if (!match(TokenType::IF)) {
        std::cerr << "SYNTAX ERROR: Expected 'if' at start of selection statement in Rule 15" << std::endl;
        syntaxError();
    }
    
    // Match '('
    if (!match(TokenType::OPARENTHESES)) {
        std::cerr << "SYNTAX ERROR: Expected ( after 'if' in Rule 15" << std::endl;
        syntaxError();
    }
    
    // Parse simple-expression instead of expression
    node->addChild(parseSimpleExpression());
    
    // Match ')'
    if (!match(TokenType::CPARENTHESES)) {
        std::cerr << "SYNTAX ERROR: Expected ) after expression in 'if' statement in Rule 15" << std::endl;
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

// Rule 16: iteration-stmt := while ( expression ) statement
ASTNode* Parser::parseIterationStmt() {
    ASTNode* node = new ASTNode(ASTNodeType::ITERATION_STMT);
    
    // Match 'while'
    if (!match(TokenType::WHILE)) {
        std::cerr << "SYNTAX ERROR: Expected 'while' at start of iteration statement in Rule 16" << std::endl;
        syntaxError();
    }
    
    // Match '('
    if (!match(TokenType::OPARENTHESES)) {
        std::cerr << "SYNTAX ERROR: Expected ( after 'while' in Rule 16" << std::endl;
        syntaxError();
    }
    
    // Parse expression
    node->addChild(parseExpression());
    
    // Match ')'
    if (!match(TokenType::CPARENTHESES)) {
        std::cerr << "SYNTAX ERROR: Expected ) after expression in 'while' statement in Rule 16" << std::endl;
        syntaxError();
    }
    
    // Parse loop body statement
    node->addChild(parseStatement());
    
    return node;
}

// Rule 17: return-stmt := return ; | return expression ;
ASTNode* Parser::parseReturnStmt() {
    ASTNode* node = new ASTNode(ASTNodeType::RETURN_STMT);
    
    // Match 'return'
    if (!match(TokenType::RETURN)) {
        std::cerr << "SYNTAX ERROR: Expected 'return' at start of return statement in Rule 17" << std::endl;
        syntaxError();
    }
    
    // Check if it's a return with expression or just a return
    if (currentToken().token != TokenType::SEMICOLON) {
        node->addChild(parseExpression());
    }
    
    // Match semicolon
    if (!match(TokenType::SEMICOLON)) {
        std::cerr << "SYNTAX ERROR: Expected ; after return statement in Rule 17" << std::endl;
        syntaxError();
    }
    
    return node;
}

// Rule 18: expression := var = expression | simple-expression
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
            node->addChild(parseExpression());
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

// Updated Rule 19: var := ID | ID [ simple-expression ]
ASTNode* Parser::parseVar() {
    // Check for ID
    if (!match(TokenType::ID)) {
        std::cerr << "SYNTAX ERROR: Expected identifier at start of variable in Rule 19" << std::endl;
        syntaxError();
    }
    
    // Save the ID token for the var node
    Token idToken = tokens.at(currentTokenIndex - 1);
    
    // Check if the variable exists in the symbol table - use getStrVal() consistently
    Symbol* varSymbol = st.findSymbol(idToken.getStrVal());
    if (!varSymbol) {
        std::cerr << "SEMANTIC ERROR: Undeclared variable '" << idToken.getStrVal() << "' in Rule 19" << std::endl;
        syntaxError();
    }
    
    ASTNode* node = new ASTNode(ASTNodeType::VAR, &idToken);
    
    // Check if this is an array access
    if (match(TokenType::OBRACKET)) {
        // Make sure the variable is actually an array
        if (varSymbol->arrSize == -1) {
            std::cerr << "SEMANTIC ERROR: Variable '" << idToken.getStrVal() << "' is not an array in Rule 19" << std::endl;
            syntaxError();
        }
        
        // Parse the index simple-expression
        node->addChild(parseSimpleExpression());
        
        // Match closing bracket
        if (!match(TokenType::CBRACKET)) {
            std::cerr << "SYNTAX ERROR: Expected ] after array index expression in Rule 19" << std::endl;
            syntaxError();
        }
    } else if (varSymbol->arrSize != -1) {
        // If it's an array but not accessed with [], it might be a semantic error
        // For this compiler, we'll allow it but note it might be an issue
    }
    
    return node;
}

// Rule 20: simple-expression := additive-expression relop additive-expression | additive-expression
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

// Rule 21: relop := <= | < | > | >= | == | !=
ASTNode* Parser::parseRelOp() {
    Token opToken = currentToken();
    ASTNode* node = new ASTNode(ASTNodeType::REL_OP, &opToken);
    
    if (match(TokenType::LE) || match(TokenType::LT) || 
        match(TokenType::GT) || match(TokenType::GE) || 
        match(TokenType::EE) || match(TokenType::NE)) {
        // Successfully matched a relational operator
        return node;
    } else {
        std::cerr << "SYNTAX ERROR: Expected relational operator in Rule 21" << std::endl;
        syntaxError();
        return nullptr; // Unreachable
    }
}

// Rule 22: additive-expression := additive-expression addop term | term
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

// Rule 23: add-op := + | -
ASTNode* Parser::parseAddOp() {
    Token opToken = currentToken();
    ASTNode* node = new ASTNode(ASTNodeType::ADD_OP, &opToken);
    
    if (match(TokenType::PLUS) || match(TokenType::MINUS)) {
        // Successfully matched an additive operator
        return node;
    } else {
        std::cerr << "SYNTAX ERROR: Expected additive operator (+ or -) in Rule 23" << std::endl;
        syntaxError();
        return nullptr; // Unreachable
    }
}

// Rule 24: term := term mulop factor | factor
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

// Rule 25: mulop := * | /
ASTNode* Parser::parseMulOp() {
    Token opToken = currentToken();
    ASTNode* node = new ASTNode(ASTNodeType::MULOP, &opToken);
    
    if (match(TokenType::TIMES) || match(TokenType::DIVIDE)) {
        // Successfully matched a multiplicative operator
        return node;
    } else {
        std::cerr << "SYNTAX ERROR: Expected multiplicative operator (* or /) in Rule 25" << std::endl;
        syntaxError();
        return nullptr; // Unreachable
    }
}

// Rule 26: factor := ( simple-expression ) | var | call | NUM
ASTNode* Parser::parseFactor() {
    ASTNode* node = new ASTNode(ASTNodeType::FACTOR);
    
    switch (currentToken().token) {
        case TokenType::OPARENTHESES: {
            // ( simple-expression )
            match(TokenType::OPARENTHESES);
            node->addChild(parseSimpleExpression());
            
            if (!match(TokenType::CPARENTHESES)) {
                std::cerr << "SYNTAX ERROR: Expected ) after expression in factor in Rule 26" << std::endl;
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
        
        default:
            std::cerr << "SYNTAX ERROR: Unexpected token in factor in Rule 26" << std::endl;
            syntaxError();
    }
    
    return node;
}

// Rule 27: call := ID ( args )
ASTNode* Parser::parseCall() {
    // Check for function ID
    if (!match(TokenType::ID)) {
        std::cerr << "SYNTAX ERROR: Expected function identifier at start of call in Rule 27" << std::endl;
        syntaxError();
    }
    
    // Save the function ID token
    Token idToken = tokens.at(currentTokenIndex - 1);
    
    // Check if the function exists in the symbol table
    Symbol* funcSymbol = st.findSymbol(idToken.getStrVal());
    if (!funcSymbol || funcSymbol->type != SymbolType::SYMBOL_FUNCTION) {
        std::cerr << "SEMANTIC ERROR: Undeclared function '" << idToken.getStrVal() << "' in Rule 27" << std::endl;
        syntaxError();
    }
    
    ASTNode* node = new ASTNode(ASTNodeType::CALL, &idToken);
    
    // Match opening parenthesis
    if (!match(TokenType::OPARENTHESES)) {
        std::cerr << "SYNTAX ERROR: Expected ( after function identifier in Rule 27" << std::endl;
        syntaxError();
    }
    
    // Parse arguments
    node->addChild(parseArgs());
    
    // Match closing parenthesis
    if (!match(TokenType::CPARENTHESES)) {
        std::cerr << "SYNTAX ERROR: Expected ) after arguments in function call in Rule 27" << std::endl;
        syntaxError();
    }
    
    return node;
}

// Rule 28: args := arg-list | ε
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

// Rule 29: arg-list := arg-list , expression | expression
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