/**
 * @file type_checker_tests.cpp
 * @brief Adds tests for the type_checker class.
 * @details BASICTESTS is a suite for testing the basic functionality of the class while STRESSTESTS adds an extensive
 *          number of items before validating
 */

#include "../source/type_checker/type_checker.hpp"
#include "gtest/gtest.h"

template<typename TType>
std::shared_ptr<TType> makeType() {
    return std::make_shared<TType>();
}

/**
 * @brief setOrUpdateTest tests the setOrUpdate() method in TypeChecker.
 * @details First, the test will check whether the object already contains variables "cat", "dog", and "bird"
 *          Second, an initial value for "cat" and "dog" will be added and validated
 *          Finally, the values for the variables are updated and checked
 */
TEST(BASICTESTS, setOrUpdateTest) {
    TypeChecker::TypeChecker testChecker = TypeChecker::TypeChecker();

    EXPECT_FALSE(testChecker.hasVariable("cat"));
    EXPECT_FALSE(testChecker.hasVariable("dog"));
    EXPECT_FALSE(testChecker.hasVariable("bird"));

    testChecker.setOrUpdateType("cat", makeType<TypeChecker::IntTy>());
    testChecker.setOrUpdateType("dog", makeType<TypeChecker::FloatTy>());

    EXPECT_TRUE(testChecker.hasVariable("cat"));
    EXPECT_TRUE(testChecker.hasVariable("dog"));
    EXPECT_FALSE(testChecker.hasVariable("bird"));
    EXPECT_EQ(TypeChecker::INT, testChecker.getType("cat")->getType());
    EXPECT_EQ(TypeChecker::FLOAT, testChecker.getType("dog")->getType());

    testChecker.setOrUpdateType("cat", makeType<TypeChecker::StringTy>());
    testChecker.setOrUpdateType("dog", makeType<TypeChecker::BoolTy>());
    EXPECT_EQ(TypeChecker::STRING, testChecker.getType("cat")->getType());
    EXPECT_EQ(TypeChecker::BOOL, testChecker.getType("dog")->getType());
}

/**
 * @brief stringTypeTests is an extension of setOrUpdateTest which confirms the type has been updated properly
 */
TEST(BASICTESTS, stringTypeTests) {
    TypeChecker::TypeChecker testChecker = TypeChecker::TypeChecker();
    testChecker.setOrUpdateType("cat", makeType<TypeChecker::IntTy>());
    EXPECT_EQ(TypeChecker::INT, testChecker.getType("cat")->getType());
    EXPECT_NE(TypeChecker::FLOAT, testChecker.getType("cat")->getType());

    testChecker.setOrUpdateType("cat", makeType<TypeChecker::FloatTy>());
    EXPECT_EQ(TypeChecker::FLOAT, testChecker.getType("cat")->getType());
    EXPECT_NE(TypeChecker::INT, testChecker.getType("cat")->getType());
}

/**
 * @brief verifyTests confirms the verifyType() method in TypeChecker
 * @details verifyType() is expected to yield TRUE for any variant of "INTEGER", "FLOAT", or "DOUBLE". Similarly, FALSE
 *          should be yielded if the specified variable doesn't exist.
 */
TEST(BASICTESTS, verifyTests) {
    TypeChecker::TypeChecker testChecker = TypeChecker::TypeChecker();
    testChecker.setOrUpdateType("cat", makeType<TypeChecker::IntTy>());
    EXPECT_TRUE(testChecker.verifyType("cat", makeType<TypeChecker::IntTy>()));
    EXPECT_FALSE(testChecker.verifyType("cat", makeType<TypeChecker::StringTy>()));

    EXPECT_FALSE(testChecker.hasVariable("dog"));
    EXPECT_FALSE(testChecker.verifyType("dog", makeType<TypeChecker::IntTy>()));
    EXPECT_FALSE(testChecker.verifyType("dog", makeType<TypeChecker::StringTy>()));
}

/**
 * @brief verifySimilarTypesTest is an extension of verifyTests. This test specifically verifies checking "INTEGER",
 *        "FLOAT", and "DOUBLE".
 * @details "cat" is assigned "INTEGER", "DOUBLE", and "STRING". The first tests are expected to be true while the final
 *          is expected to be false
 */
TEST(BASICTESTS, verifySimilarTypesTest) {
    TypeChecker::TypeChecker testChecker = TypeChecker::TypeChecker();
    testChecker.setOrUpdateType("cat", makeType<TypeChecker::IntTy>());
    EXPECT_TRUE(testChecker.verifyType("cat", makeType<TypeChecker::IntTy>()));
    EXPECT_TRUE(testChecker.verifyType("cat", makeType<TypeChecker::FloatTy>()));

    testChecker.setOrUpdateType("cat", makeType<TypeChecker::FloatTy>());
    EXPECT_TRUE(testChecker.verifyType("cat", makeType<TypeChecker::IntTy>()));
    EXPECT_TRUE(testChecker.verifyType("cat", makeType<TypeChecker::FloatTy>()));

    testChecker.setOrUpdateType("cat", makeType<TypeChecker::StringTy>());
    EXPECT_FALSE(testChecker.verifyType("cat", makeType<TypeChecker::IntTy>()));
    EXPECT_FALSE(testChecker.verifyType("cat", makeType<TypeChecker::FloatTy>()));
}

/**
 * @brief stressTest100 assigns 100 variables with the Type "INTEGER" and confirms whether they have been properly added
 * with the expected type.
 */
TEST(BASICTESTS, stressTest100) {
    TypeChecker::TypeChecker testChecker = TypeChecker::TypeChecker();
    for (int currentVariable = 0; currentVariable < 100; currentVariable++) {
        testChecker.setOrUpdateType(std::to_string(currentVariable), makeType<TypeChecker::IntTy>());
    }
    for (int currentVariable = 0; currentVariable < 100; currentVariable++) {
        EXPECT_TRUE(testChecker.hasVariable(std::to_string(currentVariable)));
        EXPECT_EQ(TypeChecker::INT, testChecker.getType(std::to_string(currentVariable))->getType());
    }
}

/**
 * @brief stressTest100000 is an extension of stressTest100, but checks more items compared to its predecessor
 */
TEST(STRESSTESTS, stressTest100000) {
    TypeChecker::TypeChecker testChecker = TypeChecker::TypeChecker();
    for (int currentVariable = 0; currentVariable < 100000; currentVariable++) {
        testChecker.setOrUpdateType(std::to_string(currentVariable), makeType<TypeChecker::IntTy>());
    }
    for (int currentVariable = 0; currentVariable < 100000; currentVariable++) {
        EXPECT_TRUE(testChecker.hasVariable(std::to_string(currentVariable)));
        EXPECT_EQ(TypeChecker::INT, testChecker.getType(std::to_string(currentVariable))->getType());
    }
}

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
