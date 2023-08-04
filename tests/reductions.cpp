#include "gmock/gmock.h"
#include "gtest/gtest.h"

#include "../src/AST.h"
#include "../src/reductions.h"

using namespace testing;

struct ReductionTest : public TestWithParam<std::shared_ptr<IReducerStrategy>> {
    std::shared_ptr<IReducerStrategy> reducer;

    void SetUp() override {
        reducer = GetParam();
    }
};

TEST_P(ReductionTest, SimpleNumberIsIrreducible) {
    AST::Node::Ptr node = AST::Number(12);

    reducer->reduce(node);

    ASSERT_THAT(node->nodeType, Eq(AST::NodeType::NUMBER_LITERAL));
    ASSERT_THAT(node->as<AST::NumberNode>()->value, Eq(12));
}

TEST_P(ReductionTest, SimpleBoolIsIrreducible) {
    AST::Node::Ptr node = AST::Boolean(true);

    reducer->reduce(node);

    ASSERT_THAT(node->nodeType, Eq(AST::NodeType::BOOLEAN_LITERAL));
    ASSERT_THAT(node->as<AST::BooleanNode>()->value, Eq(true));
}

TEST_P(ReductionTest, BinaryOpWithTwoNumberLeaves) {
    AST::Node::Ptr node = AST::Add(AST::Number(1), AST::Number(3));

    reducer->reduce(node);

    ASSERT_THAT(node->nodeType, Eq(AST::NodeType::NUMBER_LITERAL));
    ASSERT_THAT(node->as<AST::NumberNode>()->value, Eq(4));
}

TEST_P(ReductionTest, Subtraction) {
    AST::Node::Ptr node = AST::Subtract(AST::Number(1), AST::Number(3));

    reducer->reduce(node);

    ASSERT_THAT(node->nodeType, Eq(AST::NodeType::NUMBER_LITERAL));
    ASSERT_THAT(node->as<AST::NumberNode>()->value, Eq(-2));
}

TEST_P(ReductionTest, BinaryOpWithLeftComplex) {
    AST::Node::Ptr node = AST::Add(
            AST::Add(
                    AST::Number(1),
                    AST::Number(2)
            ),
            AST::Number(3)
    );

    reducer->reduce(node);

    ASSERT_THAT(node->nodeType, Eq(AST::NodeType::NUMBER_LITERAL));
    ASSERT_THAT(node->as<AST::NumberNode>()->value, Eq(6));
}

TEST_P(ReductionTest, BinaryOpWithRightComplex) {
    AST::Node::Ptr node = AST::Add(
            AST::Number(3),
            AST::Add(
                    AST::Number(1),
                    AST::Number(2)
            )
    );

    reducer->reduce(node);

    ASSERT_THAT(node->nodeType, Eq(AST::NodeType::NUMBER_LITERAL));
    ASSERT_THAT(node->as<AST::NumberNode>()->value, Eq(6));
}

TEST_P(ReductionTest, BinaryOpWithBothComplex) {
    AST::Node::Ptr node = AST::Add(
            AST::Add(
                    AST::Number(1),
                    AST::Number(2)
            ),
            AST::Add(
                    AST::Number(3),
                    AST::Number(4)
            )
    );

    reducer->reduce(node);

    ASSERT_THAT(node->nodeType, Eq(AST::NodeType::NUMBER_LITERAL));
    ASSERT_THAT(node->as<AST::NumberNode>()->value, Eq(10));
}

TEST_P(ReductionTest, BinaryOpWithManyDepths) {
    AST::Node::Ptr node = AST::Add(
            AST::Add(
                    AST::Number(1),
                    AST::Add(
                            AST::Number(2),
                            AST::Number(3)
                    )
            ),
            AST::Add(
                    AST::Number(4),
                    AST::Number(5)
            )
    );

    reducer->reduce(node);

    ASSERT_THAT(node->nodeType, Eq(AST::NodeType::NUMBER_LITERAL));
    ASSERT_THAT(node->as<AST::NumberNode>()->value, Eq(15));
}

INSTANTIATE_TEST_SUITE_P(ReductionTests, ReductionTest, Values(
        std::make_shared<DumbReducerService>(),
        std::make_shared<SmartReducerService>()
));