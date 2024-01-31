/**
 * @file symbol_table_tests
 * @brief symbol_table_tests contains the unit tests for the Scope and SymbolTableOrganizer classes
 * @details Tests are separated into three suites:
 *          SCOPE_TEST - tests the functionality of the Scope class
 *          SCOPE_STRESS_TEST - inserts a large number of items into a Scope object, perform an operation, and validates the result
 *          SYMORG_TEST - tests the functionality of the SymbolTableOrganizer class
 *          GARBAGE_COLLECTION_SCOPE - tests the functionality of garbage collection logic in the Scope class
 *          GARBAGE_COLLECTION_SYMBOL_TABLE - tests the functionality of garbage collection logic in the SymbolTableOrganizer class
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
TEST(SCOPE_TEST, addToScope) {
    Scope testScope = Scope();
    int testValue = 3;
    testScope.addOrUpdateVariable("cat", testValue);
    EXPECT_TRUE(testScope.hasLocalVariable("cat"));
    EXPECT_FALSE(testScope.hasLocalVariable("dog"));
}

/**
 * @brief addPointerTest() checks if identical data between two different variables is shared
 * @details "bird" and "dog" are both initialized with a value of 8. These variables are expected to yield the same value, but not the same pointer
 */
TEST(SCOPE_TEST, addPointerTests) {
    Scope testScope = Scope();

    testScope.addOrUpdateVariable("bird", 8);
    testScope.addOrUpdateVariable("dog", 8);
    EXPECT_EQ(*testScope.getVariable("bird"), *testScope.getVariable("dog"));
    EXPECT_NE(testScope.getVariable("bird"), testScope.getVariable("dog"));

    testScope.addOrUpdateVariable("dog", 16);
    EXPECT_NE(*testScope.getVariable("bird"), *testScope.getVariable("dog"));
}

/**
 * @brief updateVariableTest() tests if the value of a provided variable updates without issue
 * @details First, we attempt to update a variable which does not exist. Next, we update a variable which exists and verifies it retains the new value
 */
TEST(SCOPE_TEST, updateVariableTest) {
    Scope testScope = Scope();
    EXPECT_NO_THROW(testScope.updateVariable("cat", 8));

    testScope.addOrUpdateVariable("cat", 255);
    testScope.updateVariable("cat", 16);

    EXPECT_EQ(*testScope.getVariable("cat"), 16);
    EXPECT_NE(*testScope.getVariable("cat"), 255);
}

/**
 * @brief updateVariableBooleanTest acts similarly to updateVariableTest. This test ensures the boolean values returned are accurate
 * @details FALSE is expected when a variable does not exist, TRUE is expected if the variable has been updated
 */
TEST(SCOPE_TEST, updateVariableBooleanTest) {
    Scope testScope = Scope();
    EXPECT_FALSE(testScope.updateVariable("birb", 16));

    testScope.addOrUpdateVariable("birb", 8);
    testScope.addOrUpdateVariable("dog", 16);

    EXPECT_TRUE(testScope.updateVariable("birb", 64));
    EXPECT_TRUE(testScope.updateVariable("dog", 8));
}

/**
 * @brief scopeParents tests the parents of Scope objects
 * @details Two Scope objects are added:
 *          The first uses nullptr for it's parent
 *          The second's parent pointer redirects to the first Scope
 *
 *          The actual pointer of the first Scope is expected to equal the second Scope's parent
 */
