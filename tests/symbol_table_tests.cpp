/**
 * @file symbol_table_tests
 * @brief symbol_table_tests contains the unit tests for the Scope and SymbolTableOrganizer classes
 * @details Tests are separated into three suites:
 *          SCOPETEST - tests the functionality of the Scope class
 *          SCOPESTRESSTEST - inserts a large number of items into a Scope object, perform an operation, and validates
 * the result SYMORGTEST - tests the functionality of the SymbolTableOrganizer class
 */

#include "source/symbol_table/scope.hpp"
#include "source/symbol_table/symbol_table_organizer.hpp"
#include "gtest/gtest.h"
#include <string>

// SCOPE TESTS

/**
 * @brief addToScope tests the functionality of addOrUpdateVariable()
 * @details An example variable is added to a Scope object. The test expects one value, but is not expecting a second
 * value
 */
TEST(SCOPETEST, addToScope) {
    Scope testScope = Scope();
    int testValue = 3;
    testScope.addOrUpdateVariable("cat", testValue);
    EXPECT_TRUE(testScope.hasLocalVariable("cat"));
    EXPECT_FALSE(testScope.hasLocalVariable("dog"));
}

/**
 * @brief scopeParents tests the parents of Scope objects
 * @details Two Scope objects are added:
 *          The first uses nullptr for it's parent
 *          The second's parent pointer redirects to the first Scope
 *
 *          The actual pointer of the first Scope is expected to equal the second Scope's parent
 */
TEST(SCOPETEST, scopeParents) {
    Scope parentScope = Scope();
    Scope* parentPointer = &parentScope;
    Scope childScope = Scope(parentPointer);

    EXPECT_EQ(nullptr, parentScope.getParentScopePointer());
    EXPECT_EQ(parentPointer, childScope.getParentScopePointer());
}

/**
 * @brief globalTest creates two constructors and checks their global status
 * @details Two Scope objects are created:
 *          The first Scope object has a nullptr parent
 *          The second Scope object redirects to the first Scope object
 *
 *          These objects run isGlobal with the expectation that a nullptr parent is the head of a symbol table
 */
TEST(SCOPETEST, globalTest) {
    Scope parentScope = Scope();
    Scope childScope = Scope(&parentScope);

    EXPECT_TRUE(parentScope.isGlobal());
    EXPECT_FALSE(childScope.isGlobal());

    Scope otherChildScope = Scope(childScope);

    EXPECT_TRUE(parentScope.isGlobal());
    EXPECT_FALSE(childScope.isGlobal());
    EXPECT_FALSE(otherChildScope.isGlobal());
}

/**
 * @brief getVariableTest adds a variable with an arbitrary value to a Scope object and verifies it with getVariable
 */
TEST(SCOPETEST, getVariableTest) {
    int catValue = 8;
    int updatedCatValue = 14;
    Scope testScope = Scope();

    testScope.addOrUpdateVariable("cat", catValue);
    EXPECT_EQ(8, *testScope.getVariable("cat"));

    testScope.addOrUpdateVariable("cat", updatedCatValue);
    EXPECT_EQ(14, *testScope.getVariable("cat"));
    EXPECT_NE(8, *testScope.getVariable("cat"));
}

/**
 * @brief copyScopeTest tests the copy constructor of the Scope object
 * @details A Scope object is created with an initial value. The copy constructor will be called and verify changes
 *          to one object does not impact the data of the other.
 */
TEST(SCOPETEST, copyScopeTest) {
    Scope testScope = Scope();
    testScope.addOrUpdateVariable("cat", 3);
    testScope.addOrUpdateVariable("dog", 2);

    Scope copiedScope = Scope(testScope);
    copiedScope.addOrUpdateVariable("bird", 8);

    EXPECT_TRUE(testScope.hasLocalVariable("cat"));
    EXPECT_TRUE(testScope.hasLocalVariable("dog"));
    EXPECT_TRUE(copiedScope.hasLocalVariable("cat"));
    EXPECT_TRUE(copiedScope.hasLocalVariable("dog"));
    EXPECT_FALSE(testScope.hasLocalVariable("bird"));
    EXPECT_TRUE(copiedScope.hasLocalVariable("bird"));

    copiedScope.addOrUpdateVariable("dog", 21);

    EXPECT_EQ(*testScope.getVariable("cat"), *copiedScope.getVariable("cat"));
    EXPECT_NE(*testScope.getVariable("dog"), *copiedScope.getVariable("dog"));
}

