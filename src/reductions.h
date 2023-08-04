#ifndef L1_REDUCTIONS_H
#define L1_REDUCTIONS_H

#include "AST.h"
#include <optional>
#include <vector>

struct ReductionResult {
    bool haveReduced;
    AST::Node::Ptr reducedTree;
};

struct IReducerStrategy {
    /// Reduces the node as much as possible. The resulting node should be a leaf node unless the program is ill-formed.
    virtual void reduce(AST::Node::Ptr &node) const = 0;

    virtual ~IReducerStrategy() = default;
};


struct IReductionRule {
    using Ptr = std::unique_ptr<IReductionRule>;

    /// Returns whether the node has been reduced by this rule
    virtual bool reduce(AST::Node::Ptr &node) const = 0;

    virtual ~IReductionRule() = default;
};


// BASIC REDUCTIONS
struct AddSimpleReduction : public IReductionRule {
    bool reduce(AST::Node::Ptr &node) const override {
        auto *addNode = node->as<AST::AddNode>();
        bool match = addNode &&
                     addNode->left->nodeType == AST::NodeType::NUMBER_LITERAL &&
                     addNode->right->nodeType == AST::NodeType::NUMBER_LITERAL;

        if (!match) return false;
        node = AST::Number(addNode->left->as<AST::NumberNode>()->value +
                           addNode->right->as<AST::NumberNode>()->value);
        return true;
    }
};

struct SubtractSimpleReduction : public IReductionRule {
    bool reduce(AST::Node::Ptr &node) const override {
        auto *subtractNode = node->as<AST::SubtractNode>();
        bool match = subtractNode &&
                     subtractNode->left->nodeType == AST::NodeType::NUMBER_LITERAL &&
                     subtractNode->right->nodeType == AST::NodeType::NUMBER_LITERAL;

        if (!match) return false;
        node = AST::Number(subtractNode->left->as<AST::NumberNode>()->value -
                           subtractNode->right->as<AST::NumberNode>()->value);
        return true;
    }
};

struct LessThanSimpleReduction : public IReductionRule {
    bool reduce(AST::Node::Ptr &node) const override {
        auto *lessThanNode = node->as<AST::LessThanNode>();
        bool match = lessThanNode &&
                     lessThanNode->left->nodeType == AST::NodeType::NUMBER_LITERAL &&
                     lessThanNode->right->nodeType == AST::NodeType::NUMBER_LITERAL;

        if (!match) return false;
        node = AST::Boolean(lessThanNode->left->as<AST::NumberNode>()->value <
                            lessThanNode->right->as<AST::NumberNode>()->value);
        return true;
    }
};

struct GreaterThanSimpleReduction : public IReductionRule {
    bool reduce(AST::Node::Ptr &node) const override {
        auto *greaterThanNode = node->as<AST::GreaterThanNode>();
        bool match = greaterThanNode &&
                     greaterThanNode->left->nodeType == AST::NodeType::NUMBER_LITERAL &&
                     greaterThanNode->right->nodeType == AST::NodeType::NUMBER_LITERAL;

        if (!match) return false;
        node = AST::Boolean(greaterThanNode->left->as<AST::NumberNode>()->value >
                            greaterThanNode->right->as<AST::NumberNode>()->value);
        return true;
    }
};

struct AndSimpleReduction : public IReductionRule {
    bool reduce(AST::Node::Ptr &node) const override {
        auto *andNode = node->as<AST::AndNode>();
        bool match = andNode &&
                     andNode->left->nodeType == AST::NodeType::BOOLEAN_LITERAL &&
                     andNode->right->nodeType == AST::NodeType::BOOLEAN_LITERAL;

        if (!match) return false;
        node = AST::Boolean(andNode->left->as<AST::BooleanNode>()->value &&
                            andNode->right->as<AST::BooleanNode>()->value);
        return true;
    }
};

struct OrSimpleReduction : public IReductionRule {
    bool reduce(AST::Node::Ptr &node) const override {
        auto *orNode = node->as<AST::OrNode>();
        bool match = orNode &&
                     orNode->left->nodeType == AST::NodeType::BOOLEAN_LITERAL &&
                     orNode->right->nodeType == AST::NodeType::BOOLEAN_LITERAL;

        if (!match) return false;
        node = AST::Boolean(orNode->left->as<AST::BooleanNode>()->value ||
                            orNode->right->as<AST::BooleanNode>()->value);
        return true;
    }
};

// LEFT REDUCTIONS
template<AST::NodeType nodeType, AST::NodeType primitiveType>
class LeftReductionForBinaryOp : public IReductionRule {
    const IReducerStrategy &reducer;

public:
    explicit LeftReductionForBinaryOp(const IReducerStrategy &reducer) : reducer(reducer) {}

