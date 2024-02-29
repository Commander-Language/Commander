/**
 * @file functions.cpp
 * @brief //TODO
 */

#include "functions.hpp"
#include <iostream>

void Function::println(TypeChecker::CommanderString string) { std::cout << string << std::endl; }

void Function::print(TypeChecker::CommanderString string) { std::cout << string; }

TypeChecker::CommanderString Function::toString(TypeChecker::CommanderInt value) { return parseAsString(value); }

TypeChecker::CommanderInt Function::parseIntAsInt(TypeChecker::CommanderInt number) { return number; }

TypeChecker::CommanderInt Function::parseFloatAsInt(TypeChecker::CommanderFloat number) {
    return (TypeChecker::CommanderInt)number;
}

TypeChecker::CommanderInt Function::parseBoolAsInt(TypeChecker::CommanderBool value) {
    return (TypeChecker::CommanderBool)value;
}

TypeChecker::CommanderInt Function::parseStringAsInt(TypeChecker::CommanderString string) {
    return (TypeChecker::CommanderInt)stoi(string);
}

TypeChecker::CommanderFloat Function::parseIntAsFloat(TypeChecker::CommanderInt number) {
    return (TypeChecker::CommanderFloat)number;
}

TypeChecker::CommanderFloat Function::parseFloatAsFloat(TypeChecker::CommanderFloat number) { return number; }

TypeChecker::CommanderFloat Function::parseBoolAsFloat(TypeChecker::CommanderBool value) {
    return (TypeChecker::CommanderFloat)value;
}

TypeChecker::CommanderFloat Function::parseStringAsFloat(TypeChecker::CommanderString string) {
    return (TypeChecker::CommanderFloat)stod(string);
}

TypeChecker::CommanderBool Function::parseIntAsBool(TypeChecker::CommanderInt number) {
    return (TypeChecker::CommanderBool)number;
}

TypeChecker::CommanderBool Function::parseFloatAsBool(TypeChecker::CommanderFloat number) {
    return (TypeChecker::CommanderBool)number;
}

TypeChecker::CommanderBool Function::parseBoolAsBool(TypeChecker::CommanderBool value) { return value; }

TypeChecker::CommanderBool Function::parseStringAsBool(TypeChecker::CommanderString string) {
    if (string == "true") { return true; }
    if (string == "false") { return false; }
    throw std::runtime_error(std::string("The passed parameter was not \"true\" or \"false\"."));
}

TypeChecker::CommanderFloat Function::sqrt(TypeChecker::CommanderInt value) {
    return std::sqrt(value);  // TODO: int64_t may cause issues
}

TypeChecker::CommanderFloat Function::sqrt(TypeChecker::CommanderFloat value) { return std::sqrt(value); }

TypeChecker::CommanderFloat Function::ln(TypeChecker::CommanderInt value) {
    return std::log((TypeChecker::CommanderFloat)value);
}

TypeChecker::CommanderFloat Function::ln(TypeChecker::CommanderFloat value) { return std::log(value); }

TypeChecker::CommanderFloat Function::log(TypeChecker::CommanderInt value) {
    return std::log10((TypeChecker::CommanderFloat)value);
}

TypeChecker::CommanderFloat Function::log(TypeChecker::CommanderFloat value) { return std::log10(value); }

TypeChecker::CommanderInt Function::abs(TypeChecker::CommanderInt value) { return std::abs(value); }

TypeChecker::CommanderFloat Function::abs(TypeChecker::CommanderFloat value) { return std::abs(value); }

TypeChecker::CommanderInt Function::floor(TypeChecker::CommanderInt value) {
    return std::floor((TypeChecker::CommanderFloat)value);
}

TypeChecker::CommanderInt Function::floor(TypeChecker::CommanderFloat value) { return std::floor(value); }

TypeChecker::CommanderInt Function::ceil(TypeChecker::CommanderInt value) {
    return std::ceil((TypeChecker::CommanderFloat)value);
}

TypeChecker::CommanderInt Function::ceil(TypeChecker::CommanderFloat value) { return std::ceil(value); }

TypeChecker::CommanderInt Function::round(TypeChecker::CommanderInt value) {
    return std::round((TypeChecker::CommanderFloat)value);
}

TypeChecker::CommanderInt Function::round(TypeChecker::CommanderFloat value) { return std::round(value); }