/**
 * @brief addStressTestSmall adds 100 variables to a Scope object and validates them
 */
TEST(SCOPESTRESSTEST, addStressTestSmall) {
    Scope testScope = Scope();
    for (int currentVariable = 0; currentVariable < 100; currentVariable++) {
        testScope.addOrUpdateVariable(std::to_string(currentVariable), currentVariable);
    }

    for (int currentVariable = 0; currentVariable < 100; currentVariable++) {
        EXPECT_EQ(currentVariable, *testScope.getVariable(std::to_string(currentVariable)));
    }
}

/**
 * @brief addStressTestLarge acts similar to it's smaller counterpart, but adds and validates 10000 items
 */
TEST(SCOPESTRESSTEST, addStressTestLarge) {
    Scope testScope = Scope();
    for (int currentVariable = 0; currentVariable < 10000; currentVariable++) {
        testScope.addOrUpdateVariable(std::to_string(currentVariable), currentVariable);
    }

    for (int currentVariable = 0; currentVariable < 10000; currentVariable++) {
        EXPECT_EQ(currentVariable, *testScope.getVariable(std::to_string(currentVariable)));
    }
}

// SYMBOLTABLEORGANIZER TESTS

/**
 * @brief pushTest adds a Scope to a SymbolTable object and validates the global status of the scope
 */
TEST(SYMORGTEST, pushTest) {
    SymbolTableOrganizer testOrg = SymbolTableOrganizer();
    testOrg.pushSymbolTable();

    EXPECT_TRUE(testOrg.isScopeGlobal());
    testOrg.pushSymbolTable();
    EXPECT_FALSE(testOrg.isScopeGlobal());
}

/**
 * @brief pushStressTest adds 100 Scope objects to a SymbolTableOrganizer and validates them
 */
TEST(SYMORGTEST, pushStressTest) {
    SymbolTableOrganizer testOrg = SymbolTableOrganizer();
    for (int currentScope = 0; currentScope < 100; currentScope++) { testOrg.pushSymbolTable(); }

    for (int currentScope = 100; currentScope > 0; currentScope--) {
        if (currentScope == 1) {
            EXPECT_TRUE(testOrg.isScopeGlobal());
        } else {
            EXPECT_FALSE(testOrg.isScopeGlobal());
        }
        testOrg.popSymbolTable();
    }
}

/**
 * @brief globalTests tests Four conditions where a global status may yield an incorrect result
 * @details Four cases are considered for the global status:
 *          No Scope objects are present (Expected TRUE)
 *          A single Scope is present (Expected TRUE)
 *          Two Scope objects are present (Expected FALSE)
 *          The second Scope is removed (Expected TRUE)
 */
TEST(SYMORGTEST, globalTests) {
    SymbolTableOrganizer testOrg = SymbolTableOrganizer();
    EXPECT_TRUE(testOrg.isScopeGlobal());

    testOrg.pushSymbolTable();
    EXPECT_TRUE(testOrg.isScopeGlobal());
    testOrg.pushSymbolTable();
    EXPECT_FALSE(testOrg.isScopeGlobal());

    testOrg.popSymbolTable();
    EXPECT_TRUE(testOrg.isScopeGlobal());
}

/**
 * @brief addItemsTest adds a variable to a SymbolTableOrganizer and validates it value
 */
TEST(SYMORGTEST, addItemsTest) {
    SymbolTableOrganizer testOrg = SymbolTableOrganizer();
    testOrg.pushSymbolTable();
    testOrg.getScope()->addOrUpdateVariable("cat", 3);  // first test: updating via the usage of getScope()
    EXPECT_TRUE(testOrg.getScope()->hasLocalVariable("cat"));
    EXPECT_EQ(*testOrg.getScope()->getVariable("cat"), 3);
    testOrg.addOrUpdateVariable("dog", 6);  // second test: updating via the usage of addVariable()
    EXPECT_TRUE(testOrg.getScope()->hasLocalVariable("dog"));
    EXPECT_EQ(*testOrg.getScope()->getVariable("dog"), 6);
}

/**
 * @brief addItemsStressTest adds 20 items to 5 Scope objects and validates their values
 * @details For each scope, 20 variables are added with an arbitrary value assigned to them. Five scopes are pushed in
 * total For each scope, a set of 20 variables are validated before calling popSymbolTable()
 */
