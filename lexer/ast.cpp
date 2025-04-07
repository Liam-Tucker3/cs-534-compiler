#include "ast.h"

AST::AST() {
    type = AST_NULL;
    left = nullptr;
    right = nullptr;
    val = -1;
}

AST::AST(ASTType t) {
    type = t;
    left = nullptr;
    right = nullptr;
    val = -1;
};

AST::AST(ASTType t, int v) {
    type = t;
    left = nullptr;
    right = nullptr;
    val = v;
}

AST::AST(const AST& other) {
    type = other.type;
    val = other.val;
    
    // Deep copy children
    if (other.left != nullptr) {
        left = new AST(*other.left);
    } else {
        left = nullptr;
    }
    
    if (other.right != nullptr) {
        right = new AST(*other.right);
    } else {
        right = nullptr;
    }
}

AST::~AST() {
    // Recursively delete children
    if (left != nullptr) {
        delete left;
        left = nullptr;
    }
    if (right != nullptr) {
        delete right;
        right = nullptr;
    }
}

std::string AST::typeToString() const {
    switch (type) {
        case AST_PLUS:
            return "PLUS";
        case AST_MINUS:
            return "MINUS";
        case AST_TIMES:
            return "TIMES";
        case AST_DIVIDE:
            return "DIVIDE";
        case AST_INT:
            return "INT";
        case AST_TREE:
            return "TREE";
        case AST_OPARAN:
            return "OPARAN";
        case AST_CPARAN:
            return "CPARAN";
        case AST_NULL:
            return "NULL";
        default:
            return "UNKNOWN";
    }
}