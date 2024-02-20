/**
 * @file type_checker_tests.cpp
 * @brief Adds tests for the type_checker class.
 * @details BASICTESTS is a suite for testing the basic functionality of the class while STRESSTESTS adds an extensive
 *          number of items before validating
 */

#include "type_checker_tests.hpp"

void expectOutputEqualsSExpressions(const Parser::ASTNodeList& nodes, const std::string& expectedOutput) {
    // TODO: Add test util class that include all test helpers
    std::ostringstream builder;
    for (const Parser::ASTNodePtr& node : nodes) builder << node->sExpression() << "\n";
    EXPECT_EQ(expectedOutput, builder.str());
}

void lexParseAndTypeCheck(const std::string& filePath) {
    Lexer::TokenList tokens;
    Lexer::tokenize(tokens, filePath);
    Parser::ASTNodeList nodes = parser.parse(tokens);
    typeChecker.typeCheck(nodes);
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

/**
 * Runs the type checker tests from the tests/files/type_checker_tests/should_type_check directory
 */
TEST_P(TypeCheckerPassTests, ShouldTypeCheckFileAndMatchExpectedExamples) {
    auto params = GetParam();

    const std::string filePath = "../tests/files/type_checker_tests/should_type_check/" + std::get<0>(params);
    const std::string expectedFilePath = "../tests/files/type_checker_tests/should_type_check/" + std::get<1>(params);

    // Lex
    Lexer::TokenList tokens;
    try {
        Lexer::tokenize(tokens, filePath);
    } catch (const Util::CommanderException& e) {
        std::cout << "Lexer Error: " << e.what() << "\n";
        FAIL();
    }

    // Parse
    Parser::ASTNodeList nodes;
    try {
        nodes = parser.parse(tokens);
    } catch (const Util::CommanderException& e) {
        std::cout << "Parser Error: " << e.what() << "\n";
        FAIL();
    }

    // Type Check
    try {
        typeChecker.typeCheck(nodes);
        const std::string expectedOutput = Lexer::readFile(expectedFilePath);
        expectOutputEqualsSExpressions(nodes, expectedOutput);
    } catch (const Util::CommanderException& e) {
        std::cout << "Type Checker Error: " << e.what() << "\n";
        FAIL();
    }
}

/**
 * Runs the type checker tests from the tests/files/type_checker_tests/should_fail directory
 */
TEST_P(TypeCheckerFailTests, ShouldNotTypeCheckFile) {
    auto param = GetParam();
    const std::string filePath = "../tests/files/type_checker_tests/should_fail/" + param;
    ASSERT_THROW(lexParseAndTypeCheck(filePath), Util::CommanderException);
}

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
