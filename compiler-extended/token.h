#ifndef TOKEN_H
#define TOKEN_H

#include <string>
#include <set>

#include <iostream>
#include <ostream>

enum TokenType {
    PLUS, MINUS, TIMES, DIVIDE, POWER, MOD, // Basic arithmetic
    PLUSEQUALS, MINUSEQUALS, TIMESEQUALS, DIVIDEEQUALS, // Compound assignment
    PLUSPLUS, MINUSMINUS, // Increment, decrement
    LE, GE, LT, GT, EE, NE, NOT, EQUALS, // Comparison
    COMMA, SEMICOLON, COLON, DOT, SLASH, BACKSLASH, QUESTION, // Punctuation
    CHAR, STRING, // Quotes
    OPARENTHESES, CPARENTHESES, OCURLY, CCURLY, OBRACKET, CBRACKET, // Braces
    AND, BITAND, OR, BITOR, XOR, XNOT, // Bitwise
    INT, VAR, KEYWORD, // Types
    WHITESPACE, //Whitespace
    LINECOMMENT, BLOCKCOMMENT, // Seperate comments
    UNKNOWN, END_OF_FILE, // ENDOFFILE
    VOID, ID, NUM, IF,ELSE, WHILE, RETURN, INPUT, OUTPUT, FLOAT_TYPE, FLOAT_VAL // NEEDED TO ADD
};

class Token {
private:
    // Attributes
    std::string filename;
    int line;
    int index;

    // Simple implementation: only one of these will be used
    int iVal;
    std::string sVal;

    // Methods

public:
    // Attributes
    TokenType token; // Type of token

    // Class variable
    static std::set<std::string> keywords;

    // Constructors
    Token();
    Token(TokenType t, int index); // For scaffolding
    Token(TokenType t, std::string filename, int line, int index);

    // Methods
    // Functions are constant to avoid potential run time errors
    TokenType getToken() const;
    std::string getFilename() const; 
    int getLine() const;
    int getIndex() const;
    int getLength() const; 
    std::string toString() const; // For getting string representation of token type
        
    int getIntVal() const; 
    std::string getStrVal() const; // For when token is a string or variable and you want the value being stored

    void setVal(int val);
    void setVal(std::string val);
    void setIndices(int line, int index);

    void printError();
};

#endif // TOKEN_H