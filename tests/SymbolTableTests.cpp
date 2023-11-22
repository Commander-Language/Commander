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
    EXPECT_TRUE(testScope.hasVariable("cat"));
    EXPECT_FALSE(testScope.hasVariable("dog"));
}

TEST(SCOPETEST, scopeParents) {
    Scope parentScope = Scope();
    Scope childScope = Scope(parentScope);

    EXPECT_EQ(nullptr, parentScope.getParentScopePointer());
    //EXPECT_EQ(parentScope, childScope.getParentScopePointer());
}

TEST(SCOPETEST, globalTest) {
    Scope parentScope = Scope();
    Scope childScope = Scope(parentScope);

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
//
//TEST(SCOPESTRESSTEST, addStressTestSmall) {
//    Scope testScope = Scope();
//    for(int currentVariable = 0; currentVariable < 100; currentVariable++) {
//        testScope.addOrUpdateVariable(std::to_string(currentVariable), currentVariable);
//    }
//
//    for(int currentVariable = 0; currentVariable < 100; currentVariable++) {
//        EXPECT_EQ(currentVariable, *testScope.getVariable(std::to_string(currentVariable)));
//    }
//}
//
//TEST(SCOPESTRESSTEST, addStressTestLarge) {
//    Scope testScope = Scope();
//    for(int currentVariable = 0; currentVariable < 10000; currentVariable++) {
//        testScope.addOrUpdateVariable(std::to_string(currentVariable), currentVariable);
//    }
//
//    for(int currentVariable = 0; currentVariable < 10000; currentVariable++) {
//        EXPECT_EQ(currentVariable, *testScope.getVariable(std::to_string(currentVariable)));
//    }
//}
//
//TEST(SCOPESTRESSTEST, recursionTest) {
//    Scope head = Scope();
//    int testData = 38;
//    head.addOrUpdateVariable("cat", testData);
//    Scope* previousScope = *head;
//    Scope* currentScope = *head;
//    for(int currentScope = 0; currentScope < 100; currentScope++) {
//        Scope newScope = Scope(previousScope);
//        currentScope = *newScope; //may need to change
//        previousScope = *newScope;
//    }
//
//    EXPECT_TRUE(*currentScope.hasVariable("cat"));
//    EXPECT_FALSE(*currentScope.hasVariable("dog"));
//}
//
//
////SYMBOLTABLEORGANIZER TESTS
//
//TEST(SYMORGTEST, pushTest) {
//    SymbolTableOrganizer testOrg = SymbolTableOrganizer();
//    testOrg.pushSymbolTable();
//
//    EXPECT_TRUE(testOrg.isScopeGlobal());
//    testOrg.pushSymbolTable();
//    EXPECT_FALSE(testOrg.isScopeGlobal());
//}