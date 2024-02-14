/**
 * @file builtin_functions_tests.cpp
 * @brief builtin_functions_tests checks the reliability of the methods in functions.hpp
 * @details Various data types are parsed as different data and interpreted**
 */
#include "source/builtin_functions/functions.hpp"
#include "gtest/gtest.h"

TEST(GENERALTESTS, intToIntTest) {
    int64_t originalValue = 36;
    int64_t newValue = parseAsType<int64_t, int64_t>(originalValue);
    EXPECT_EQ(newValue, originalValue);
}

TEST(GENERALTESTS, boolToBoolTest) {
    bool originalValue = true;
    bool newValue = parseAsType<bool, bool>(originalValue);
    EXPECT_TRUE(originalValue);
    EXPECT_TRUE(newValue);
}

TEST(GENERALTESTS, floatToFloatTest) {
    double originalValue = 3.14; //As specified in functions.hpp, float and double will be identical
    double newValue = parseAsType<double, double>(originalValue);
    EXPECT_EQ(newValue, originalValue);
}

TEST(INTTESTS, intToBoolTest) {
    int64_t originalValue = 8;
    bool newValue = parseAsType<int64_t, bool>(originalValue);
    EXPECT_TRUE(newValue);
    originalValue = 0;
    newValue = parseAsType<int64_t, bool>(originalValue);
    EXPECT_FALSE(newValue);
}

TEST(INTTESTS, intToFloatTest) {
    int64_t originalValue = 8;
    double newValue = parseAsType<int64_t, double>(originalValue);
    EXPECT_EQ(8.0, newValue);
}

TEST(FLOATTESTS, floatToBoolTest) {
    double originalValue = 3.14;
    bool newValue = parseAsType<double, bool>(originalValue);
    EXPECT_TRUE(newValue);
    originalValue = 0;
    newValue = parseAsType<double, bool>(originalValue);
    EXPECT_FALSE(newValue);
}

TEST(FLOATTESTS, floatToIntTest) {
    double originalValue = 3.14;
    int64_t newValue = parseAsType<double, int64_t>(originalValue);
    EXPECT_EQ(3, newValue);
}

TEST(STRINGTESTS, boolToStringTest) {
    bool originalValue = true;
    std::string newValue = parseAsString(originalValue);
    EXPECT_EQ("true", newValue);
    originalValue = false;
    newValue = parseAsString(originalValue);
    EXPECT_EQ("false", newValue);
}

TEST(STRINGTESTS, floatToStringTest) {
    double originalValue = 3.14;
    std::string newValue = parseAsString(originalValue);
    EXPECT_EQ("3.14", newValue);
    originalValue = -36.45;
    newValue = parseAsString(originalValue);
    EXPECT_EQ("-36.45", newValue);
}

TEST(STRINGTESTS, intToStringTest) {
    int64_t originalValue = 8;
    std::string newValue = parseAsString(originalValue);
    EXPECT_EQ("8", newValue);
    originalValue = 0;
    newValue = parseAsString(originalValue);
    EXPECT_EQ("0", newValue);
}

