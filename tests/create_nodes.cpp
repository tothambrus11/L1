#include "gtest/gtest.h"
#include "gmock/gmock.h"

#include "../src/AST.h"
#include "../src/types.h"

using namespace testing;

TEST(TypeInitialization, LeafNodes) {
    {
        auto node = AST::Number(12);
        init_types(*node);
        EXPECT_THAT(node->type, Eq(AST::ValueType::NUMBER));
    }
    {
        auto node = AST::Boolean(true);
        init_types(*node);
        EXPECT_THAT(node->type, Eq(AST::ValueType::BOOLEAN));
    }
}

TEST(TypeInitialization, BinarySameOperands){
    auto node = AST::Add(AST::Number(1), AST::Number(2));
    init_types(*node);
    EXPECT_THAT(node->type, Eq(AST::ValueType::NUMBER));
}

TEST(TypeInitialization, BinaryDifferent){
    auto node = AST::Add(AST::Boolean(true), AST::Number(2));
    init_types(*node);
    EXPECT_THAT(node->type, Eq(AST::ValueType::UNKNOWN));
}
TEST(TypeInitialization, TernarySameAndBoolCond){
    auto node = AST::If(AST::Boolean(true), AST::Number(1), AST::Number(2));
    init_types(*node);
    EXPECT_THAT(node->type, Eq(AST::ValueType::NUMBER));
}
TEST(TypeInitialization, TernarySameAndNotBoolCond){
    auto node = AST::If(AST::Number(1), AST::Number(1), AST::Number(2));
    init_types(*node);
    EXPECT_THAT(node->type, Eq(AST::ValueType::UNKNOWN));
}
TEST(TypeInitialization, TernaryDifferent){
    auto node = AST::If(AST::Boolean(true), AST::Boolean(true), AST::Number(2));
    init_types(*node);
    EXPECT_THAT(node->type, Eq(AST::ValueType::UNKNOWN));
}
TEST(TypeInitialization, ComplexTree){
    auto node = AST::If(AST::Boolean(true), AST::Boolean(true), AST::Number(2));
    init_types(*node);
    EXPECT_THAT(node->type, Eq(AST::ValueType::UNKNOWN));
}