TypeChecker::CommanderFloat Function::sin(TypeChecker::CommanderInt value) {
    return std::sin((TypeChecker::CommanderFloat)value);
}

TypeChecker::CommanderFloat Function::sin(TypeChecker::CommanderFloat value) { return std::sin(value); }

TypeChecker::CommanderFloat Function::cos(TypeChecker::CommanderInt value) {
    return std::cos((TypeChecker::CommanderFloat)value);
}

TypeChecker::CommanderFloat Function::cos(TypeChecker::CommanderFloat value) { return std::cos(value); }

TypeChecker::CommanderFloat Function::tan(TypeChecker::CommanderInt value) {
    return std::tan((TypeChecker::CommanderFloat)value);
}

TypeChecker::CommanderFloat Function::tan(TypeChecker::CommanderFloat value) { return std::tan(value); }

TypeChecker::CommanderFloat Function::csc(TypeChecker::CommanderInt value) {
    return 1 / std::sin((TypeChecker::CommanderFloat)value);
}

TypeChecker::CommanderFloat Function::csc(TypeChecker::CommanderFloat value) { return 1 / std::sin(value); }

TypeChecker::CommanderFloat Function::sec(TypeChecker::CommanderInt value) {
    return 1 / std::cos((TypeChecker::CommanderFloat)value);
}

TypeChecker::CommanderFloat Function::sec(TypeChecker::CommanderFloat value) { return 1 / std::cos(value); }

TypeChecker::CommanderFloat Function::cot(TypeChecker::CommanderInt value) {
    return std::cos((TypeChecker::CommanderFloat)value) / std::sin((TypeChecker::CommanderFloat)value);
}

TypeChecker::CommanderFloat Function::cot(TypeChecker::CommanderFloat value) {
    return std::cos(value) / std::sin(value);
}

TypeChecker::CommanderFloat Function::sinh(TypeChecker::CommanderInt value) {
    return std::sinh((TypeChecker::CommanderFloat)value);
}

TypeChecker::CommanderFloat Function::sinh(TypeChecker::CommanderFloat value) { return std::sinh(value); }

TypeChecker::CommanderFloat Function::cosh(TypeChecker::CommanderInt value) {
    return std::cosh((TypeChecker::CommanderFloat)value);
}

TypeChecker::CommanderFloat Function::cosh(TypeChecker::CommanderFloat value) { return std::cosh(value); }

TypeChecker::CommanderFloat Function::tanh(TypeChecker::CommanderInt value) {
    return std::tanh((TypeChecker::CommanderFloat)value);
}

TypeChecker::CommanderFloat Function::tanh(TypeChecker::CommanderFloat value) { return std::tanh(value); }

TypeChecker::CommanderFloat Function::csch(TypeChecker::CommanderInt value) {
    return 1 / sinh((TypeChecker::CommanderFloat)value);
}

TypeChecker::CommanderFloat Function::csch(TypeChecker::CommanderFloat value) { return 1 / sinh(value); }

TypeChecker::CommanderFloat Function::sech(TypeChecker::CommanderInt value) {
    return 1 / cosh((TypeChecker::CommanderFloat)value);
}

TypeChecker::CommanderFloat Function::sech(TypeChecker::CommanderFloat value) { return 1 / cosh(value); }

TypeChecker::CommanderFloat Function::coth(TypeChecker::CommanderInt value) {
    return std::cosh((TypeChecker::CommanderFloat)value) / std::sinh((TypeChecker::CommanderFloat)value);
}

TypeChecker::CommanderFloat Function::coth(TypeChecker::CommanderFloat value) {
    return std::cosh(value) / std::sinh(value);
}

TypeChecker::CommanderFloat Function::arcsin(TypeChecker::CommanderInt value) {
    return std::asin((TypeChecker::CommanderFloat)value);
}

TypeChecker::CommanderFloat Function::arcsin(TypeChecker::CommanderFloat value) { return std::asin(value); }

TypeChecker::CommanderFloat Function::arccos(TypeChecker::CommanderInt value) {
    return std::acos((TypeChecker::CommanderFloat)value);
}

TypeChecker::CommanderFloat Function::arccos(TypeChecker::CommanderFloat value) { return std::acos(value); }

TypeChecker::CommanderFloat Function::arctan(TypeChecker::CommanderInt value) {
    return std::atan((TypeChecker::CommanderFloat)value);
}

TypeChecker::CommanderFloat Function::arctan(TypeChecker::CommanderFloat value) { return std::atan(value); }