    LeftReductionForBinaryOp() = delete;

    bool reduce(AST::Node::Ptr &node) const override {
        auto *binaryNode = node->as<AST::BinaryOpBase<nodeType>>();
        if (!binaryNode) return false;

        AST::Node::Ptr &leftChild = binaryNode->left;
        if (leftChild->nodeType == primitiveType) return false;

        reducer.reduce(leftChild);
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
struct LessThanLeftReduction
        : public LeftReductionForBinaryOp<AST::NodeType::LESS_THAN, AST::NodeType::NUMBER_LITERAL> {
    using LeftReductionForBinaryOp_t::LeftReductionForBinaryOp;
};
struct GreaterThanLeftReduction
        : public LeftReductionForBinaryOp<AST::NodeType::GREATER_THAN, AST::NodeType::NUMBER_LITERAL> {
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
class RightReductionForBinaryOp : public IReductionRule {
    const IReducerStrategy &reducer;

public:
    explicit RightReductionForBinaryOp(const IReducerStrategy &reducer) : reducer(reducer) {}

    bool reduce(AST::Node::Ptr &node) const override {
        AST::BinaryOpBase<nodeType> *binaryNode = node->as<AST::BinaryOpBase<nodeType>>();
        if (!binaryNode) return false;

        bool match = binaryNode->left->nodeType == primitiveType && binaryNode->right->nodeType != primitiveType;
        if (!match) return false;

        reducer.reduce(binaryNode->right);
        return true;
    }

protected:
    using RightReductionForBinaryOp_t = RightReductionForBinaryOp<nodeType, primitiveType>;
};

struct AddRightReduction : public RightReductionForBinaryOp<AST::NodeType::ADD, AST::NodeType::NUMBER_LITERAL> {
    using RightReductionForBinaryOp_t::RightReductionForBinaryOp;
};
struct SubtractRightReduction
        : public RightReductionForBinaryOp<AST::NodeType::SUBTRACT, AST::NodeType::NUMBER_LITERAL> {
    using RightReductionForBinaryOp_t::RightReductionForBinaryOp;
};
struct LessThanRightReduction
        : public RightReductionForBinaryOp<AST::NodeType::LESS_THAN, AST::NodeType::NUMBER_LITERAL> {
    using RightReductionForBinaryOp_t::RightReductionForBinaryOp;
};
struct GreaterThanRightReduction
        : public RightReductionForBinaryOp<AST::NodeType::GREATER_THAN, AST::NodeType::NUMBER_LITERAL> {
    using RightReductionForBinaryOp_t::RightReductionForBinaryOp;
};
struct OrRightReduction : public RightReductionForBinaryOp<AST::NodeType::OR, AST::NodeType::NUMBER_LITERAL> {
    using RightReductionForBinaryOp_t::RightReductionForBinaryOp;
};
struct AndRightReduction : public RightReductionForBinaryOp<AST::NodeType::AND, AST::NodeType::NUMBER_LITERAL> {
    using RightReductionForBinaryOp_t::RightReductionForBinaryOp;
};

struct IfConditionReduction : public IReductionRule {
    const IReducerStrategy &reducer;

public:
    explicit IfConditionReduction(const IReducerStrategy &reducer) : reducer(reducer) {}

    bool reduce(AST::Node::Ptr &node) const override {
        auto *ifNode = node->as<AST::IfNode>();

        bool match = ifNode && ifNode->condition->nodeType != AST::NodeType::BOOLEAN_LITERAL;
        if (!match) return false;

        reducer.reduce(ifNode->condition);
        return true;
    }
};

struct IfResultReduction : public IReductionRule {
    bool reduce(AST::Node::Ptr &node) const override {
        auto *ifNode = node->as<AST::IfNode>();

        bool match = ifNode && ifNode->condition->nodeType == AST::NodeType::BOOLEAN_LITERAL;
        if (!match) return false;

        node = ifNode->condition->as<AST::BooleanNode>()->value ? std::move(ifNode->whenTrue) : std::move(
                ifNode->whenFalse);
        return true;
    }
};


class DumbReducerService : public IReducerStrategy {
    std::vector<IReductionRule::Ptr> reductionRules;

public:
    DumbReducerService() {
        // simple binary reductions
        reductionRules.push_back(std::make_unique<AddSimpleReduction>());
        reductionRules.push_back(std::make_unique<SubtractSimpleReduction>());
        reductionRules.push_back(std::make_unique<LessThanSimpleReduction>());
        reductionRules.push_back(std::make_unique<GreaterThanSimpleReduction>());
        reductionRules.push_back(std::make_unique<AndSimpleReduction>());
        reductionRules.push_back(std::make_unique<OrSimpleReduction>());

        // left reductions
        reductionRules.push_back(std::make_unique<AddLeftReduction>(*this));
        reductionRules.push_back(std::make_unique<SubtractLeftReduction>(*this));
        reductionRules.push_back(std::make_unique<LessThanLeftReduction>(*this));
        reductionRules.push_back(std::make_unique<GreaterThanLeftReduction>(*this));
        reductionRules.push_back(std::make_unique<AndLeftReduction>(*this));
        reductionRules.push_back(std::make_unique<OrLeftReduction>(*this));

        // right reductions
        reductionRules.push_back(std::make_unique<AddRightReduction>(*this));
        reductionRules.push_back(std::make_unique<SubtractRightReduction>(*this));
        reductionRules.push_back(std::make_unique<LessThanRightReduction>(*this));
        reductionRules.push_back(std::make_unique<GreaterThanRightReduction>(*this));
        reductionRules.push_back(std::make_unique<AndRightReduction>(*this));
        reductionRules.push_back(std::make_unique<OrRightReduction>(*this));

        // if reductions
        reductionRules.push_back(std::make_unique<IfConditionReduction>(*this));
        reductionRules.push_back(std::make_unique<IfResultReduction>());
    }

    void reduce(AST::Node::Ptr &node) const override {
        while (std::ranges::any_of(reductionRules, [&](const IReductionRule::Ptr &rule) -> bool {
            return rule->reduce(node);
        }));
    };
};


class SmartReducerService : public IReducerStrategy {
public:
    const AddSimpleReduction addSimpleReduction{};
    const SubtractSimpleReduction subtractSimpleReduction{};
    const LessThanSimpleReduction lessThanSimpleReduction{};
    const GreaterThanSimpleReduction greaterThanSimpleReduction{};
    const AndSimpleReduction andSimpleReduction{};
    const OrSimpleReduction orSimpleReduction{};
    const AddLeftReduction addLeftReduction{*this};
    const SubtractLeftReduction subtractLeftReduction{*this};
    const LessThanLeftReduction lessThanLeftReduction{*this};
    const GreaterThanLeftReduction greaterThanLeftReduction{*this};
    const AndLeftReduction andLeftReduction{*this};
    const OrLeftReduction orLeftReduction{*this};
    const AddRightReduction addRightReduction{*this};
    const SubtractRightReduction subtractRightReduction{*this};
    const LessThanRightReduction lessThanRightReduction{*this};
    const GreaterThanRightReduction greaterThanRightReduction{*this};
    const AndRightReduction andRightReduction{*this};
    const OrRightReduction orRightReduction{*this};
    const IfConditionReduction ifConditionReduction{*this};
    const IfResultReduction ifResultReduction{};

    void reduce(AST::Node::Ptr &node) const override {
        bool haveReduced = true;

        while (haveReduced) {
            haveReduced = false;

            switch (node->nodeType) {
                case AST::NodeType::NUMBER_LITERAL:
                case AST::NodeType::BOOLEAN_LITERAL:
                    // Irreducible
                    break;
                case AST::NodeType::ADD:
                    haveReduced |= addLeftReduction.reduce(node);
                    haveReduced |= addRightReduction.reduce(node);
                    haveReduced |= addSimpleReduction.reduce(node);
                    break;
                case AST::NodeType::SUBTRACT:
                    haveReduced |= subtractLeftReduction.reduce(node);
                    haveReduced |= subtractRightReduction.reduce(node);
                    haveReduced |= subtractSimpleReduction.reduce(node);
                    break;
                case AST::NodeType::LESS_THAN:
                    haveReduced |= lessThanLeftReduction.reduce(node);
                    haveReduced |= lessThanRightReduction.reduce(node);
                    haveReduced |= lessThanSimpleReduction.reduce(node);
                    break;
                case AST::NodeType::GREATER_THAN:
                    haveReduced |= greaterThanLeftReduction.reduce(node);
                    haveReduced |= greaterThanRightReduction.reduce(node);
                    haveReduced |= greaterThanSimpleReduction.reduce(node);
                    break;
                case AST::NodeType::AND:
                    haveReduced |= andLeftReduction.reduce(node);
                    haveReduced |= andRightReduction.reduce(node);
                    haveReduced |= andSimpleReduction.reduce(node);
                    break;
                case AST::NodeType::OR:
                    haveReduced |= orLeftReduction.reduce(node);
                    haveReduced |= orRightReduction.reduce(node);
                    haveReduced |= orSimpleReduction.reduce(node);
                    break;
                case AST::NodeType::IF:
                    haveReduced |= ifConditionReduction.reduce(node);
                    haveReduced |= ifResultReduction.reduce(node);
                    break;
            }
        }
    };
};


#endif//L1_REDUCTIONS_H
