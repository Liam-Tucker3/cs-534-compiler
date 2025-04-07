#include <stdio.h>
#include <string>
#include <cstring>
#include <iostream>
#include <fstream>
#include <algorithm>
#include <set>
#include <vector>
#include<functional>

#include "token.h"
#include "ast.h"

// Forward declaration for astElemToCode to allow it to be used with const AST&
// Based on suggestion from Claude
std::string astElemToCode(const AST& a);

// Not currently used
std::set<char> delimiters = {
    '(', ')', '{', '}', '[', ']', // Braces
    ',', ';', ':', // Punctuation
    '\'', '\"', '`', // Quotes
    '+', '-', '*', '/', // Operators
    '<', '>', '=', '!' // Comparison operators
};

// Identifies next token from input
// LL parser; no backtracking
Token identifyToken(const std::string &input, int index) {
    char ch = input[index];
    // std::cout << "index: " << index << " | ch: " << ch << std::endl;

    // Punctuation
    if (ch == ',') return Token(COMMA, index); 
    if (ch == ';') return Token(SEMICOLON, index);
    if (ch == ':') return Token(COLON, index);
    if (ch == '.') return Token(DOT, index);
    if (ch == '?' ) return Token(QUESTION, index);
    if (ch == '/') { // Can be divide, comment, or backslash
        if (input[index+1] == '=') return Token(DIVIDEEQUALS, index); // Divide equals
        else if (input[index+1] != '/' && input[index+1] != '*') return Token(DIVIDE, index); // Divide
        else { // Comment
            if (input[index+1] == '/') { // Line comment: lasts until next newline
                int s = index; // Start index
                int e = index+2; // End index
                while (input[e] != '\n') e++;
                Token t = Token(LINECOMMENT, s);
                t.setVal(input.substr(s+2, e-(s+2))); // Storing value of s
                return t;
            }
            else if (input[index+1] == '*') { // Block comment: lasts until next '*/'
                int s = index; // Start index
                int e = index+2; // End index
                while (input[e] != '*' || input[e+1] != '/') e++;
                Token t = Token(BLOCKCOMMENT, s);
                t.setVal(input.substr(s+2, e-(s+2))); // Storing value of s
                return t;
            }
        }
    }
    if (ch == '\\') return Token(BACKSLASH, index); // TODO: Fix
    
    // Quotes
    if (ch == '"') { // STRING
        int s = index; // Start index
        int e = index+1; // End index
        while (input[e] != '"') e++;
        Token t = Token(STRING, s);
        t.setVal(input.substr(s+1, e-(s+1))); // Excluding quotation marks from stored value
        return t;
    }
    if (ch == '\'') { // CHAR
        int s = index; // Start index
        int e = index+1; // End index
        while (input[e] != '\'') e++;
        Token t = Token(CHAR, s);
        t.setVal(input.substr(s+1, e-(s+1))); // Excluding quotation marks from stored value
        return t;
    }

    // Braces
    if (ch == '(') return Token(OPARENTHESES, index);
    if (ch == ')') return Token(CPARENTHESES, index);
    if (ch == '{') return Token(OCURLY, index);
    if (ch == '}') return Token(CCURLY, index);
    if (ch == '[') return Token(OBRACKET, index);
    if (ch == ']') return Token(CBRACKET, index);

    // Operators
    if (ch == '+') {
        if (input[index+1] == '=') return Token(PLUSEQUALS, index);
        else if (input[index+1] == '+') return Token(PLUSPLUS, index);
        else return Token(PLUS, index);
    }
    if (ch == '-') {
        if (input[index+1] == '=') return Token(MINUSEQUALS, index);
        else if (input[index+1] == '-') return Token(MINUSMINUS, index);
        else return Token(MINUS, index);
    }
    if (ch == '*') {
        if (input[index+1] == '=') return Token(TIMESEQUALS, index);
        else if (input[index+1] == '*') return Token(POWER, index);
        else return Token(TIMES, index);
    }
    if (ch == '%') return Token(MOD, index);

    // Comparison operators
    if (ch == '<') {
        if (input[index+1] == '=') return Token(LE, index);
        else return Token(LT, index);
    }
    if (ch == '>') {
        if (input[index+1] == '=') return Token(GE, index);
        else return Token(GT, index);
    }
    if (ch == '=') {
        if (input[index+1] == '=') return Token(EE, index);
        else return Token(EQUALS, index);
    }
    if (ch == '!') {
        if (input[index+1] == '=') return Token(NE, index);
        else return Token(NOT, index);
    }

    // AND, OR
    if (ch == '&') {
        if (input[index+1] == '&') return Token(AND, index);
        else return Token(BITAND, index);
    }
    if (ch == '|') {
        if (input[index+1] == '|') return Token(OR, index);
        else return Token(BITOR, index);
    }
    if (ch == '^') return Token(XOR, index);
    if (ch == '~') return Token(XNOT, index);

    // INT
    if (std::isdigit(ch)) {
        int s = index; // Start index
        int e = index; // End index
        while (isdigit(input[e])) e++;
        Token t = Token(INT, s);
        t.setVal(std::stoi(input.substr(s, e-s))); // Storing value of s
        return t;
    }

    // VAR, KEYWORD
    if (std::isalpha(ch)) {
        // Getting string
        int s = index; // Start index
        int e = index; // End index
        while (isalpha(input[e]) || std::isdigit(input[e]) || '_' == input[e]) e++;
        std::string val = input.substr(s, e-s);

        // Checking if keyword or variable name
        if (Token::keywords.find(val) != Token::keywords.end()) { // Case: Keyword
            Token t = Token(KEYWORD, s);
            t.setVal(val); // Storing value of s
            return t;
        } else { // Case: VAR
            Token t = Token(VAR, s);
            t.setVal(input.substr(s, e-s)); // Storing value of s
            return t;
        }
    }
    
    // Check for whitespace
    if (std::isspace(ch)) return Token(WHITESPACE, index);

    return Token(UNKNOWN, index);
}