TypeChecker::CommanderFloat Function::arccsc(TypeChecker::CommanderInt value) { return std::asin(1 / value); }

TypeChecker::CommanderFloat Function::arccsc(TypeChecker::CommanderFloat value) { return std::asin(1 / value); }

TypeChecker::CommanderFloat Function::arcsec(TypeChecker::CommanderInt value) { return std::acos(1 / value); }

TypeChecker::CommanderFloat Function::arcsec(TypeChecker::CommanderFloat value) { return std::acos(1 / value); }

TypeChecker::CommanderFloat Function::arccot(TypeChecker::CommanderInt value) { return std::atan(1 / value); }

TypeChecker::CommanderFloat Function::arccot(TypeChecker::CommanderFloat value) { return std::atan(1 / value); }

TypeChecker::CommanderFloat Function::arcsinh(TypeChecker::CommanderInt value) {
    return std::asinh((TypeChecker::CommanderFloat)value);
}

TypeChecker::CommanderFloat Function::arcsinh(TypeChecker::CommanderFloat value) { return std::asinh(value); }

TypeChecker::CommanderFloat Function::arccosh(TypeChecker::CommanderInt value) {
    return std::acosh((TypeChecker::CommanderFloat)value);
}

TypeChecker::CommanderFloat Function::arccosh(TypeChecker::CommanderFloat value) { return std::acosh(value); }

TypeChecker::CommanderFloat Function::arctanh(TypeChecker::CommanderInt value) {
    return std::atanh((TypeChecker::CommanderFloat)value);
}

TypeChecker::CommanderFloat Function::arctanh(TypeChecker::CommanderFloat value) { return std::atanh(value); }

TypeChecker::CommanderFloat Function::arccsch(TypeChecker::CommanderInt value) { return std::asinh(1 / value); }

TypeChecker::CommanderFloat Function::arccsch(TypeChecker::CommanderFloat value) { return std::asinh(1 / value); }

TypeChecker::CommanderFloat Function::arcsech(TypeChecker::CommanderInt value) { return std::acosh(1 / value); }

TypeChecker::CommanderFloat Function::arcsech(TypeChecker::CommanderFloat value) { return std::acosh(1 / value); }

TypeChecker::CommanderFloat Function::arccoth(TypeChecker::CommanderInt value) { return std::atanh(1 / value); }

TypeChecker::CommanderFloat Function::arccoth(TypeChecker::CommanderFloat value) { return std::atanh(1 / value); }

TypeChecker::CommanderFloat Function::random() {
    return (TypeChecker::CommanderFloat)rand();
    // TODO: may generate an identical value every run
}

TypeChecker::CommanderInt Function::time() {
    std::chrono::system_clock::time_point timeMethodCalled = std::chrono::system_clock::now();
    std::chrono::system_clock::duration epochTime = timeMethodCalled.time_since_epoch();
    return epochTime.count() * std::chrono::system_clock::period::num
         / std::chrono::system_clock::period::den;  // returns seconds since epoch time
}

TypeChecker::CommanderTuple Function::date() {
    TypeChecker::CommanderTuple dataToReturn;

    std::chrono::system_clock::time_point timeMethodCalled = std::chrono::system_clock::now();
    std::time_t end_time = std::chrono::system_clock::to_time_t(timeMethodCalled);
    std::string timeAndDate = std::ctime(&end_time);

    // string r = s1.substr(3, 2);
    dataToReturn.push_back(std::make_any<TypeChecker::CommanderString>(
            timeAndDate.substr(0, 3)));  // should be the current day (e.g. Mon)
    dataToReturn.push_back(
            std::make_any<TypeChecker::CommanderString>(timeAndDate.substr(4, 3)));  // should be the month (e.g. Nov)
    dataToReturn.push_back(
            std::make_any<TypeChecker::CommanderInt>(stoi(timeAndDate.substr(8, 2))));  // should be the date (e.g. 30)
    dataToReturn.push_back(std::make_any<TypeChecker::CommanderInt>(
            stoi(timeAndDate.substr(20, 4))));  // should be the year (e.g. 2024)
    return dataToReturn;
}

void Function::sleep(TypeChecker::CommanderInt timeToSleep) {
    std::this_thread::sleep_for(std::chrono::milliseconds(timeToSleep));
}

