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
    OPARENTHESES, CPARENTHESES, OCURLY, CCURLY, OBRACKET, CBRACKET, // Braces
    AND, BITAND, OR, BITOR, XOR, XNOT, // Bitwise
    LINECOMMENT, BLOCKCOMMENTOPEN, BLOCKCOMMENTCLOSE, // Comments
    INT, VAR, KEYWORD, // Types
    WHITESPACE, //Whitespace
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
    TokenType getToken();
    std::string getFilename();
    int getLine();
    int getIndex();
    int getLength();
    std::string toString();

    void setVal(int val);
    void setVal(std::string val);
};

#endif // TOKEN_H