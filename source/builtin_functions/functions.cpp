/**
 * @file functions.cpp
 * @brief //TODO
 */

#include "functions.hpp"
#include <iostream>

TypeChecker::CommanderTuple Function::println(TypeChecker::CommanderString string) {
    std::cout << string << std::endl;
}

TypeChecker::CommanderTuple Function::print(TypeChecker::CommanderString string) {
    std::cout << string;
}

TypeChecker::CommanderString Function::toString(TypeChecker::CommanderInt value) {
    return parseAsString(value);
}