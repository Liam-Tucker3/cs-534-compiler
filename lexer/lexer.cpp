#include <stdio.h>
#include <string>
#include <cstring>
#include <iostream>
#include <algorithm>
#include <set>
#include "token.h"

std::set<char> delimiters = {
    '(', ')', '{', '}', '[', ']', // Braces
    ',', ';', ':', // Punctuation
    '\'', '\"', '`', // Quotes
    '+', '-', '*', '/', // Operators
    '<', '>', '=', '!' // Comparison operators
};

/*
std::set<std::string, TokenType> keywords = {
    {"+", PLUS},
    {"-", MINUS},
    {"*", TIMES},
    {"/", DIVIDE},
    {"<=", LE},
    {">=", GE},
    {"<", LT},
    {">", GT}
};
*/


Token identifyToken(const std::string &input, int index) {
    char ch = input[index];
    // std::cout << "index: " << index << " | ch: " << ch << std::endl;

    // Punctuation
    if (ch == ',') return Token(COMMA, index); 
    if (ch == ';') return Token(SEMICOLON, index);
    if (ch == ':') return Token(COLON, index);
    if (ch == '.') return Token(DOT, index);
    if (ch == '?' ) return Token(QUESTION, index);
    if (ch == '/') {
        if (input[index+1] == '/') return Token(LINECOMMENT, index);
        else if (input[index+1] == '*') return Token(BLOCKCOMMENTOPEN, index);
        else return Token(SLASH, index);
    }
    if (ch == '\\') return Token(BACKSLASH, index); // TODO: Fix

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
        else if (input[index+1] == '/') return Token(BLOCKCOMMENTCLOSE, index);
        else return Token(TIMES, index);
    }
    if (ch == '/')  {
        if (input[index+1] == '=') return Token(DIVIDEEQUALS, index);
        else return Token(DIVIDE, index);
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
    // std::cout << ch << " isdigit " << std::isdigit(ch) << std::endl;
    if (std::isdigit(ch)) {
        int s = index; // Start index
        int e = index; // End index
        while (isdigit(input[e])) e++;
        Token t = Token(INT, s);
        t.setVal(std::stoi(input.substr(s, e-s))); // Storing value of s
        return t;
    }

    return Token(UNKNOWN, index);
}


/* Iterates through input, finds delimiters, and calls function to identify input*/
void parseInput(const std::string &input) {
    size_t i = 0;
    // std::cout << input.length() << std::endl;
    while (i < input.length()) {
        char ch = input[i];
        // std::cout << "PARse input: index " << i << " | ch: " << ch << std::endl;
        if (true) {
            Token t = identifyToken(input, i);
            std::cout << t.getIndex() << " | " << t.toString() << std::endl;
            i += t.getLength(); // Increment by length of token
        }
        else i++; // Increment by 1
    }
}


int main() {
    std::string input = "+-*/<=<=>==!=<>/***//123?.45";
    parseInput(input);
    return 0;
}