/* Returns next Token */
Token lex(const std::string &input, std::vector<int> &lineIndices, int index) {
    Token t = identifyToken(input, index);
            
    // Finding line num, relative index
    auto it = std::upper_bound(lineIndices.begin(), lineIndices.end(), index);
    int lineNum = std::distance(lineIndices.begin(), it); // No -1 to adjust to 1-based indexing
    int relIndex = index - lineIndices[lineNum-1] + 1; // Adjusting to 1-based indexing
    t.setIndices(lineNum+1, relIndex); // Updating indexes

    return t;

}


/* Iterates through input, finds delimiters, and calls function to identify input*/
void parseInput(const std::string &input, std::vector<Token> &tokens, std::vector<int> &lineIndices) {
    size_t i = 0;
    // Iterating through characters
    while (i < input.length()) {
        char ch = input[i];
        if (true) { // TODO: refactor
            // Creating token
            Token t = identifyToken(input, i);

            // Finding line num, relative index
            auto it = std::upper_bound(lineIndices.begin(), lineIndices.end(), i);
            int lineNum = std::distance(lineIndices.begin(), it); // No -1 to adjust to 1-based indexing
            int relIndex = i - lineIndices[lineNum-1] + 1; // Adjusting to 1-based indexing
            t.setIndices(lineNum+1, relIndex); // Updating indexes

            tokens.push_back(t); // Adding token to vector
            i += t.getLength(); // Increment by length of token
        }
        else i++; // Increment by 1
    }
}

// Prints list of tokens
void printTokens(const std::vector<Token> &tokens) {
    for (Token t : tokens) {
        if (t.getToken() == WHITESPACE) continue;
        std::cout << "Line " << t.getLine() << " Char " << t.getIndex() << " | " << t.toString() << std::endl;
    }
}

