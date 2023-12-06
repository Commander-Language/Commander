/**
 * SymbolTableTests contains test cases for the SymbolTableOrganizer and Scope classes.
 */

#include "SymbolTableTests.hpp"
#include <string>
#include "source/symbol_table/Scope.hpp"
#include "source/symbol_table/SymbolTableOrganizer.hpp"
#include "gtest/gtest.h"

//SCOPE TESTS
TEST(SCOPETEST, addToScope) {
    Scope testScope = Scope();
    int testValue = 3;
    testScope.addOrUpdateVariable("cat", testValue);
    EXPECT_TRUE(testScope.hasLocalVariable("cat"));
    EXPECT_FALSE(testScope.hasLocalVariable("dog"));
}

TEST(SCOPETEST, scopeParents) {
    Scope parentScope = Scope();
    Scope* parentPointer = &parentScope;
    Scope childScope = Scope(parentPointer);

    EXPECT_EQ(nullptr, parentScope.getParentScopePointer());
    EXPECT_EQ(parentPointer, childScope.getParentScopePointer());
}

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

TEST(SCOPESTRESSTEST, addStressTestSmall) {
    Scope testScope = Scope();
    for(int currentVariable = 0; currentVariable < 100; currentVariable++) {
        testScope.addOrUpdateVariable(std::to_string(currentVariable), currentVariable);
    }

    for(int currentVariable = 0; currentVariable < 100; currentVariable++) {
        EXPECT_EQ(currentVariable, *testScope.getVariable(std::to_string(currentVariable)));
    }
}

TEST(SCOPESTRESSTEST, addStressTestLarge) {
    Scope testScope = Scope();
    for(int currentVariable = 0; currentVariable < 10000; currentVariable++) {
        testScope.addOrUpdateVariable(std::to_string(currentVariable), currentVariable);
    }

    for(int currentVariable = 0; currentVariable < 10000; currentVariable++) {
        EXPECT_EQ(currentVariable, *testScope.getVariable(std::to_string(currentVariable)));
    }
}

////SYMBOLTABLEORGANIZER TESTS

TEST(SYMORGTEST, pushTest) {
    SymbolTableOrganizer testOrg = SymbolTableOrganizer();
    testOrg.pushSymbolTable();

    EXPECT_TRUE(testOrg.isScopeGlobal());
    testOrg.pushSymbolTable();
    EXPECT_FALSE(testOrg.isScopeGlobal());
}

TEST(SYMORGTEST, pushStressTest) {
    SymbolTableOrganizer testOrg = SymbolTableOrganizer();
    for(int currentScope = 0; currentScope < 100; currentScope++) {
        testOrg.pushSymbolTable();
    }

    for(int currentScope = 100; currentScope > 0; currentScope--) {
        if(currentScope == 1) {
            EXPECT_TRUE(testOrg.isScopeGlobal());
        }
        else {
            EXPECT_FALSE(testOrg.isScopeGlobal());
        }
        testOrg.popSymbolTable();
    }
}

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

TEST(SYMORGTEST, addItemsTest) {
    SymbolTableOrganizer testOrg = SymbolTableOrganizer();
    testOrg.pushSymbolTable();
    testOrg.getScope()->addOrUpdateVariable("cat", 3); //first test: updating via the usage of getScope()
    EXPECT_TRUE(testOrg.getScope()->hasLocalVariable("cat"));
    EXPECT_EQ(*testOrg.getScope()->getVariable("cat"), 3);
    testOrg.addOrUpdateVariable("dog", 6); //second test: updating via the usage of addVariable()
    EXPECT_TRUE(testOrg.getScope()->hasLocalVariable("dog"));
    EXPECT_EQ(*testOrg.getScope()->getVariable("dog"), 6);
}

TEST(SYMORGTEST, addItemsStressTest) {
    SymbolTableOrganizer testOrg = SymbolTableOrganizer();
    for(int currentScope = 0; currentScope < 5; currentScope++) {
        testOrg.pushSymbolTable();
        for(int currentVariable = 0; currentVariable < 20; currentVariable++) {
            testOrg.addOrUpdateVariable(std::to_string(currentVariable), currentVariable);
        }
    }

    for(int currentScope = 0; currentScope < 5; currentScope++) {
        for(int currentVariable = 0; currentVariable < 20; currentVariable++) {
            EXPECT_TRUE(testOrg.varExistsInCurrentSymbolTable(std::to_string(currentVariable)));
            EXPECT_EQ(*testOrg.getVariable(std::to_string(currentVariable)), currentVariable);
        }
        testOrg.popSymbolTable();
    }
}

TEST(SYMORGTEST, recursionTest) {
    SymbolTableOrganizer testOrg = SymbolTableOrganizer();
    testOrg.pushSymbolTable();
    testOrg.addOrUpdateVariable("cat", 8);

    for(int currentScope = 0; currentScope < 99; currentScope++) {
        testOrg.pushSymbolTable();
    }

    EXPECT_TRUE(testOrg.getScope()->hasGlobalVariable("cat"));
    EXPECT_EQ(*testOrg.getScope()->getVariable("cat"), 8);
}