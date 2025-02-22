#ifndef TOKEN_H
#define TOKEN_H

#include <string>

// TO ADD
// <, >, ::, **=, %=, #, ^=, <<, >>, <<=, >>=, ~, ->
// class, private, std, string, include, int, char, void, bool, if, else, for, while, do, switch, case, default, return, break, continue, true, false, nullptr, new, delete, this, static, virtual, friend, const, inline, template, typename, namespace, using, public, protected, operator, sizeof, auto, register, extern, volatile, mutable, explicit, typedef, struct, union, enum, goto, try, catch, throw, volatile, asm, __asm, __asm__, __volatile__, __volatile, __volatile__, __volatile, __volatile__,
// numbers
// strings

enum TokenType {
    PLUS, MINUS, TIMES, DIVIDE, POWER, MOD, // Basic arithmetic
    PLUSEQUALS, MINUSEQUALS, TIMESEQUALS, DIVIDEEQUALS, // Compound assignment
    PLUSPLUS, MINUSMINUS, // Increment, decrement
    LE, GE, LT, GT, EE, NE, NOT, EQUALS, // Comparison
    COMMA, SEMICOLON, COLON, DOT, SLASH, BACKSLASH, QUESTION, // Punctuation
    OPARENTHESES, CPARENTHESES, OCURLY, CCURLY, OBRACKET, CBRACKET, // Braces
    AND, BITAND, OR, BITOR, XOR, XNOT, // Bitwise
    LINECOMMENT, BLOCKCOMMENTOPEN, BLOCKCOMMENTCLOSE, // Comments
    INT, // Types
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