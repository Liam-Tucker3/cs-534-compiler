#include <stdio.h>
#include <string>
#include <cstring>
#include <iostream>
#include <fstream>
#include <algorithm>
#include <set>
#include <vector>

#include "token.h"
#include "ast.h"

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
        if (t.getToken() == OPARENTHESES) asts.push_back(AST(AST_OPARAN));
        else if (t.getToken() == CPARENTHESES) asts.push_back(AST(AST_CPARAN));
        else if (t.getToken() == INT) asts.push_back(AST(AST_INT, t.getIntVal()));
        else if (t.getToken() == PLUS) asts.push_back(AST(AST_PLUS));
        else if (t.getToken() == MINUS) asts.push_back(AST(AST_MINUS));
        else if (t.getToken() == TIMES) asts.push_back(AST(AST_TIMES));
        else if (t.getToken() == DIVIDE) asts.push_back(AST(AST_DIVIDE));
    }
    return asts;
}


std::pair<int, int> findParantheses(std::vector<AST> &asts) {
    for (int i = 0; i < asts.size(); i++) {
        if (*asts.at(i).type != AST_OPARAN) continue; // Not open parantheses

        // Case: open parantheses
        int o = 0; // Starting at index i
        int c = 0;
        for (int j = i; j < asts.size(); j++) {
            if (*asts.at(j).type == AST_OPARAN) o++;
            else if (*asts.at(j).type == AST_CPARAN) c++;
            if (o == c) return std::pair<int, int>(i, j); // Found matching parantheses
        }

    }
    return std::pair<int, int>(-1, -1); // Base case: no more parantheses
}

/* Returns index of next (, +, -, or -1)*/
/* An expr in parantheses will evaluate to a num, so this does not count, even if root is + or -*/
int findPlusMinusParan(std::vector<AST> *asts) {
    for (int i = 0; i < asts->size(); i++) {
        ASTType t = *asts->at(i).type;
        if (t == AST_OPARAN) return i;
        if (t == AST_PLUS || t == AST_MINUS) {
            if (asts->at(i).left == nullptr && asts->at(i).right == nullptr) return i;
        }
    }
    return -1;
}

AST makeTreeRecursive(std::vector<AST> &asts) {
    // Finding next item to reduce
    int i = findPlusMinusParan(&asts);
    if (i == -1) { // Base case: Only expr, *, and / remaining

        if (asts.size() == 1) return asts.at(0); // Base case: only one element
        else if (asts.size() == 3) { // Base case: 2nd element is * or /
            AST root = asts.at(1); // No children
            // root.val = AST_TREE; // Setting type to tree
            root.left = &asts.at(0); // No children
            std::vector<AST> rightAsts(asts.begin() + 2, asts.end());
            AST* rightSubTree = new AST(makeTreeRecursive(rightAsts));
            root.right = rightSubTree;
            return root;
        }
    }

    // Recursive case: parantheses
    if (*asts.at(i).type == AST_OPARAN) {
        // Getting AST for values inside parantheses
        std::pair<int, int> p = findParantheses(asts);
        std::vector<AST> paranAsts(asts.begin() + p.first + 1, asts.begin() + p.second);
        AST paranRoot = makeTreeRecursive(paranAsts);

        // Replacing all values from parantheses with new value
        std::vector<AST> newAsts;
        for (int j = 0; j < asts.size(); j++) {
            if (j < p.first || j > p.second) newAsts.push_back(asts.at(j));
            else if (j == p.first) newAsts.push_back(paranRoot);
        }

        return makeTreeRecursive(newAsts);
    }

    // Recursive case: + or -
    if (*asts.at(i).type == AST_PLUS || *asts.at(i).type == AST_MINUS) {
        AST root = asts.at(i);
        // root.val = AST_TREE; // Setting type to tree

        // Getting left, right portions of arraylist
        std::vector<AST> leftAsts(asts.begin(), asts.begin() + i);
        std::vector<AST> rightAsts(asts.begin() + i + 1, asts.end());
        AST* leftSubTree = new AST(makeTreeRecursive(leftAsts));
        AST* rightSubTree = new AST(makeTreeRecursive(rightAsts));
        root.left = leftSubTree;
        root.right = rightSubTree;
        
        return root;
    }

    else {
        std::cerr << "Error: Invalid ASTType" << std::endl;
        return AST(AST_NULL);
    }
}

/* Returns 1 if expr is valid, 0 if not*/
int validateExpr(std::vector<AST> asts) {
    int pCount = 0;
    std::string nextType = "int";
    for (int i = 0; i < asts.size(); i++) {
        ASTType t = *asts.at(i).type;

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

std::string astElemToCode(AST a) {
    if (*a.type == AST_INT) return "LOAD(" + std::to_string(a.val) + ");";
    else if (*a.type == AST_PLUS) return "ADD();";
    else if (*a.type == AST_MINUS) return "SUB();";
    else if (*a.type == AST_TIMES) return "MUL();";
    else if (*a.type == AST_DIVIDE) return "DIV();";
    else {
        std::cerr << "Error: Invalid ASTType" << std::endl;
        return "";
    }
}

void getStackMachineCode(AST root, std::vector<std::string> &code) {
    if (root.left != nullptr) getStackMachineCode(*root.left, code);
    if (root.right != nullptr) getStackMachineCode(*root.right, code);
    code.push_back(astElemToCode(root));
}

void printStackMachineCodeToFile(std::vector<std::string> code, std::string filename) {
    std::ofstream file(filename);
    for (std::string s : code) file << s << std::endl;
    file.close();
}


int main(int argc, char *argv[]) {
    
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
    

    // AST WORK
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
    std::string input;
    std::string line;
    while (std::getline(file, line)) {
        input += line;
        lineIndices.push_back(input.length());
    }
    file.close();

    std::cout << "Starting AST WORK" << std::endl;
    // std::string astExpr = "10 + (8 - 6 / 4) * 2";
    std::vector<Token> astTokens;
    parseInput(input, astTokens, lineIndices);
    std::cout << "Parsed AST INPUT" << std::endl;
    std::vector<AST> asts = tokensToAST(astTokens);
    std::cout << "Called Tokens to AST" << std::endl;
    for (const auto& ast : asts) {
        std::cout << "AST Type: " << *ast.type << ", Value: " << ast.val << std::endl;
    }
    AST root = makeTreeRecursive(asts);
    std::cout << "Called makeTreeRecursive" << std::endl;
    std::vector<std::string> code;
    getStackMachineCode(root, code);
    std::cout << "Got stack machine code" << std::endl;
    printStackMachineCodeToFile(code, "output.txt");
    std::cout << "Printed stack machine code to file" << std::endl;

    return 0;
}