#include "token.h"

// Static variables
std::set<std::string> Token::keywords = {"main", "class", "for", "private", "std", "int", "string", "char", "bool", "void", "if", "else", "while", "do", "switch", "case", "default", "return", "break", "continue", "true", "false", "nullptr", "new", "delete", "this", "static", "virtual", "friend", "const", "inline", "template", "typename", "namespace", "using", "public", "protected", "operator", "sizeof", "auto", "register", "extern", "volatile", "mutable", "explicit", "typedef", "struct", "union", "enum", "goto", "try", "catch", "throw", "volatile", "asm", "__asm", "__asm__", "__volatile__", "__volatile", "__volatile", "__volatile", "__volatile", "__volatile"};

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


void Token::setVal(int val) {
    iVal = val;
    sVal = "";
}

void Token::setVal(std::string val) {
    sVal = val;
    iVal = -1;
}

void Token::setIndices(int line, int index) {
    this->line = line;
    this->index = index;
}

// MUST MANUALLY UPDATE FOR NEW TOKEN TYPES

int Token::getLength() {
    TokenType t = this->token;

    if (t == PLUS || t == MINUS || t == TIMES || t == DIVIDE || t == POWER || t == MOD) return 1; // Arithmetic
    else if (t == PLUSEQUALS || t == MINUSEQUALS || t == TIMESEQUALS || t == DIVIDEEQUALS) return 2; // Compound assignment
    else if (t == PLUSPLUS || t == MINUSMINUS) return 2; // Increment, decrement
    else if (t == LE || t == GE || t == EE || t == NE) return 2; // Comparison, 2 char
    else if (t == LT || t == GT || t == NOT || t == EQUALS) return 1; // Comparison, 1 char
    else if (t == COMMA || t == SEMICOLON || t == COLON || t == DOT || t == SLASH || t == BACKSLASH || t == QUESTION) return 1; // Punctuation
    else if (t == CHAR || t == STRING) return sVal.length() + 2; // Quotes; +2 for "" or ''
    else if (t == OPARENTHESES || t == CPARENTHESES || t == OCURLY || t == CCURLY || t == OBRACKET || t == CBRACKET) return 1; // Braces
    else if (t == BITAND || t == BITOR || t == XOR || t == XNOT) return 1; // Bitwise op, 1 char
    else if (t == AND || t == OR) return 2; // Bitwise op, 2 char
    else if (t == INT) return std::to_string(iVal).length(); // INT
    else if (t == VAR || t == KEYWORD) return sVal.length(); // VAR or KEYWORD
    else if (t == LINECOMMENT) return sVal.length() + 3; // Comments; +3 for '//' and '\n'
    else if (t == BLOCKCOMMENT) return sVal.length() + 4; // Comments; +4 for '/*' and '*/'
    else if (t == WHITESPACE) return 1; // Whitespace
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
        case POWER:
            tokenString = "POWER";
            break;
        case MOD:
            tokenString == "MOD";
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
        case CHAR: // TODO: insert check to see if char is valid
            tokenString = "CHAR: " + sVal;
            break;
        case STRING:
            tokenString = "STRING: " + sVal;
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
        case DOT:
            tokenString = "DOT";
            break;
        case SLASH:
            tokenString = "SLASH";
            break;
        case BACKSLASH:
            tokenString = "BACKSLASH";
            break;
        case QUESTION:
            tokenString = "QUESTION";
            break;
        case AND:
            tokenString = "AND";
            break;
        case BITAND:
            tokenString = "BITAND";
            break;
        case OR:
            tokenString = "OR";
            break;
        case BITOR:
            tokenString = "BITOR";
            break;
        case XOR:
            tokenString = "XOR";
            break;
        case XNOT:
            tokenString = "XNOT";
            break;
        case INT:
            tokenString = "INT: " + std::to_string(iVal);
            break;
        case VAR:
            tokenString = "VAR: " + sVal;
            break;
        case KEYWORD:
            tokenString = "KEYWORD: " + sVal;
            break;
        case LINECOMMENT:
            tokenString = "COMMENT: " + sVal;
            break;
        case BLOCKCOMMENT:
            tokenString = "COMMENT: " + sVal;
            break;
        case WHITESPACE:
            tokenString = "WHITESPACE";
            break;
        case UNKNOWN:
            tokenString = "UNKNOWN";
            break;
        }
        return tokenString;
    }
