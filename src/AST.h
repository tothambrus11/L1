#ifndef AST_H
#define AST_H

#include <concepts>
#include <memory>

namespace AST {
    enum class NodeType {
        NUMBER_LITERAL,
        BOOLEAN_LITERAL,
        ADD,
        SUBTRACT,
        LESS_THAN,
        GREATER_THAN,
        AND,
        OR,
        IF
    };

    enum class ValueType {
        BOOLEAN,
        NUMBER,
        UNKNOWN
    };

    class Node;

    template<class T>
    concept is_node = std::is_base_of<Node, T>::value &&
        requires()
    {
        { T::NODE_TYPE } -> std::convertible_to<NodeType>;
    };

    struct Node {
        NodeType nodeType;
        ValueType type{ValueType::UNKNOWN};

        using Ptr = std::unique_ptr<Node>;

        explicit Node(NodeType nodeType) : nodeType(nodeType) {}

        template<is_node N>
        N *as() {
            return N::NODE_TYPE == nodeType ? static_cast<N *>(this) : nullptr;
        }
    };


    template<NodeType nodeType_>
    struct NodeBase : Node {
        static constexpr NodeType NODE_TYPE = nodeType_;

        NodeBase() : Node(nodeType_) {}
    };


    struct NumberNode : public NodeBase<NodeType::NUMBER_LITERAL> {
        int value;

        explicit NumberNode(int value) : value(value) {}
    };

    struct BooleanNode : public NodeBase<NodeType::BOOLEAN_LITERAL> {
        bool value;

        explicit BooleanNode(bool value) : value(value) {}
    };

    template<NodeType nodeType_>
    struct BinaryOpBase : public NodeBase<nodeType_> {
        Node::Ptr left, right;
        using BinaryOpBase_t = BinaryOpBase<nodeType_>;

        BinaryOpBase(Node::Ptr left, Node::Ptr right)
            : left(std::move(left)),
              right(std::move(right)) {
        }
    };

    struct AddNode : public BinaryOpBase<NodeType::ADD> {
        using BinaryOpBase_t::BinaryOpBase;
    };
    struct SubtractNode : public BinaryOpBase<NodeType::SUBTRACT> {
        using BinaryOpBase_t::BinaryOpBase;
    };
    struct LessThanNode : public BinaryOpBase<NodeType::LESS_THAN> {
        using BinaryOpBase_t::BinaryOpBase;
    };
    struct GreaterThanNode : public BinaryOpBase<NodeType::GREATER_THAN> {
        using BinaryOpBase_t::BinaryOpBase;
    };
    struct AndNode : public BinaryOpBase<NodeType::AND> {
        using BinaryOpBase_t::BinaryOpBase;
    };
    struct OrNode : public BinaryOpBase<NodeType::OR> {
        using BinaryOpBase_t::BinaryOpBase;
    };

    struct IfNode : NodeBase<NodeType::IF> {
        Node::Ptr condition;
        Node::Ptr whenTrue;
        Node::Ptr whenFalse;

        IfNode(Node::Ptr condition, Node::Ptr whenTrue, Node::Ptr whenFalse)
            : condition(std::move(condition)),
              whenTrue(std::move(whenTrue)),
              whenFalse(std::move(whenFalse)) {}
    };

    static auto Number(int n) {
        return std::make_unique<NumberNode>(n);
    }

    static auto Boolean(bool b) {
        return std::make_unique<BooleanNode>(b);
    }

    static auto If(AST::Node::Ptr cond, AST::Node::Ptr whenTrue, AST::Node::Ptr whenFalse) {
        return std::make_unique<IfNode>(std::move(cond), std::move(whenTrue), std::move(whenFalse));
    }

    static auto Add(AST::Node::Ptr left, AST::Node::Ptr right) {
        return std::make_unique<AddNode>(std::move(left), std::move(right));
    }

    static auto Subtract(AST::Node::Ptr left, AST::Node::Ptr right) {
        return std::make_unique<SubtractNode>(std::move(left), std::move(right));
    }

    static auto LessThan(AST::Node::Ptr left, AST::Node::Ptr right) {
        return std::make_unique<LessThanNode>(std::move(left), std::move(right));
    }

    static auto GraterThan(AST::Node::Ptr left, AST::Node::Ptr right) {
        return std::make_unique<GreaterThanNode>(std::move(left), std::move(right));
    }

}// namespace AST

#endif