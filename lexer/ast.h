#ifndef AST_H
#define AST_H

enum ASTType {
    AST_PLUS, AST_MINUS, AST_TIMES, AST_DIVIDE, AST_INT, AST_TREE, AST_OPARAN, AST_CPARAN, AST_NULL // PARAN may be removed later; 
    // currently for ease of processing; should not appear in any real AST tree.
};

class AST {
private:

public:
    // Attributes
    ASTType* type;
    AST* left;
    AST* right;
    int val; // Meaningless unless type is INT

    // Constructors
    AST();
    AST(ASTType t); // For +,-,*,/
    AST(ASTType t, int v); // For int
};

#endif // AST_H