TEST(SYMORGTEST, addItemsStressTest) {
    SymbolTableOrganizer testOrg = SymbolTableOrganizer();
    for (int currentScope = 0; currentScope < 5; currentScope++) {
        testOrg.pushSymbolTable();
        for (int currentVariable = 0; currentVariable < 20; currentVariable++) {
            testOrg.addOrUpdateVariable(std::to_string(currentVariable), currentVariable);
        }
    }

    for (int currentScope = 0; currentScope < 5; currentScope++) {
        for (int currentVariable = 0; currentVariable < 20; currentVariable++) {
            EXPECT_TRUE(testOrg.varExistsInCurrentSymbolTable(std::to_string(currentVariable)));
            EXPECT_EQ(*testOrg.getVariable(std::to_string(currentVariable)), currentVariable);
        }
        testOrg.popSymbolTable();
    }
}

/**
 * @brief recursionTest tests the recursive nature of hasGlobalVariable() and getVariable()
 * @details 100 Scope objects are pushed to the SymbolTableOrganizer:
 *          The first Scope contains a variable "cat" with any value
 *          99 empty Scopes are added in turn with each Scope referencing the previous
 *          When hasGlobalVariable() and getVariable() are called, the Scope is expected to return a value present in
 * the First Scope
 */
TEST(SYMORGTEST, recursionTest) {
    SymbolTableOrganizer testOrg = SymbolTableOrganizer();
    testOrg.pushSymbolTable();
    testOrg.addOrUpdateVariable("cat", 8);

    for (int currentScope = 0; currentScope < 99; currentScope++) { testOrg.pushSymbolTable(); }

    EXPECT_TRUE(testOrg.getScope()->hasGlobalVariable("cat"));
    EXPECT_EQ(*testOrg.getScope()->getVariable("cat"), 8);

    EXPECT_FALSE(testOrg.getScope()->hasGlobalVariable("dog"));
    EXPECT_EQ(testOrg.getScope()->getVariable("dog"), nullptr);
}

/**
 * @brief copyTest checks the validity of the copy constructor in SymbolTableOrganizer
 * @details Two variables are initialized to testOrg, "dog" and "bird". The copy constructor is called for the copiedOrg
 *          object. First, determine whether both variables exist in the objects. Next, add a new variable to copiedOrg
 *          and update a pre-existing variable. Finally, determine whether the original contents of testOrg have changed
 */
TEST(SYMORGTEST, copyTest) {
    SymbolTableOrganizer testOrg = SymbolTableOrganizer();
    testOrg.pushSymbolTable();
    testOrg.addOrUpdateVariable("bird", 64);
    testOrg.addOrUpdateVariable("dog", 36);

    SymbolTableOrganizer copiedOrg = SymbolTableOrganizer(testOrg);

    EXPECT_TRUE(testOrg.varExistsInScope("bird"));
    EXPECT_TRUE(copiedOrg.varExistsInScope("bird"));
    EXPECT_TRUE(testOrg.varExistsInScope("dog"));
    EXPECT_TRUE(copiedOrg.varExistsInScope("dog"));

    copiedOrg.addOrUpdateVariable("dog", 4);
    copiedOrg.addOrUpdateVariable("cat", 36);

    EXPECT_EQ(*testOrg.getVariable("bird"), *copiedOrg.getVariable("bird"));
    EXPECT_NE(*testOrg.getVariable("dog"), *copiedOrg.getVariable("dog"));
    EXPECT_FALSE(testOrg.varExistsInScope("cat"));
    EXPECT_TRUE(copiedOrg.varExistsInScope("cat"));
}

//Garbage Collection Tests
//Scope
TEST(GARBAGE_COLLECTION_SCOPE, hasExpiredTest) {
    Scope testScope = Scope();
    testScope.addOrUpdateVariable("cat", 255);
    testScope.setVariableOccurences("cat", 8);

    EXPECT_FALSE(testScope.hasExpired("cat"));
    for(int calls = 0; calls < 8; calls++) {
        testScope.decrementUses("cat");
    }
    EXPECT_TRUE(testScope.hasExpired("cat"));
    EXPECT_FALSE(testScope.hasExpired("dog"));
}

TEST(GARBAGE_COLLECTION_SCOPE, decrementTest) {
    Scope testScope = Scope();
}