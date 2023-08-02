#ifndef TYPES_H
#define TYPES_H

#include "AST.h"

static void init_types(AST::Node &tree) {
    if (tree.type != AST::ValueType::UNKNOWN) return;

    switch (tree.nodeType) {
        case AST::NodeType::BOOLEAN_LEAF: {
            tree.type = AST::ValueType::BOOLEAN;
            break;
        }
        case AST::NodeType::NUMBER_LEAF: {
            tree.type = AST::ValueType::NUMBER;
            break;
        }
        case AST::NodeType::BINARY_OP: {
            auto binaryOp = tree.as<AST::BinaryOp>();
            init_types(*binaryOp->left);
            init_types(*binaryOp->right);
            if (binaryOp->left->type == binaryOp->right->type)
                binaryOp->type = binaryOp->left->type;
            break;
        }
        case AST::NodeType::TERNARY_IF: {
            auto ternaryIf = tree.as<AST::IfNode>();
            init_types(*ternaryIf->condition);
            init_types(*ternaryIf->whenFalse);
            init_types(*ternaryIf->whenTrue);
            if (ternaryIf->condition->type == AST::ValueType::BOOLEAN &&
                ternaryIf->whenTrue->type == ternaryIf->whenFalse->type) {

                ternaryIf->type = ternaryIf->whenFalse->type;
            }
            break;
        }

    }
}

#endif