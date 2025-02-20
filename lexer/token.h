#ifndef TOKEN_H
#define TOKEN_H

#include <string>

enum TokenType {
    PLUS, MINUS, TIMES, DIVIDE, // Basic arithmetic
    PLUSEQUALS, MINUSEQUALS, TIMESEQUALS, DIVIDEEQUALS, // Compound assignment
    PLUSPLUS, MINUSMINUS, // Increment, decrement
    LE, GE, LT, GT, EE, NE, NOT, EQUALS, // Comparison
    COMMA, SEMICOLON, COLON,
    OPARENTHESES, CPARENTHESES, OCURLY, CCURLY, OBRACKET, CBRACKET,
    UNKNOWN
};

class Token {
private:
    // Attributes
    TokenType token;
    std::string filename;
    int line;
    int index;

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
};

#endif // TOKEN_H