/* CODE BELOW FOR LL - RDP*/
/* Converts vector of tokens into vector of AST elements*/
std::vector<AST> tokensToAST(std::vector<Token> &tokens) {
    std::vector<AST> asts;
    for (int i = 0; i < tokens.size(); i++) {
        Token t = tokens.at(i);
        AST *thisAST;
        if (t.getToken() == OPARENTHESES) thisAST = new AST(AST_OPARAN);
        else if (t.getToken() == CPARENTHESES) thisAST = new AST(AST_CPARAN);
        else if (t.getToken() == INT) thisAST = new AST(AST_INT, t.getIntVal());
        else if (t.getToken() == PLUS) thisAST = new AST(AST_PLUS);
        else if (t.getToken() == MINUS) thisAST = new AST(AST_MINUS);
        else if (t.getToken() == TIMES) thisAST = new AST(AST_TIMES);
        else if (t.getToken() == DIVIDE) thisAST = new AST(AST_DIVIDE);
        else thisAST = new AST(AST_NULL);
        asts.push_back(*thisAST);
    }
    return asts;
}


std::pair<int, int> findParantheses(std::vector<AST> &asts) {
    for (int i = 0; i < asts.size(); i++) {
        if (asts.at(i).type != AST_OPARAN) continue; // Not open parantheses

        // Case: open parantheses
        int o = 0; // Starting at index i
        int c = 0;
        for (int j = i; j < asts.size(); j++) {
            if (asts.at(j).type == AST_OPARAN) o++;
            else if (asts.at(j).type == AST_CPARAN) c++;
            if (o == c) return std::pair<int, int>(i, j); // Found matching parantheses
        }

    }
    return std::pair<int, int>(-1, -1); // Base case: no more parantheses
}

/* Returns index of next (, +, -, or -1)*/
/* An expr in parantheses will evaluate to a num, so this does not count, even if root is + or -*/
int findPlusMinusParan(std::vector<AST> *asts) {
    for (int i = 0; i < asts->size(); i++) {
        ASTType t = asts->at(i).type;
        if (t == AST_OPARAN) return i;
        if (t == AST_PLUS || t == AST_MINUS) {
            if (asts->at(i).left == nullptr && asts->at(i).right == nullptr) return i;
        }
    }
    return -1;
}

/* Prints tree to stdout in understandable format */
void printTree(AST* root) {
    std::function<void(AST*, int)> printNode = [&](AST* node, int depth) {
        if (node == nullptr || node->type == AST_NULL) return;
        std::cout << std::string(depth * 2, ' ') << node->typeToString() << " (" << node->val << ")" << std::endl;
        std::cout << "L: ";
        printNode(node->left, depth + 1);
        std::cout << "R: ";
        printNode(node->right, depth + 1);
    };

    printNode(root, 0);
}
void printTreeToFile(AST* root, const std::string& filename) {
    std::ofstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Error: Could not open file " << filename << std::endl;
        return;
    }

    std::function<void(AST*, int)> printNode = [&](AST* node, int depth) {
        if (node == nullptr || node->type == AST_NULL) return;
        file << std::string(depth * 2, ' ') << node->typeToString() << " (" << node->val << ")" << std::endl;
        printNode(node->left, depth + 1);
        printNode(node->right, depth + 1);
    };

    printNode(root, 0);
    file.close();
}



