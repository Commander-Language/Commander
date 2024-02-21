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

TypeChecker::CommanderInt Function::parseIntAsInt(TypeChecker::CommanderInt number) {
    return number;
}

TypeChecker::CommanderInt Function::parseFloatAsInt(TypeChecker::CommanderFloat number) {
    return (TypeChecker::CommanderInt) number;
}

TypeChecker::CommanderInt Function::parseBoolAsInt(TypeChecker::CommanderBool value) {
    return (TypeChecker::CommanderBool) value;
}

TypeChecker::CommanderInt Function::parseStringAsInt(TypeChecker::CommanderString string) {
    return -1; //TODO
}

TypeChecker::CommanderFloat Function::parseIntAsFloat(TypeChecker::CommanderInt number) {
    return (TypeChecker::CommanderFloat) number;
}

TypeChecker::CommanderFloat Function::parseFloatAsFloat(TypeChecker::CommanderFloat number) {
    return number;
}

TypeChecker::CommanderFloat Function::parseBoolAsFloat(TypeChecker::CommanderBool value) {
    return (TypeChecker::CommanderFloat) value;
}

TypeChecker::CommanderFloat Function::parseStringAsFloat(TypeChecker::CommanderString string) {
    return -1.0; //TODO
}

TypeChecker::CommanderBool Function::parseIntAsBool(TypeChecker::CommanderInt number) {
    return (TypeChecker::CommanderBool) number;
}

TypeChecker::CommanderBool Function::parseFloatAsBool(TypeChecker::CommanderFloat number) {
    return (TypeChecker::CommanderBool) number;
}

TypeChecker::CommanderBool Function::parseBoolAsBool(TypeChecker::CommanderBool value) {
    return value;
}

TypeChecker::CommanderBool Function::parseStringAsBool(TypeChecker::CommanderString string) {
    if(string == "true") {
        return true;
    }
    if(string == "false") {
        return false;
    }
    throw std::runtime_error(std::string("The passed parameter was not \"true\" or \"false\"."));
}

TypeChecker::CommanderFloat Function::sqrt(TypeChecker::CommanderInt value) {
    return std::sqrt(value); //TODO: int64_t may cause issues
}

TypeChecker::CommanderFloat Function::sqrt(TypeChecker::CommanderFloat value) {
    return std::sqrt(value);
}