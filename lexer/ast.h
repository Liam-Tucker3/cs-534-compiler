#ifndef AST_H
#define AST_H

#include <string>

enum ASTType {
    AST_PLUS, AST_MINUS, AST_TIMES, AST_DIVIDE, AST_INT, AST_TREE,  AST_OPARAN, AST_CPARAN, AST_NULL // PARAN may be removed later; 
};

class AST {
private:

public:
    // Attributes
    ASTType type;
    AST* left;
    AST* right;
    int val; // Meaningless unless type is INT

    // Constructors
    AST();
    AST(ASTType t); // For +,-,*,/
    AST(ASTType t, int v); // For int
    AST(const AST& other); // Copy constructor
    ~AST(); // Destructor

    // Methods
    std::string typeToString() const; // Add const here
};

#endif // AST_H