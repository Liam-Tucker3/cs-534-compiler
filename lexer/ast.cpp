#include "ast.h"

AST::AST() {
    type = nullptr;
    left = nullptr;
    right = nullptr;
    val = -1;
}

AST::AST(ASTType t) {
    type = &t;
    left = nullptr;
    right = nullptr;
    val = -1;
};

AST::AST(ASTType t, int v) {
    type = &t;
    left = nullptr;
    right = nullptr;
    val = v;
}