// Recursive function to construct AST from vercot of individual AST nodes
AST* makeTreeRecursive(const std::vector<AST>& asts) {
    if (asts.empty()) {
        return new AST(AST_NULL);
    }
    
    // Base case: only one element
    if (asts.size() == 1) {
        return new AST(asts[0]);
    }
    
    // Find the first +, -, or parenthesis
    int i = -1;
    for (int j = 0; j < asts.size(); j++) {
        ASTType t = asts[j].type;
        if (t == AST_OPARAN || (j > 0 && (t == AST_PLUS || t == AST_MINUS))) {
            i = j;
            break;
        }
    }
    
    if (i == -1) {
        // Only have *, / operators or a single term
        if (asts.size() == 1) {
            return new AST(asts[0]);
        }
        else if (asts.size() >= 3 && (asts[1].type == AST_TIMES || asts[1].type == AST_DIVIDE)) {
            // Handle multiplication or division
            AST* root = new AST(asts[1]);
            
            // Create left subtree
            std::vector<AST> leftAsts(asts.begin(), asts.begin() + 1);
            root->left = makeTreeRecursive(leftAsts);
            
            // Create right subtree
            std::vector<AST> rightAsts(asts.begin() + 2, asts.end());
            root->right = makeTreeRecursive(rightAsts);
            
            return root;
        }
    }
    
    // Handle parentheses
    if (i >= 0 && asts[i].type == AST_OPARAN) {
        // Find matching closing parenthesis
        int openCount = 1;
        int closeIndex = i + 1;
        while (openCount > 0 && closeIndex < asts.size()) {
            if (asts[closeIndex].type == AST_OPARAN) {
                openCount++;
            } else if (asts[closeIndex].type == AST_CPARAN) {
                openCount--;
            }
            if (openCount > 0) {
                closeIndex++;
            }
        }
        
        // ERROR HANDLING
        if (openCount != 0 || closeIndex >= asts.size()) {
            std::cerr << "Error: Mismatched parentheses" << std::endl;
            return new AST(AST_NULL);
        }
        
        // Generating AST for expr inside parantheses
        std::vector<AST> paranAsts(asts.begin() + i + 1, asts.begin() + closeIndex);
        AST* paranRoot = makeTreeRecursive(paranAsts);
        
        // Replacing parantheses with new AST
        std::vector<AST> newAsts;
        for (int j = 0; j < i; j++) {
            newAsts.push_back(asts[j]);
        }
        
        // Converting AST* to AST for vector
        AST paranValue = *paranRoot;
        newAsts.push_back(paranValue);
        
        for (int j = closeIndex + 1; j < asts.size(); j++) {
            newAsts.push_back(asts[j]);
        }
        
        // Process the new expression
        AST* result = makeTreeRecursive(newAsts);
        
        // Memory management
        if (paranRoot != result) {
            delete paranRoot;
        }
        
        return result;
    }
    
    // Handle + or - operators
    if (i >= 0 && (asts[i].type == AST_PLUS || asts[i].type == AST_MINUS)) {
        AST* root = new AST(asts[i]);
        
        // Create left subtree from everything before the operator
        std::vector<AST> leftAsts(asts.begin(), asts.begin() + i);
        root->left = makeTreeRecursive(leftAsts);
        
        // Create right subtree from everything after the operator
        std::vector<AST> rightAsts(asts.begin() + i + 1, asts.end());
        root->right = makeTreeRecursive(rightAsts);
        
        return root;
    }
    
    // Error Handling
    std::cerr << "Error: Invalid expression" << std::endl;
    return new AST(AST_NULL);
}

/* Returns 1 if expr is valid, 0 if not*/
int validateExpr(std::vector<AST> asts) {
    int pCount = 0;
    std::string nextType = "int";
    for (int i = 0; i < asts.size(); i++) {
        ASTType t = asts.at(i).type;

        if (nextType == "int") {
            if (t != AST_INT && t != AST_OPARAN && t != AST_CPARAN) return -1; // op in invalid location
            else if (t == AST_INT) nextType = "op";
            else if (t == AST_OPARAN) {
                nextType = "int";
                pCount ++;
            }
            else { // if t == AST_CPARAN
                nextType = "op";
                pCount--;
                if (pCount < 0) return -1; // Too many closing parantheses
            }
        }
        else if (nextType == "op") {
            if (t != AST_PLUS && t != AST_MINUS && t != AST_TIMES && t != AST_DIVIDE) return -1;
            else nextType = "int";
        }
    }
    if (pCount != 0) return -1; // Mismatched parantheses
    return 1; // Valid expression
}


// Generates stack machine code from completed AST
void getStackMachineCode(const AST& root, std::vector<std::string>& code) {
    // Use post-order traversal (left, right, root) for stack machine code
    if (root.left != nullptr) {
        getStackMachineCode(*root.left, code);
    }
    
    if (root.right != nullptr) {
        getStackMachineCode(*root.right, code);
    }
    
    // Add the operation/value for this node
    std::string instruction = astElemToCode(root);
    if (!instruction.empty()) {
        code.push_back(instruction);
    }
}