// STRING METHODS
TypeChecker::CommanderString Function::charAt(TypeChecker::CommanderInt index,
                                              TypeChecker::CommanderString sourceString) {
    return std::to_string(sourceString[index]);
}

TypeChecker::CommanderBool Function::startsWith(TypeChecker::CommanderString expected,
                                                TypeChecker::CommanderString sourceString) {
    return sourceString.rfind(expected, 0);  // rfind(str, 0) checks the beginning of the string
}

TypeChecker::CommanderBool Function::endsWith(TypeChecker::CommanderString expected,
                                              TypeChecker::CommanderString sourceString) {
    if (expected.size() == sourceString.size()) { return sourceString == expected; }
    if (sourceString.find_last_of(expected) == std::string::npos) { return false; }
    return true;
}

TypeChecker::CommanderBool Function::includes(TypeChecker::CommanderString expected,
                                              TypeChecker::CommanderString sourceString) {
    return sourceString.find(expected);
}

TypeChecker::CommanderInt Function::indexOf(TypeChecker::CommanderString expected,
                                            TypeChecker::CommanderString sourceString) {
    size_t startingIndex = sourceString.find_first_of(expected);
    if (startingIndex == std::string::npos) { return -1; }
    return startingIndex;
}

TypeChecker::CommanderInt Function::length(TypeChecker::CommanderString sourceString) { return sourceString.size(); }

TypeChecker::CommanderString Function::replace(TypeChecker::CommanderString oldPhrase,
                                               TypeChecker::CommanderString newPhrase,
                                               TypeChecker::CommanderString sourceString) {  // TODO: refactor?
    size_t replaceStartsAt = sourceString.find(oldPhrase);
    if (replaceStartsAt == std::string::npos) { return sourceString; }
    TypeChecker::CommanderString firstHalf = sourceString.substr(
            0, replaceStartsAt);  // Technically may not be half of the string
    TypeChecker::CommanderString secondHalf = sourceString.substr(replaceStartsAt + oldPhrase.size() - 1,
                                                                  sourceString.size() - 1);

    return firstHalf + newPhrase + secondHalf;
}

TypeChecker::CommanderString Function::replaceAll(TypeChecker::CommanderString oldPhrase,
                                                  TypeChecker::CommanderString newPhrase,
                                                  TypeChecker::CommanderString sourceString) {
    TypeChecker::CommanderString newString = std::string(sourceString);
    newString.replace(oldPhrase.begin(), oldPhrase.end(), newPhrase);
    return newString;
}

TypeChecker::CommanderString Function::substring(TypeChecker::CommanderInt startingIndex,
                                                 TypeChecker::CommanderString sourceString) {
    return sourceString.substr(startingIndex);
}

TypeChecker::CommanderString Function::substring(TypeChecker::CommanderInt startingIndex,
                                                 TypeChecker::CommanderInt endingIndex,
                                                 TypeChecker::CommanderString sourceString) {
    return sourceString.substr(startingIndex, endingIndex);
}

TypeChecker::CommanderString Function::trim(TypeChecker::CommanderString sourceString) {
    std::string whitespaceChars = "\n\r\t\f\v";
    size_t startIndex = sourceString.find_first_of(whitespaceChars);
    size_t endIndex = sourceString.find_last_of(whitespaceChars);
    return sourceString.substr(startIndex + 1, endIndex);
}

TypeChecker::CommanderString Function::lower(TypeChecker::CommanderString sourceString) {
    TypeChecker::CommanderString newString;
    for (char currentChar : sourceString) { newString += std::tolower(currentChar); }
    return newString;
}

TypeChecker::CommanderString Function::upper(TypeChecker::CommanderString sourceString) {
    TypeChecker::CommanderString newString;
    for (char currentChar : sourceString) { newString += std::toupper(currentChar); }
    return newString;
}

TypeChecker::CommanderArray<TypeChecker::CommanderString> Function::split(TypeChecker::CommanderString splitToken,
                                                                          TypeChecker::CommanderString sourceString) {
    TypeChecker::CommanderArray<TypeChecker::CommanderString> strings
            = TypeChecker::CommanderArray<TypeChecker::CommanderString>();
    TypeChecker::CommanderString workingString = std::string(sourceString);
    size_t currentTokenLocation = 1;  // temp value

    while (currentTokenLocation != 0) {
        currentTokenLocation = workingString.find(splitToken);
        // TODO: incomplete
        // find an occurrence of the token to remove, make a substring, and remove from workingString
    }
    return strings;
}