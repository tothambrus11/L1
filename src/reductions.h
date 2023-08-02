#ifndef L1_REDUCTIONS_H
#define L1_REDUCTIONS_H

#include "AST.h"
#include <optional>

struct ReductionResult {
    bool haveReduced;
    AST::Node::Ptr reducedTree;
};

struct IReducerService {
    /// Reduces the node as much as possible. The resulting node should be a leaf node unless the program is ill-formed.
    virtual void reduce(AST::Node::Ptr &node) const = 0;
};


void reduce(AST::Node::Ptr &node);

struct IReductionRule {
    using Ptr = std::unique_ptr<IReductionRule>;

    /// Returns whether the node has been reduced by this rule
    virtual bool reduce(AST::Node::Ptr &node) = 0;
};


// BASIC REDUCTIONS
class AddSimpleNumbersReduction : public IReductionRule {
    const IReducerService &reducer;

public:
    explicit AddSimpleNumbersReduction(const IReducerService &reducer) : reducer(reducer) {}

    bool reduce(AST::Node::Ptr &node) override {
        auto *addNode = node->as<AST::AddNode>();
        bool match = addNode &&
                     addNode->left->nodeType == AST::NodeType::NUMBER_LITERAL &&
                     addNode->right->nodeType == AST::NodeType::NUMBER_LITERAL;

        if (!match) return false;
        node = AST::Number(addNode->left->as<AST::NumberNode>()->value +
                           addNode->right->as<AST::NumberNode>()->value);
    }
};
class SubtractSimpleNumbersReduction : public IReductionRule {
    const IReducerService &reducer;

public:
    explicit SubtractSimpleNumbersReduction(const IReducerService &reducer) : reducer(reducer) {}

    bool reduce(AST::Node::Ptr &node) override {
        auto *addNode = node->as<AST::AddNode>();
        bool match = addNode &&
                     addNode->left->nodeType == AST::NodeType::NUMBER_LITERAL &&
                     addNode->right->nodeType == AST::NodeType::NUMBER_LITERAL;

        if (!match) return false;
        node = AST::Number(addNode->left->as<AST::NumberNode>()->value -
                           addNode->right->as<AST::NumberNode>()->value);
    }
};


// LEFT REDUCTIONS
template<AST::NodeType nodeType, AST::NodeType primitiveType>
class LeftReductionForBinaryOp : IReductionRule {
    const IReducerService &reducer;

public:
    explicit LeftReductionForBinaryOp(const IReducerService &reducer) : reducer(reducer) {}
    bool reduce(AST::Node::Ptr &node) override {
        AST::BinaryOpBase<nodeType> *binaryNode = node->as<AST::BinaryOpBase<nodeType>>();
        if (!binaryNode) return false;

        AST::Node::Ptr &leftChild = binaryNode->left;
        if (leftChild->nodeType == primitiveType) return false;

        reduce(leftChild);
        return true;
    }

protected:
    using LeftReductionForBinaryOp_t = LeftReductionForBinaryOp<nodeType, primitiveType>;
};

struct AddLeftReduction : public LeftReductionForBinaryOp<AST::NodeType::ADD, AST::NodeType::NUMBER_LITERAL> {
    using LeftReductionForBinaryOp_t::LeftReductionForBinaryOp;
};
struct SubtractLeftReduction : public LeftReductionForBinaryOp<AST::NodeType::SUBTRACT, AST::NodeType::NUMBER_LITERAL> {
    using LeftReductionForBinaryOp_t::LeftReductionForBinaryOp;
};
struct LessThanLeftReduction : public LeftReductionForBinaryOp<AST::NodeType::LESS_THAN, AST::NodeType::NUMBER_LITERAL> {
    using LeftReductionForBinaryOp_t::LeftReductionForBinaryOp;
};
struct GreaterThanLeftReduction : public LeftReductionForBinaryOp<AST::NodeType::GREATER_THAN, AST::NodeType::NUMBER_LITERAL> {
    using LeftReductionForBinaryOp_t::LeftReductionForBinaryOp;
};
struct OrLeftReduction : public LeftReductionForBinaryOp<AST::NodeType::OR, AST::NodeType::BOOLEAN_LITERAL> {
    using LeftReductionForBinaryOp_t::LeftReductionForBinaryOp;
};
struct AndLeftReduction : public LeftReductionForBinaryOp<AST::NodeType::AND, AST::NodeType::BOOLEAN_LITERAL> {
    using LeftReductionForBinaryOp_t::LeftReductionForBinaryOp;
};

// RIGHT REDUCTIONS
template<AST::NodeType nodeType, AST::NodeType primitiveType>
struct RightReductionForBinaryOp : IReductionRule {
    bool reduce(AST::Node::Ptr &node) override {
        AST::BinaryOpBase<nodeType> *binaryNode = node->as<AST::BinaryOpBase<nodeType>>();
        if (!binaryNode) return false;

        bool match = binaryNode->left->nodeType == primitiveType && binaryNode->right->nodeType != primitiveType;
        if (!match) return false;

        reduce(binaryNode->right);
        return true;
    }
};

struct AddRightReduction : public RightReductionForBinaryOp<AST::NodeType::ADD, AST::NodeType::NUMBER_LITERAL> {
};
struct SubtractRightReduction : public RightReductionForBinaryOp<AST::NodeType::SUBTRACT, AST::NodeType::NUMBER_LITERAL> {
};
struct LessThanRightReduction : public RightReductionForBinaryOp<AST::NodeType::LESS_THAN, AST::NodeType::NUMBER_LITERAL> {
};
struct GreaterThanRightReduction : public RightReductionForBinaryOp<AST::NodeType::GREATER_THAN, AST::NodeType::NUMBER_LITERAL> {
};
struct OrRightReduction : public RightReductionForBinaryOp<AST::NodeType::OR, AST::NodeType::NUMBER_LITERAL> {
};
struct AndRightReduction : public RightReductionForBinaryOp<AST::NodeType::AND, AST::NodeType::NUMBER_LITERAL> {
};


//
//ReductionResult reduce(AST::Node::Ptr node) {
//    bool haveReduced = false;
//tryReducingAgain:
//
//    if (node->nodeType == AST::NodeType::BINARY_OP) {
//        auto *opNode = node->as<AST::BinaryOp>();
//        if (opNode->left->nodeType == AST::NodeType::NUMBER_LEAF &&
//            opNode->right->nodeType == AST::NodeType::NUMBER_LEAF) {
//
//            node = AST::Number(opNode->left->as<AST::NumberNode>()->value +
//                               opNode->right->as<AST::NumberNode>()->value);
//
//            haveReduced = true;
//            goto tryReducingAgain;
//        }
//
//        // reduce left child of binary operand
//        auto [haveReducedLeft, reducedLeft] = reduce(std::move(opNode->left));
//        opNode->left = std::move(reducedLeft);
//        if (haveReducedLeft) {
//            haveReduced = true;
//            goto tryReducingAgain;
//        }
//
//        if (opNode->left->nodeType == AST::NodeType::NUMBER_LEAF) {}
//        // reduce right child of binary operand
//        auto [haveReducedRight, reducedRight] = reduce(std::move(opNode->right));
//        opNode->right = std::move(reducedRight);
//        if (haveReducedRight) {
//            haveReduced = true;
//            goto tryReducingAgain;
//        }
//    }
//
//    return {haveReduced, std::move(node)};
//}

#endif//L1_REDUCTIONS_H
