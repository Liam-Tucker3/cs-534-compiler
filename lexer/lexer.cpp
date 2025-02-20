#include <stdio.h>
#include <string>
#include <cstring>
#include <iostream>
#include <algorithm>
#include <set>
#include "token.h"

/*
enum TokenType {
    PLUS, MINUS, TIMES, DIVIDE, // Basic arithmetic
    PLUSEQUALS, MINUSEQUALS, TIMESEQUALS, DIVIDEEQUALS, // Compound assignment
    PLUSPLUS, MINUSMINUS, // Increment, decrement
    LE, GE, LT, GT, EE, NE, NOT, EQUALS, // Comparison
    COMMA, SEMICOLON, COLON,
    OPARENTHESES, CPARENTHESES, OCURLY, CCURLY, OBRACKET, CBRACKET,
    UNKNOWN
};
*/

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

    // Punctuation
    if (ch == ',') return Token(COMMA, index); 
    if (ch == ';') return Token(SEMICOLON, index);
    if (ch == ':') return Token(COLON, index);

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
        else return Token(TIMES, index);
    }
    if (ch == '/')  {
        if (input[index+1] == '=') return Token(DIVIDEEQUALS, index);
        else return Token(DIVIDE, index);
    }

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

    return Token(UNKNOWN, index);
}


/* Iterates through input, finds delimiters, and calls function to identify input*/
void parseInput(const std::string &input) {
    size_t i = 0;
    while (i < input.length()) {
        char ch = input[i];
        if (delimiters.find(ch) != delimiters.end()) {
            Token t = identifyToken(input, i);
            std::cout << t.getIndex() << " | " << t.toString() << std::endl;
            i += t.getLength(); // Increment by length of token
        }
        else i++; // Increment by 1
    }
}


int main() {
    std::string input = "+-*/<=<=>==!=<>";
    parseInput(input);
    return 0;
}