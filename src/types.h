#ifndef TYPES_H
#define TYPES_H

#include "AST.h"

static void init_types(AST::Node &tree) {
    if (tree.type != AST::ValueType::UNKNOWN) return;

    switch (tree.nodeType) {
        case AST::NodeType::BOOLEAN_LITERAL: {
            tree.type = AST::ValueType::BOOLEAN;
            break;
        }
        case AST::NodeType::NUMBER_LITERAL: {
            tree.type = AST::ValueType::NUMBER;
            break;
        }
        case AST::NodeType::ADD: {
            auto binaryOp = tree.as<AST::AddNode>();
            init_types(*binaryOp->left);
            init_types(*binaryOp->right);
            if (binaryOp->left->type == AST::ValueType::NUMBER && binaryOp->right->type == AST::ValueType::NUMBER)
                binaryOp->type = AST::ValueType::NUMBER;
            break;
        }
        case AST::NodeType::SUBTRACT: {
            auto binaryOp = tree.as<AST::SubtractNode>();
            init_types(*binaryOp->left);
            init_types(*binaryOp->right);
            if (binaryOp->left->type == AST::ValueType::NUMBER && binaryOp->right->type == AST::ValueType::NUMBER)
                binaryOp->type = AST::ValueType::NUMBER;
            break;
        }
        case AST::NodeType::LESS_THAN: {
            auto binaryOp = tree.as<AST::LessThanNode>();
            init_types(*binaryOp->left);
            init_types(*binaryOp->right);
            if (binaryOp->left->type == AST::ValueType::NUMBER && binaryOp->right->type == AST::ValueType::NUMBER)
                binaryOp->type = AST::ValueType::BOOLEAN;
            break;
        }
        case AST::NodeType::GREATER_THAN: {
            auto binaryOp = tree.as<AST::GreaterThanNode>();
            init_types(*binaryOp->left);
            init_types(*binaryOp->right);
            if (binaryOp->left->type == AST::ValueType::NUMBER && binaryOp->right->type == AST::ValueType::NUMBER)
                binaryOp->type = AST::ValueType::BOOLEAN;
            break;
        }
        case AST::NodeType::AND: {
            auto binaryOp = tree.as<AST::AndNode>();
            init_types(*binaryOp->left);
            init_types(*binaryOp->right);
            if (binaryOp->left->type == AST::ValueType::BOOLEAN && binaryOp->right->type == AST::ValueType::BOOLEAN)
                binaryOp->type = AST::ValueType::BOOLEAN;
            break;
        }
        case AST::NodeType::OR: {
            auto binaryOp = tree.as<AST::AndNode>();
            init_types(*binaryOp->left);
            init_types(*binaryOp->right);
            if (binaryOp->left->type == AST::ValueType::BOOLEAN && binaryOp->right->type == AST::ValueType::BOOLEAN)
                binaryOp->type = AST::ValueType::BOOLEAN;
            break;
        }
        case AST::NodeType::IF: {
            auto ternaryIf = tree.as<AST::IfNode>();
            init_types(*ternaryIf->condition);
            init_types(*ternaryIf->whenFalse);
            init_types(*ternaryIf->whenTrue);
            if (ternaryIf->condition->type == AST::ValueType::BOOLEAN && ternaryIf->whenTrue->type == ternaryIf->whenFalse->type) {
                ternaryIf->type = ternaryIf->whenTrue->type;
            }
            break;
        }
    }
}

#endif