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
    TypeChecker::VariableTable table;

    EXPECT_FALSE(table.hasVariable("cat"));
    EXPECT_FALSE(table.hasVariable("dog"));
    EXPECT_FALSE(table.hasVariable("bird"));

    table.addVariable("cat", makeType<TypeChecker::IntTy>());
    table.addVariable("dog", makeType<TypeChecker::FloatTy>());

    EXPECT_TRUE(table.hasVariable("cat"));
    EXPECT_TRUE(table.hasVariable("dog"));
    EXPECT_FALSE(table.hasVariable("bird"));
    EXPECT_EQ(TypeChecker::INT, table.getVariable("cat")->types[0]->getType());
    EXPECT_EQ(TypeChecker::FLOAT, table.getVariable("dog")->types[0]->getType());

    table.addVariable("cat", makeType<TypeChecker::StringTy>());
    table.addVariable("dog", makeType<TypeChecker::BoolTy>());
    EXPECT_EQ(TypeChecker::STRING, table.getVariable("cat")->types[0]->getType());
    EXPECT_EQ(TypeChecker::BOOL, table.getVariable("dog")->types[0]->getType());
}

/**
 * @brief stringTypeTests is an extension of setOrUpdateTest which confirms the type has been updated properly
 */
TEST(BASICTESTS, stringTypeTests) {
    TypeChecker::VariableTable table;
    table.addVariable("cat", makeType<TypeChecker::IntTy>());
    EXPECT_EQ(TypeChecker::INT, table.getVariable("cat")->types[0]->getType());
    EXPECT_NE(TypeChecker::FLOAT, table.getVariable("cat")->types[0]->getType());

    table.addVariable("cat", makeType<TypeChecker::FloatTy>());
    EXPECT_EQ(TypeChecker::FLOAT, table.getVariable("cat")->types[0]->getType());
    EXPECT_NE(TypeChecker::INT, table.getVariable("cat")->types[0]->getType());
}

/**
 * @brief stressTest100 assigns 100 variables with the Type "INTEGER" and confirms whether they have been properly added
 * with the expected type.
 */
TEST(BASICTESTS, stressTest100) {
    TypeChecker::VariableTable table;
    for (int currentVariable = 0; currentVariable < 100; currentVariable++) {
        table.addVariable(std::to_string(currentVariable), makeType<TypeChecker::IntTy>());
    }
    for (int currentVariable = 0; currentVariable < 100; currentVariable++) {
        EXPECT_TRUE(table.hasVariable(std::to_string(currentVariable)));
        EXPECT_EQ(TypeChecker::INT, table.getVariable(std::to_string(currentVariable))->types[0]->getType());
    }
}

/**
 * @brief stressTest100000 is an extension of stressTest100, but checks more items compared to its predecessor
 */
TEST(STRESSTESTS, stressTest100000) {
    TypeChecker::VariableTable table;
    for (int currentVariable = 0; currentVariable < 100000; currentVariable++) {
        table.addVariable(std::to_string(currentVariable), makeType<TypeChecker::IntTy>());
    }
    for (int currentVariable = 0; currentVariable < 100000; currentVariable++) {
        EXPECT_TRUE(table.hasVariable(std::to_string(currentVariable)));
        EXPECT_EQ(TypeChecker::INT, table.getVariable(std::to_string(currentVariable))->types[0]->getType());
    }
}

/**
 * Runs the type checker tests from the tests/files/type_checker_tests/should_type_check directory
 */
TEST_P(TypeCheckerPassTests, ShouldTypeCheckFileAndMatchExpectedExamples) {
    auto params = GetParam();

    typeChecker = TypeChecker::TypeChecker(parser);

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
    typeChecker = TypeChecker::TypeChecker(parser);
    const std::string filePath = "../tests/files/type_checker_tests/should_fail/" + param;
    ASSERT_THROW(lexParseAndTypeCheck(filePath), Util::CommanderException);
}

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
