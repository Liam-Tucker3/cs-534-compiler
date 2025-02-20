#include "token.h"

// Constructors
Token::Token() {
    token = UNKNOWN;
    this->filename = "";
    this->line = -1;
    this->index = -1;
}

Token::Token(TokenType t, int index) {
    token = t;
    this->filename = "";
    this->line = -1;
    this->index = index;

}

Token::Token(TokenType t, std::string filename, int line, int index) {
    token = t;
    this->filename = filename;
    this->line = line;
    this->index = index;
}

// Public Methods
TokenType Token::getToken() {
    return token;
}

std::string Token::getFilename() {
    return filename;
}

int Token::getLine() {
    return line;
}

int Token::getIndex() {
    return index;
}

int Token::getLength() {
    TokenType t = this->token;

    if (t == PLUS || t == MINUS || t == TIMES || t == DIVIDE) return 1; // Arithmetic
    else if (t == PLUSEQUALS || t == MINUSEQUALS || t == TIMESEQUALS || t == DIVIDEEQUALS) return 2; // Compound assignment
    else if (t == PLUSPLUS || t == MINUSMINUS) return 2; // Increment, decrement
    else if (t == LE || t == GE || t == EE || t == NE) return 2; // Comparison, 2 char
    else if (t == LT || t == GT || t == NOT || t == EQUALS) return 1; // Comparison, 1 char
    else if (t == COMMA || t == SEMICOLON || t == COLON) return 1; // Punctuation
    else if (t == OPARENTHESES || t == CPARENTHESES || t == OCURLY || t == CCURLY || t == OBRACKET || t == CBRACKET) return 1; // Braces
    else return 0; // Unknown 
}
   
std::string Token::toString() {
    std::string tokenString = "";
    switch (token) {
        case PLUS:
            tokenString = "PLUS";
            break;
        case MINUS:
            tokenString = "MINUS";
            break;
        case TIMES:
            tokenString = "TIMES";
            break;
        case DIVIDE:
            tokenString = "DIVIDE";
            break;
        case PLUSEQUALS:
            tokenString = "PLUSEQUALS";
            break;
        case MINUSEQUALS:
            tokenString = "MINUSEQUALS";
            break;
        case TIMESEQUALS:
            tokenString = "TIMESEQUALS";
            break;
        case DIVIDEEQUALS:
            tokenString = "DIVIDEEQUALS";
            break;
        case PLUSPLUS:
            tokenString = "PLUSPLUS";
            break;
        case MINUSMINUS:
            tokenString = "MINUSMINUS";
            break;
        case LE:
            tokenString = "LE";
            break;
        case GE:
            tokenString = "GE";
            break;
        case LT:
            tokenString = "LT";
            break;
        case GT:
            tokenString = "GT";
            break;
        case EE:
            tokenString = "EE";
            break;
        case NE:
            tokenString = "NE";
            break;
        case NOT:
            tokenString = "NOT";
            break;
        case EQUALS:
            tokenString = "EQUALS";
            break;
        case COMMA:
            tokenString = "COMMA";
            break;
        case SEMICOLON:
            tokenString = "SEMICOLON";
            break;
        case COLON:
            tokenString = "COLON";
            break;
        case OPARENTHESES:
            tokenString = "OPARENTHESES";
            break;
        case CPARENTHESES:
            tokenString = "CPARENTHESES";
            break;
        case OCURLY:
            tokenString = "OCURLY";
            break;
        case CCURLY:
            tokenString = "CCURLY";
            break;
        case OBRACKET:
            tokenString = "OBRACKET";
            break;
        case CBRACKET:
            tokenString = "CBRACKET";
            break;
        case UNKNOWN:
            tokenString = "UNKNOWN";
            break;
        }
        return tokenString;
    }
