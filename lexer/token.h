#ifndef TOKEN_H
#define TOKEN_H

#include <string>
#include <set>

// TO ADD
// ::, **=, %=, #, ^=, <<, >>, <<=, >>=, ~, ->

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
    UNKNOWN
};

class Token {
private:
    // Attributes
    TokenType token;
    std::string filename;
    int line;
    int index;

    // Simple implementation: only one of these will be used
    int iVal;
    std::string sVal;

    // Methods

public:
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
    std::string toString() const;
        
    int getIntVal() const; 
    std::string getStrVal() const;

    void setVal(int val);
    void setVal(std::string val);
    void setIndices(int line, int index);
};

#endif // TOKEN_H