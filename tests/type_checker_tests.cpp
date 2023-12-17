/**
 * @file type_checker_tests.cpp
 * @brief Adds tests for the type_checker class.
 * @details BASICTESTS is a suite for testing the basic functionality of the class while STRESSTESTS adds an extensive
 *          number of items before validating
 */

#include "../source/type_checker/type_checker.hpp"
#include "gtest/gtest.h"

/**
 * @brief setOrUpdateTest tests the setOrUpdate() method in TypeChecker.
 * @details First, the test will check whether the object already contains variables "cat", "dog", and "bird"
 *          Second, an initial value for "cat" and "dog" will be added and validated
 *          Finally, the values for the variables are updated and checked
 */
TEST(BASICTESTS, setOrUpdateTest) {
    TypeChecker testChecker = TypeChecker();

    EXPECT_FALSE(testChecker.hasVariable("cat"));
    EXPECT_FALSE(testChecker.hasVariable("dog"));
    EXPECT_FALSE(testChecker.hasVariable("bird"));

    testChecker.setOrUpdateType("cat", "INTEGER");
    testChecker.setOrUpdateType("dog", "FLOAT");

    EXPECT_TRUE(testChecker.hasVariable("cat"));
    EXPECT_TRUE(testChecker.hasVariable("dog"));
    EXPECT_FALSE(testChecker.hasVariable("bird"));
    EXPECT_TRUE(testChecker.getType("cat") == "INTEGER");
    EXPECT_TRUE(testChecker.getType("dog") == "FLOAT");

    testChecker.setOrUpdateType("cat", "STRING");
    testChecker.setOrUpdateType("dog", "NULL");
    EXPECT_TRUE(testChecker.getType("cat") == "STRING");
    EXPECT_TRUE(testChecker.getType("dog") == "NULL");
}

/**
 * @brief stringTypeTests is an extension of setOrUpdateTest which confirms the type has been updated properly
 */
TEST(BASICTESTS, stringTypeTests) {
    TypeChecker testChecker = TypeChecker();
    testChecker.setOrUpdateType("cat", "INTEGER");
    EXPECT_EQ("INTEGER", testChecker.getType("cat"));
    EXPECT_NE("FLOAT", testChecker.getType("cat"));

    testChecker.setOrUpdateType("cat", "FLOAT");
    EXPECT_EQ("FLOAT", testChecker.getType("cat"));
    EXPECT_NE("INTEGER", testChecker.getType("cat"));
}

/**
 * @brief verifyTests confirms the verifyType() method in TypeChecker
 * @details verifyType() is expected to yield TRUE for any variant of "INTEGER", "FLOAT", or "DOUBLE". Similarly, FALSE
 *          should be yielded if the specified variable doesn't exist.
 */
TEST(BASICTESTS, verifyTests) {
    TypeChecker testChecker = TypeChecker();
    testChecker.setOrUpdateType("cat", "INTEGER");
    EXPECT_TRUE(testChecker.verifyType("cat", "INTEGER"));
    EXPECT_FALSE(testChecker.verifyType("cat", "STRING"));

    EXPECT_FALSE(testChecker.hasVariable("dog"));
    EXPECT_FALSE(testChecker.verifyType("dog", "INTEGER"));
    EXPECT_FALSE(testChecker.verifyType("dog", "STRING"));
}

/**
 * @brief verifySimilarTypesTest is an extension of verifyTests. This test specifically verifies checking "INTEGER",
 *        "FLOAT", and "DOUBLE".
 * @details "cat" is assigned "INTEGER", "DOUBLE", and "STRING". The first tests are expected to be true while the final
 *          is expected to be false
 */
TEST(BASICTESTS, verifySimilarTypesTest) {
    TypeChecker testChecker = TypeChecker();
    testChecker.setOrUpdateType("cat", "INTEGER");
    EXPECT_TRUE(testChecker.verifyType("cat", "INTEGER"));
    EXPECT_TRUE(testChecker.verifyType("cat", "FLOAT"));
    EXPECT_TRUE(testChecker.verifyType("cat", "DOUBLE"));

    testChecker.setOrUpdateType("cat", "DOUBLE");
    EXPECT_TRUE(testChecker.verifyType("cat", "INTEGER"));
    EXPECT_TRUE(testChecker.verifyType("cat", "FLOAT"));
    EXPECT_TRUE(testChecker.verifyType("cat", "DOUBLE"));

    testChecker.setOrUpdateType("cat", "STRING");
    EXPECT_FALSE(testChecker.verifyType("cat", "INTEGER"));
    EXPECT_FALSE(testChecker.verifyType("cat", "FLOAT"));
    EXPECT_FALSE(testChecker.verifyType("cat", "DOUBLE"));
}

/**
 * @brief varsExist assigns 100 variables with the Type "INTEGER" and confirms whether they have been properly added.
 */
TEST(BASICTESTS, varsExist) {
    TypeChecker testChecker = TypeChecker();
    for (int currentVariable = 0; currentVariable < 100; currentVariable++) {
        testChecker.setOrUpdateType(std::to_string(currentVariable), "INTEGER");
    }
    for (int currentVariable = 0; currentVariable < 100; currentVariable++) {
        EXPECT_TRUE(testChecker.hasVariable(std::to_string(currentVariable)));
    }
}

/**
 * @brief stressTest100 adds 100 variables with an associated type and validates if they are equal
 * @details variables are assigned a key according to the order they were added and a dummy value. For example, item 43
 *          should have the variableID "43" and a type of "43"
 */
TEST(STRESSTESTS, stressTest100) {
    TypeChecker testChecker = TypeChecker();
    for (int currentString = 0; currentString < 100; currentString++) {
        testChecker.setOrUpdateType(std::to_string(currentString), std::to_string(currentString));
    }

    for (int currentString = 0; currentString < 100; currentString++) {
        EXPECT_EQ(std::to_string(currentString), testChecker.getType(std::to_string(currentString)));
    }
}

/**
 * @brief stressTest100000 is an extension of stressTest100, but checks more items compared to its predecessor
 */
TEST(STRESSTESTS, stressTest100000) {
    TypeChecker testChecker = TypeChecker();
    for (int currentString = 0; currentString < 100000; currentString++) {
        testChecker.setOrUpdateType(std::to_string(currentString), std::to_string(currentString));
    }

    for (int currentString = 0; currentString < 100000; currentString++) {
        EXPECT_EQ(std::to_string(currentString), testChecker.getType(std::to_string(currentString)));
    }
}

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