TEST(SCOPE_TEST, scopeParents) {
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
TEST(SCOPE_TEST, globalTest) {
    Scope parentScope = Scope();
    Scope childScope = Scope(&parentScope);

    EXPECT_TRUE(parentScope.isGlobal());
    EXPECT_FALSE(childScope.isGlobal());

    Scope otherChildScope = Scope(&childScope);

    EXPECT_TRUE(parentScope.isGlobal());
    EXPECT_FALSE(childScope.isGlobal());
    EXPECT_FALSE(otherChildScope.isGlobal());
}

/**
 * @brief getVariableTest adds a variable with an arbitrary value to a Scope object and verifies it with getVariable
 */
TEST(SCOPE_TEST, getVariableTest) {
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
TEST(SCOPE_TEST, copyScopeTest) {
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
TEST(SCOPE_STRESS_TEST, addStressTestSmall) {
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
TEST(SCOPE_STRESS_TEST, addStressTestLarge) {
    Scope testScope = Scope();
    for (int currentVariable = 0; currentVariable < 10000; currentVariable++) {
        testScope.addOrUpdateVariable(std::to_string(currentVariable), currentVariable);
    }

    for (int currentVariable = 0; currentVariable < 10000; currentVariable++) {
        EXPECT_EQ(currentVariable, *testScope.getVariable(std::to_string(currentVariable)));
    }
}

//SYMBOLTABLEORGANIZER TESTS

/**
 * @brief pushTest adds a Scope to a SymbolTable object and validates the global status of the scope
 */
TEST(SYMORG_TEST, pushTest) {
    SymbolTableOrganizer testOrg = SymbolTableOrganizer();
    testOrg.pushSymbolTable();

    EXPECT_TRUE(testOrg.isScopeGlobal());
    testOrg.pushSymbolTable();
    EXPECT_FALSE(testOrg.isScopeGlobal());
}

/**
 * @brief pushStressTest adds 100 Scope objects to a SymbolTableOrganizer and validates them
 */
TEST(SYMORG_TEST, pushStressTest) {
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
TEST(SYMORG_TEST, globalTests) {
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
TEST(SYMORG_TEST, addItemsTest) {
    SymbolTableOrganizer testOrg = SymbolTableOrganizer();
    testOrg.pushSymbolTable();
    testOrg.getScope()->addOrUpdateVariable("cat", 3);  // first test: updating via the usage of getScope()
    EXPECT_TRUE(testOrg.getScope()->hasLocalVariable("cat"));
    EXPECT_EQ(*testOrg.getScope()->getVariable("cat"), 3);
    testOrg.addOrUpdateVariable("dog", 6);  // second test: updating via the usage of addVariable()
    EXPECT_TRUE(testOrg.getScope()->hasLocalVariable("dog"));
    EXPECT_EQ(*testOrg.getScope()->getVariable("dog"), 6);

    testOrg.pushSymbolTable(); //third test: we'll add a new Scope to ensure cat is being properly updated instead of re-initializing
    testOrg.addOrUpdateVariable("cat", 8);
    EXPECT_EQ(*testOrg.getScope()->getVariable("cat"), 8);
    testOrg.popSymbolTable();
    EXPECT_EQ(*testOrg.getScope()->getVariable("cat"), 8);
}

/**
 * @brief addItemsStressTest adds 20 items to 5 Scope objects and validates their values
 * @details For each scope, 20 variables are added with an arbitrary value assigned to them. Five scopes are pushed in
 * total For each scope, a set of 20 variables are validated before calling popSymbolTable()
 */
TEST(SYMORG_TEST, addItemsStressTest) {
    SymbolTableOrganizer testOrg = SymbolTableOrganizer();
    testOrg.pushSymbolTable();
    for(int currentVariable = 0; currentVariable < 100; currentVariable++) {
        if((currentVariable != 0) && (currentVariable % 20 == 0)) {
            testOrg.pushSymbolTable(); //every 20 items, add a new Scope
        }
        testOrg.addOrUpdateVariable(std::to_string(currentVariable), currentVariable);
    }

    for(int currentVariable = 99; currentVariable > -1; currentVariable--) {
        EXPECT_TRUE(testOrg.varExistsInCurrentSymbolTable(std::to_string(currentVariable)));
        EXPECT_EQ(*testOrg.getVariable(std::to_string(currentVariable)), currentVariable);
        if((currentVariable != 0) && (currentVariable % 20 == 0)) {
            testOrg.popSymbolTable(); //every 20 items, remove a Scope
        }
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
TEST(SYMORG_TEST, recursionTest) {
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
TEST(SYMORG_TEST, copyTest) {
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

/**
 * @brief multiScopeCopyTest() tests the status of SymbolTableOrganizer's copy constructor on objects with multiple Scopes
 * @details One variable exists on each Scope in the organizer. A copy of the Symbol Table Organizer is created and its contents are modified. The contents of the copied organizer are not expected to impact the original symbol table organizer
 */
TEST(SYMORG_TEST, multiScopeCopyTest) {
    SymbolTableOrganizer testOrg = SymbolTableOrganizer();
    testOrg.pushSymbolTable();
    testOrg.addOrUpdateVariable("cat", 8);
    testOrg.pushSymbolTable();
    testOrg.addOrUpdateVariable("dog", 16);

    SymbolTableOrganizer copiedOrg = SymbolTableOrganizer(testOrg);
    copiedOrg.addOrUpdateVariable("cat", 16);
    copiedOrg.addOrUpdateVariable("dog", 32);

    EXPECT_NE(*testOrg.getVariable("cat"), *copiedOrg.getVariable("cat")); //data checks
    EXPECT_EQ(*copiedOrg.getVariable("cat"), 16);
    EXPECT_EQ(*testOrg.getVariable("cat"), 8);
    EXPECT_NE(*testOrg.getVariable("dog"), *copiedOrg.getVariable("dog"));
    EXPECT_EQ(*copiedOrg.getVariable("dog"), 32);
    EXPECT_EQ(*testOrg.getVariable("dog"), 16);
    EXPECT_NE(testOrg.getVariable("cat"), copiedOrg.getVariable("cat")); //pointer checks
    EXPECT_NE(testOrg.getVariable("dog"), copiedOrg.getVariable("dog"));
}

/**
 * @brief deepCopyTest() is functionally similar to multiScopeCopyTest(). This test includes 100 Scope objects
 * @details A single variable is created on each scope. The contents of the copied Symbol Table Organizer are modified and validated. The contents of one organizer should not affect the other
 */
TEST(SYMORG_TEST, deepCopyTest) {
    SymbolTableOrganizer testOrg = SymbolTableOrganizer();
    for(int currentScope = 0; currentScope < 100; currentScope++) {
        testOrg.pushSymbolTable();
        testOrg.addOrUpdateVariable(std::to_string(currentScope), currentScope);
    }

    SymbolTableOrganizer copiedOrg = SymbolTableOrganizer(testOrg);
    for(int currentScope = 0; currentScope < 100; currentScope++) {
        copiedOrg.addOrUpdateVariable(std::to_string(currentScope), currentScope + 1);
        EXPECT_NE(testOrg.getVariable(std::to_string(currentScope)), copiedOrg.getVariable(std::to_string(currentScope)));
        EXPECT_NE(*testOrg.getVariable(std::to_string(currentScope)), *copiedOrg.getVariable(std::to_string(currentScope)));
    }
}

//GARBAGE COLLECTION TESTS

/**
 * @brief hasExpiredTest checks the functionality of the hasExpired() method
 * @details A variable "cat" is initialized. It's value is set to 0 and other non-zero values. hasExpired is expected to
 * be true when the value equals zero.
 */
TEST(GARBAGE_COLLECTION_SCOPE, hasExpiredTest) {
    Scope testScope = Scope();
    testScope.addOrUpdateVariable("cat", 255);
    testScope.setVariableOccurrences("cat", 8);

    EXPECT_FALSE(testScope.hasExpired("cat"));
    for(int calls = 0; calls < 8; calls++) {
        testScope.decrementUses("cat");
    }
    EXPECT_TRUE(testScope.hasExpired("cat"));
    EXPECT_FALSE(testScope.hasExpired("dog"));
}

/**
 * @details decrementTest checks the functionality of the decrementUses() method
 */
TEST(GARBAGE_COLLECTION_SCOPE, decrementTest) {
    Scope testScope = Scope();
    testScope.addOrUpdateVariable("cat", 255);
    testScope.setVariableOccurrences("cat", 1);

    EXPECT_FALSE(testScope.hasExpired("cat"));
    testScope.decrementUses("cat"); //Should change 1 to 0
    EXPECT_TRUE(testScope.hasExpired("cat"));
    testScope.decrementUses("cat"); //Should not update from 0
    EXPECT_TRUE(testScope.hasExpired("cat"));
}

/**
 * @brief expiredTest is a small stress test for the hasExpired() method
 * @details 10 variables are given 3 occurrences in the Scope object. These are decremented and expected to expire after the third call of decrementUses()
 */
TEST(GARBAGE_COLLECTION_SCOPE, expiredTest) {
    Scope testScope = Scope();
    for(int currentVar = 0; currentVar < 10; currentVar++) {
        testScope.addOrUpdateVariable(std::to_string(currentVar), 1);
        testScope.setVariableOccurrences(std::to_string(currentVar), 3);
    }

    for(int currentVar = 0; currentVar < 10; currentVar++) {
        for(int currentDec = 0; currentDec < 4; currentDec++) {
            if(currentDec == 3) {
                EXPECT_TRUE(testScope.hasExpired(std::to_string(currentVar)));
            }
            else {
                EXPECT_FALSE(testScope.hasExpired(std::to_string(currentVar)));
            }
            testScope.decrementUses(std::to_string(currentVar));
        }
    }
}

/**
 * @brief setOccurrences() is not reccommended outside of variable initialization, but will be tested regardless
 * @details The variable "cat" has its number of occurrences modified. The variable is expected to expire when the number of occurrences is zero
 */
TEST(GARBAGE_COLLECTION_SCOPE, setOccurrencesTest) {
    Scope testScope = Scope();
    testScope.addOrUpdateVariable("cat", 128);
    testScope.setVariableOccurrences("cat", 8);
    EXPECT_FALSE(testScope.hasExpired("cat"));

    testScope.setVariableOccurrences("cat", 16);
    EXPECT_FALSE(testScope.hasExpired("cat"));

    testScope.setVariableOccurrences("cat", 0);
    EXPECT_TRUE(testScope.hasExpired("cat"));

    testScope.setVariableOccurrences("cat", 2);
    EXPECT_FALSE(testScope.hasExpired("cat"));
}

/**
 * @brief freeDataTest() checks the functionality of freeVariableData()
 * @details A variable "cat" is assigned a value. The test is expected to not throw an exception when freeVariableData() is called.
 */
TEST(GARBAGE_COLLECTION_SCOPE, freeDataTest) {
    Scope testScope = Scope();
    testScope.addOrUpdateVariable("cat", 48);
    ASSERT_NO_THROW(testScope.freeVariableData("cat")); //We shouldn't segfault here
    ASSERT_NO_THROW(testScope.freeVariableData("cat")); //We shouldn't throw here either after removing the data
}

/**
 * @brief freeDataSuccessfulTest() checks the functionality of boolean values returned from freeVariableData()
 * @details TRUE is expected from any reset of data, FALSE is expected when a variable does not exist
 */
TEST(GARBAGE_COLLECTION_SCOPE, freeDataSuccessfulTest) {
    Scope testScope = Scope();
    testScope.addOrUpdateVariable("cat", 48);
    ASSERT_TRUE(testScope.freeVariableData("cat")); //TRUE indicates we trashed the data
    ASSERT_TRUE(testScope.freeVariableData("cat")); //TRUE can also indicate we have already trashed data

    ASSERT_FALSE(testScope.freeVariableData("dog")); //We'll expect FALSE when data doesn't exist
}

/**
 * @brief tryFreeDataSuccessfullTest() tests the functionality of boolean values returned from SymbolTableOrganizer's tryFreevariableData() method
 * @details TRUE is expected for any variable which exists and has expired, FALSE is expected for variables which don't exist or have not expired
 */
TEST(GARBAGE_COLLECTION_SYMBOL_TABLE_ORGANIZER, tryFreeDataSuccessfulTest) {
    SymbolTableOrganizer organizer = SymbolTableOrganizer();
    organizer.pushSymbolTable();
    organizer.addOrUpdateVariable("cat", 8, 0);
    organizer.addOrUpdateVariable("dog", 16, 5);

    ASSERT_TRUE(organizer.tryFreeVariableData("cat"));
    ASSERT_FALSE(organizer.tryFreeVariableData("dog"));
}

/**
 * @brief tryFreeNestedDataSuccessfulTest() places two variables in different Scopes. They are expected to free without issues
 */
TEST(GARBAGE_COLLECTION_SYMBOL_TABLE_ORGANIZER, tryFreeNestedDataSuccessfulTest) {
    SymbolTableOrganizer organizer = SymbolTableOrganizer();
    organizer.pushSymbolTable();
    organizer.addOrUpdateVariable("cat", 8, 0);
    organizer.pushSymbolTable();
    organizer.addOrUpdateVariable("dog", 16, 5);

    ASSERT_TRUE(organizer.tryFreeVariableData("cat"));
    ASSERT_FALSE(organizer.tryFreeVariableData("dog"));
}

/**
 * @brief tryFreeDataTest() checks if no errors are thrown upon calling SymbolTableOrganizer's tryFreeData() method
 */
TEST(GARBAGE_COLLECTION_SYMBOL_TABLE_ORGANIZER, tryFreeDataTest) {
    SymbolTableOrganizer organizer = SymbolTableOrganizer();
    organizer.pushSymbolTable();
    organizer.addOrUpdateVariable("cat", 8, 0);
    organizer.pushSymbolTable();
    organizer.addOrUpdateVariable("dog", 16, 5);

    ASSERT_NO_THROW(organizer.tryFreeVariableData("cat"));
    ASSERT_NO_THROW(organizer.tryFreeVariableData("dog"));
}

/**
 * @brief forceFreeDataTest() checks if no errors are thrown upon calling SymbolTableOrganizer's forceFreeData() method
 */
TEST(GARBAGE_COLLECTION_SYMBOL_TABLE_ORGANIZER, forceFreeDataTest) {
    SymbolTableOrganizer organizer = SymbolTableOrganizer();
    organizer.pushSymbolTable();
    organizer.addOrUpdateVariable("cat", 8, 0);
    organizer.pushSymbolTable();
    organizer.addOrUpdateVariable("dog", 16, 5);

    ASSERT_NO_THROW(organizer.forceFreeVariableData("cat"));
    ASSERT_NO_THROW(organizer.forceFreeVariableData("dog"));
}

/**
 * @brief expirationTest() checks SymbolTableOrganizer's variableHasExpired() method
 * @details "cat" is expected to expire after calling getVariable(). "dog" and "birb" are expected to not expire. All variables are present in different Scope levels
 */
TEST(GARBAGE_COLLECTION_SYMBOL_TABLE_ORGANIZER, expirationTest) {
    SymbolTableOrganizer organizer = SymbolTableOrganizer();
    organizer.pushSymbolTable();
    organizer.addOrUpdateVariable("cat", 8, 1);
    organizer.pushSymbolTable();
    organizer.addOrUpdateVariable("dog", 16, 2);
    organizer.pushSymbolTable();
    organizer.addOrUpdateVariable("birb", 32, 3);

    ASSERT_FALSE(organizer.variableHasExpired("cat"));
    ASSERT_FALSE(organizer.variableHasExpired("dog"));
    ASSERT_FALSE(organizer.variableHasExpired("birb"));

    organizer.getVariable("cat"); //We won't do anything with this data, we just need to decrement uses
    organizer.getVariable("dog");
    organizer.getVariable("birb");

    ASSERT_TRUE(organizer.variableHasExpired("cat"));
    ASSERT_FALSE(organizer.variableHasExpired("dog"));
    ASSERT_FALSE(organizer.variableHasExpired("birb"));
}

// GENERIC DATA TYPE TESTS

//TEST(ANY_DATA, alternateIntTest) {
//    Scope testScope = Scope();
//    testScope.addOrUpdateVariable("cat", 8);
//
//    EXPECT_NO_THROW(testScope.getVariableAsType<int>("cat")); //Shouldn't throw an error here
//    EXPECT_EQ(*testScope.getVariable("cat"), 8);
//    EXPECT_EQ(testScope.getVariableAsType<int>("cat"), 8);
//}