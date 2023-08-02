#include "gtest/gtest.h"
#include "gmock/gmock.h"

#include "../src/AST.h"
#include "../src/reductions.h"

using namespace testing;

TEST(Reduction, SimpleNumberIsIrreducible) {
    auto [reduced, node] = reduce(AST::Number(12));
    ASSERT_THAT(reduced, IsFalse());
    ASSERT_THAT(node->nodeType, Eq(AST::NodeType::NUMBER_LEAF));
}

TEST(Reduction, SimpleBoolIsIrreducible) {
    auto [reduced, node] = reduce(AST::Boolean(true));
    ASSERT_THAT(reduced, IsFalse());
    ASSERT_THAT(node->nodeType, Eq(AST::NodeType::BOOLEAN_LEAF));
}

TEST(Reduction, BinaryOpWithTwoNumberLeaves) {
    auto [reduced, node] = reduce(AST::Add(AST::Number(1), AST::Number(3)));
    ASSERT_THAT(reduced, IsTrue());
    ASSERT_THAT(node->nodeType, Eq(AST::NodeType::NUMBER_LEAF));
    ASSERT_THAT(node->as<AST::NumberNode>()->value, Eq(4));
}
TEST(Reduction, Subtraction) {
    auto [reduced, node] = reduce(AST::Subtract(AST::Number(1), AST::Number(3)));
    ASSERT_THAT(reduced, IsTrue());
    ASSERT_THAT(node->nodeType, Eq(AST::NodeType::NUMBER_LEAF));
    ASSERT_THAT(node->as<AST::NumberNode>()->value, Eq(-2));
}

TEST(Reduction, BinaryOpWithLeftComplex) {
    auto [reduced, node] = reduce(
            AST::Add(
                    AST::Add(AST::Number(1),
                             AST::Number(2)),
                    AST::Number(3)
            )
    );
    ASSERT_THAT(reduced, IsTrue());
    ASSERT_THAT(node->nodeType, Eq(AST::NodeType::NUMBER_LEAF));
    ASSERT_THAT(node->as<AST::NumberNode>()->value, Eq(6));
}

TEST(Reduction, BinaryOpWithRightComplex) {
    auto [reduced, node] = reduce(
            AST::Add(
                    AST::Number(3),
                    AST::Add(AST::Number(1),
                             AST::Number(2))
            )
    );
    ASSERT_THAT(reduced, IsTrue());
    ASSERT_THAT(node->nodeType, Eq(AST::NodeType::NUMBER_LEAF));
    ASSERT_THAT(node->as<AST::NumberNode>()->value, Eq(6));
}

TEST(Reduction, BinaryOpWithBothComplex) {
    auto [reduced, node] = reduce(
            AST::Add(
                    AST::Add(AST::Number(1),
                             AST::Number(2)),
                    AST::Add(AST::Number(9),
                             AST::Number(4))
            )
    );
    ASSERT_THAT(reduced, IsTrue());
    ASSERT_THAT(node->nodeType, Eq(AST::NodeType::NUMBER_LEAF));
    ASSERT_THAT(node->as<AST::NumberNode>()->value, Eq(16));
}

TEST(Reduction, BinaryOpWithManyDepths) {
    auto [reduced, node] = reduce(
            AST::Add(
                    AST::Number(2),
                    AST::Add(AST::Number(9),
                             AST::Add(
                                     AST::Number(2),
                                     AST::Number(1)
                             )
                    )
            )
    );
    ASSERT_THAT(reduced, IsTrue());
    ASSERT_THAT(node->nodeType, Eq(AST::NodeType::NUMBER_LEAF));
    ASSERT_THAT(node->as<AST::NumberNode>()->value, Eq(14));
}