// Coverts AST Node to stack machine code
std::string astElemToCode(const AST& a) {
    switch (a.type) {
        case AST_INT:
            return "LOAD(" + std::to_string(a.val) + ");";
        case AST_PLUS:
            return "ADD();";
        case AST_MINUS:
            return "SUB();";
        case AST_TIMES:
            return "MUL();";
        case AST_DIVIDE:
            return "DIV();";
        case AST_NULL:
            return ""; // Don't generate code for null nodes
        default:
            std::cerr << "Warning: Unrecognized AST node type: " << a.typeToString() << std::endl;
            return "";
    }
}

// Writes stack machine code to file
void printStackMachineCodeToFile(std::vector<std::string> code, std::string filename) {
    std::ofstream file(filename);
    for (std::string s : code) file << s << std::endl;
    file.close();
}


int main(int argc, char *argv[]) {
    
    /* Code to use executable as lexer */
    // Confirming proper number of arguments
    // if (argc != 2) {
    //     std::cerr << "Usage: " << argv[0] << " <filename>" << std::endl;
    //     return 1;
    // }

    // // Opening file
    // std::ifstream file(argv[1]);
    // if (!file.is_open()) {
    //     std::cerr << "Error: Could not open file " << argv[1] << std::endl;
    //     return 1;
    // }

    // // Reading file into string
    // std::vector<int> lineIndices; // lineIndices.at(x) = index of first character of line x+1
    // std::string input;
    // std::string line;
    // while (std::getline(file, line)) {
    //     input += line;
    //     lineIndices.push_back(input.length());
    // }
    // file.close();

    // // Printing lineIndices
    // for (int i : lineIndices) std::cout << i << std::endl;

    // // Hardcoding input
    // // input = "+-*/ <=class<=>==!=<for >/* int**//123?.45 apublic //publica public\n ab_cD12094 3a. 'abc' \"def\"";

    // // Performing process
    // std::vector<Token> tokens;
    // parseInput(input, tokens, lineIndices);
    // printTokens(tokens);
    

    /* Code to use executable as parser */
    // Confirming proper number of arguments
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " <filename>" << std::endl;
        return 1;
    }

    // Opening file
    std::ifstream file(argv[1]);
    if (!file.is_open()) {
        std::cerr << "Error: Could not open file " << argv[1] << std::endl;
        return 1;
    }

    // Reading file into string
    std::vector<int> lineIndices; // lineIndices.at(x) = index of first character of line x+1
    lineIndices.push_back(0); // Add starting position
    std::string input;
    std::string line;
    while (std::getline(file, line)) {
        input += line + "\n"; // Make sure to add newline character
        lineIndices.push_back(input.length());
    }
    file.close();

    // Parse the input into tokens
    std::vector<Token> astTokens;
    parseInput(input, astTokens, lineIndices);

    // Filter out whitespace tokens
    std::vector<Token> filteredTokens;
    for (const Token& t : astTokens) {
        if (t.getToken() != WHITESPACE && t.getToken() != LINECOMMENT && t.getToken() != BLOCKCOMMENT) {
            filteredTokens.push_back(t);
        }
    }

    // Convert tokens to AST nodes
    std::vector<AST> asts = tokensToAST(filteredTokens);

    // Remove NULL nodes
    asts.erase(std::remove_if(asts.begin(), asts.end(), 
            [](AST &ast) { return ast.type == AST_NULL; }), asts.end());

    // Display AST nodes for debugging
    // for (auto& ast : asts) {
    //     std::cout << "AST Type: " << ast.type << " (" << ast.typeToString() << ") | Value: " << ast.val << std::endl;
    // }

    // Build the AST
    AST* root = makeTreeRecursive(asts);

    // Generating stack machine code if AST is valid
    if (root && root->type != AST_NULL) {
        std::vector<std::string> code;
        getStackMachineCode(*root, code);
        
        // Write code to file
        printStackMachineCodeToFile(code, "output.txt");
        std::cout << "Printed stack machine code to file output.txt" << std::endl;
        
        // Memory management
        delete root;
    } else {
        std::cerr << "Error: Failed to build valid AST" << std::endl;
        delete root; // Memory management
    }

    return 0;

}