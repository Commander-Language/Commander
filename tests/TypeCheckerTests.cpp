/**
 * TypeCheckerTests adds tests for the TypeChecker class.
 */

#include "TypeCheckerTests.hpp"

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

TEST(BASICTESTS, stringTypeTests) {
    TypeChecker testChecker = TypeChecker();
    testChecker.setOrUpdateType("cat", "INTEGER");
    EXPECT_EQ("INTEGER", testChecker.getType("cat"));
    EXPECT_NE("FLOAT", testChecker.getType("cat"));

    testChecker.setOrUpdateType("cat", "FLOAT");
    EXPECT_EQ("FLOAT", testChecker.getType("cat"));
    EXPECT_NE("INTEGER", testChecker.getType("cat"));
}

TEST(BASICTESTS, verifyTests) {
    TypeChecker testChecker = TypeChecker();
    testChecker.setOrUpdateType("cat", "INTEGER");
    EXPECT_TRUE(testChecker.verifyType("cat", "INTEGER"));
    EXPECT_FALSE(testChecker.verifyType("cat", "STRING"));

    EXPECT_FALSE(testChecker.hasVariable("dog"));
    EXPECT_FALSE(testChecker.verifyType("dog", "INTEGER"));
    EXPECT_FALSE(testChecker.verifyType("dog", "STRING"));
}

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

TEST(BASICTESTS, varsExist) {
    TypeChecker testChecker = TypeChecker();
    for(int currentVariable = 0; currentVariable < 100; currentVariable++) {
        testChecker.setOrUpdateType(std::to_string(currentVariable), "INTEGER");
    }
    for(int currentVariable = 0; currentVariable < 100; currentVariable++) {
        EXPECT_TRUE(testChecker.hasVariable(std::to_string(currentVariable)));
    }
}

TEST(STRESSTESTS, stressTest100) {
    TypeChecker testChecker = TypeChecker();
    for(int currentString = 0; currentString < 100; currentString++) {
        testChecker.setOrUpdateType(std::to_string(currentString), std::to_string(currentString));
    }

    for(int currentString = 0; currentString < 100; currentString++) {
        EXPECT_EQ(std::to_string(currentString), testChecker.getType(std::to_string(currentString)));
    }
}

TEST(STRESSTESTS, stressTest100000) {
    TypeChecker testChecker = TypeChecker();
    for(int currentString = 0; currentString < 100000; currentString++) {
        testChecker.setOrUpdateType(std::to_string(currentString), std::to_string(currentString));
    }

    for(int currentString = 0; currentString < 100000; currentString++) {
        EXPECT_EQ(std::to_string(currentString), testChecker.getType(std::to_string(currentString)));
    }
}

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
