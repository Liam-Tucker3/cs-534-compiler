#include <stdio.h>
#include <string>
#include <cstring>
#include <iostream>
#include <fstream>
#include <algorithm>
#include <set>
#include <vector>
#include "token.h"

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

int main(int argc, char *argv[]) {
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

    // Printing lineIndices
    for (int i : lineIndices) std::cout << i << std::endl;

    // Hardcoding input
    // input = "+-*/ <=class<=>==!=<for >/* int**//123?.45 apublic //publica public\n ab_cD12094 3a. 'abc' \"def\"";

    // Performing process
    std::vector<Token> tokens;
    parseInput(input, tokens, lineIndices);
    printTokens(tokens);

    return 0;
}