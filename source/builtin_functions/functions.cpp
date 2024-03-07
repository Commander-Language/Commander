/**
 * @file functions.cpp
 * @brief //TODO
 */

#include "functions.hpp"
#include <iostream>

TypeChecker::FunctionTyPtr Function::getFunctionTy(TypeChecker::TyPtr arg) {
    const std::vector<TypeChecker::TyPtr> params;
    return std::make_shared<TypeChecker::FunctionTy>(params, arg);
}

void Function::println(FlowController::CommanderString string) { std::cout << string << std::endl; }

void Function::print(FlowController::CommanderString string) { std::cout << string; }

FlowController::CommanderString Function::toString(FlowController::CommanderInt value) { return parseAsString(value); }

FlowController::CommanderInt Function::parseIntAsInt(FlowController::CommanderInt number) { return number; }

FlowController::CommanderInt Function::parseFloatAsInt(FlowController::CommanderFloat number) {
    return (FlowController::CommanderInt)number;
}

FlowController::CommanderInt Function::parseBoolAsInt(FlowController::CommanderBool value) {
    return (FlowController::CommanderBool)value;
}

FlowController::CommanderInt Function::parseStringAsInt(FlowController::CommanderString string) {
    return (FlowController::CommanderInt)stoi(string);
}

FlowController::CommanderFloat Function::parseIntAsFloat(FlowController::CommanderInt number) {
    return (FlowController::CommanderFloat)number;
}

FlowController::CommanderFloat Function::parseFloatAsFloat(FlowController::CommanderFloat number) { return number; }

FlowController::CommanderFloat Function::parseBoolAsFloat(FlowController::CommanderBool value) {
    return (FlowController::CommanderFloat)value;
}

FlowController::CommanderFloat Function::parseStringAsFloat(FlowController::CommanderString string) {
    return (FlowController::CommanderFloat)stod(string);
}

FlowController::CommanderBool Function::parseIntAsBool(FlowController::CommanderInt number) {
    return (FlowController::CommanderBool)number;
}

FlowController::CommanderBool Function::parseFloatAsBool(FlowController::CommanderFloat number) {
    return (FlowController::CommanderBool)number;
}

FlowController::CommanderBool Function::parseBoolAsBool(FlowController::CommanderBool value) { return value; }

FlowController::CommanderBool Function::parseStringAsBool(FlowController::CommanderString string) {
    if (string == "true") { return true; }
    if (string == "false") { return false; }
    throw std::runtime_error(std::string("The passed parameter was not \"true\" or \"false\"."));
}

FlowController::CommanderFloat Function::sqrt(FlowController::CommanderInt value) {
    return std::sqrt(value);  // TODO: int64_t may cause issues
}

FlowController::CommanderFloat Function::sqrt(FlowController::CommanderFloat value) { return std::sqrt(value); }

FlowController::CommanderFloat Function::ln(FlowController::CommanderInt value) {
    return std::log((FlowController::CommanderFloat)value);
}

FlowController::CommanderFloat Function::ln(FlowController::CommanderFloat value) { return std::log(value); }

FlowController::CommanderFloat Function::log(FlowController::CommanderInt value) {
    return std::log10((FlowController::CommanderFloat)value);
}

FlowController::CommanderFloat Function::log(FlowController::CommanderFloat value) { return std::log10(value); }

FlowController::CommanderInt Function::abs(FlowController::CommanderInt value) { return std::abs(value); }

FlowController::CommanderFloat Function::abs(FlowController::CommanderFloat value) { return std::abs(value); }

FlowController::CommanderInt Function::floor(FlowController::CommanderInt value) {
    return std::floor((FlowController::CommanderFloat)value);
}

FlowController::CommanderInt Function::floor(FlowController::CommanderFloat value) { return std::floor(value); }

FlowController::CommanderInt Function::ceil(FlowController::CommanderInt value) {
    return std::ceil((FlowController::CommanderFloat)value);
}

FlowController::CommanderInt Function::ceil(FlowController::CommanderFloat value) { return std::ceil(value); }

FlowController::CommanderInt Function::round(FlowController::CommanderInt value) {
    return std::round((FlowController::CommanderFloat)value);
}

FlowController::CommanderInt Function::round(FlowController::CommanderFloat value) { return std::round(value); }

FlowController::CommanderFloat Function::sin(FlowController::CommanderInt value) {
    return std::sin((FlowController::CommanderFloat)value);
}

FlowController::CommanderFloat Function::sin(FlowController::CommanderFloat value) { return std::sin(value); }

FlowController::CommanderFloat Function::cos(FlowController::CommanderInt value) {
    return std::cos((FlowController::CommanderFloat)value);
}

FlowController::CommanderFloat Function::cos(FlowController::CommanderFloat value) { return std::cos(value); }

FlowController::CommanderFloat Function::tan(FlowController::CommanderInt value) {
    return std::tan((FlowController::CommanderFloat)value);
}

FlowController::CommanderFloat Function::tan(FlowController::CommanderFloat value) { return std::tan(value); }

FlowController::CommanderFloat Function::csc(FlowController::CommanderInt value) {
    return 1 / std::sin((FlowController::CommanderFloat)value);
}

FlowController::CommanderFloat Function::csc(FlowController::CommanderFloat value) { return 1 / std::sin(value); }

FlowController::CommanderFloat Function::sec(FlowController::CommanderInt value) {
    return 1 / std::cos((FlowController::CommanderFloat)value);
}

FlowController::CommanderFloat Function::sec(FlowController::CommanderFloat value) { return 1 / std::cos(value); }

FlowController::CommanderFloat Function::cot(FlowController::CommanderInt value) {
    return std::cos((FlowController::CommanderFloat)value) / std::sin((FlowController::CommanderFloat)value);
}

FlowController::CommanderFloat Function::cot(FlowController::CommanderFloat value) {
    return std::cos(value) / std::sin(value);
}

FlowController::CommanderFloat Function::sinh(FlowController::CommanderInt value) {
    return std::sinh((FlowController::CommanderFloat)value);
}

FlowController::CommanderFloat Function::sinh(FlowController::CommanderFloat value) { return std::sinh(value); }

FlowController::CommanderFloat Function::cosh(FlowController::CommanderInt value) {
    return std::cosh((FlowController::CommanderFloat)value);
}

FlowController::CommanderFloat Function::cosh(FlowController::CommanderFloat value) { return std::cosh(value); }

FlowController::CommanderFloat Function::tanh(FlowController::CommanderInt value) {
    return std::tanh((FlowController::CommanderFloat)value);
}

FlowController::CommanderFloat Function::tanh(FlowController::CommanderFloat value) { return std::tanh(value); }

FlowController::CommanderFloat Function::csch(FlowController::CommanderInt value) {
    return 1 / sinh((FlowController::CommanderFloat)value);
}

FlowController::CommanderFloat Function::csch(FlowController::CommanderFloat value) { return 1 / sinh(value); }

FlowController::CommanderFloat Function::sech(FlowController::CommanderInt value) {
    return 1 / cosh((FlowController::CommanderFloat)value);
}

FlowController::CommanderFloat Function::sech(FlowController::CommanderFloat value) { return 1 / cosh(value); }

FlowController::CommanderFloat Function::coth(FlowController::CommanderInt value) {
    return std::cosh((FlowController::CommanderFloat)value) / std::sinh((FlowController::CommanderFloat)value);
}

FlowController::CommanderFloat Function::coth(FlowController::CommanderFloat value) {
    return std::cosh(value) / std::sinh(value);
}

FlowController::CommanderFloat Function::arcsin(FlowController::CommanderInt value) {
    return std::asin((FlowController::CommanderFloat)value);
}

FlowController::CommanderFloat Function::arcsin(FlowController::CommanderFloat value) { return std::asin(value); }

FlowController::CommanderFloat Function::arccos(FlowController::CommanderInt value) {
    return std::acos((FlowController::CommanderFloat)value);
}

FlowController::CommanderFloat Function::arccos(FlowController::CommanderFloat value) { return std::acos(value); }

FlowController::CommanderFloat Function::arctan(FlowController::CommanderInt value) {
    return std::atan((FlowController::CommanderFloat)value);
}

FlowController::CommanderFloat Function::arctan(FlowController::CommanderFloat value) { return std::atan(value); }

FlowController::CommanderFloat Function::arccsc(FlowController::CommanderInt value) { return std::asin(1 / value); }

FlowController::CommanderFloat Function::arccsc(FlowController::CommanderFloat value) { return std::asin(1 / value); }

FlowController::CommanderFloat Function::arcsec(FlowController::CommanderInt value) { return std::acos(1 / value); }

FlowController::CommanderFloat Function::arcsec(FlowController::CommanderFloat value) { return std::acos(1 / value); }

FlowController::CommanderFloat Function::arccot(FlowController::CommanderInt value) { return std::atan(1 / value); }

FlowController::CommanderFloat Function::arccot(FlowController::CommanderFloat value) { return std::atan(1 / value); }

FlowController::CommanderFloat Function::arcsinh(FlowController::CommanderInt value) {
    return std::asinh((FlowController::CommanderFloat)value);
}

FlowController::CommanderFloat Function::arcsinh(FlowController::CommanderFloat value) { return std::asinh(value); }

FlowController::CommanderFloat Function::arccosh(FlowController::CommanderInt value) {
    return std::acosh((FlowController::CommanderFloat)value);
}

FlowController::CommanderFloat Function::arccosh(FlowController::CommanderFloat value) { return std::acosh(value); }

FlowController::CommanderFloat Function::arctanh(FlowController::CommanderInt value) {
    return std::atanh((FlowController::CommanderFloat)value);
}

FlowController::CommanderFloat Function::arctanh(FlowController::CommanderFloat value) { return std::atanh(value); }

FlowController::CommanderFloat Function::arccsch(FlowController::CommanderInt value) { return std::asinh(1 / value); }

FlowController::CommanderFloat Function::arccsch(FlowController::CommanderFloat value) { return std::asinh(1 / value); }

FlowController::CommanderFloat Function::arcsech(FlowController::CommanderInt value) { return std::acosh(1 / value); }

FlowController::CommanderFloat Function::arcsech(FlowController::CommanderFloat value) { return std::acosh(1 / value); }

FlowController::CommanderFloat Function::arccoth(FlowController::CommanderInt value) { return std::atanh(1 / value); }

FlowController::CommanderFloat Function::arccoth(FlowController::CommanderFloat value) { return std::atanh(1 / value); }

FlowController::CommanderFloat Function::random() {
    return (FlowController::CommanderFloat)rand();
    // TODO: may generate an identical value every run
}

FlowController::CommanderInt Function::time() {
    std::chrono::system_clock::time_point timeMethodCalled = std::chrono::system_clock::now();
    std::chrono::system_clock::duration epochTime = timeMethodCalled.time_since_epoch();
    return epochTime.count() * std::chrono::system_clock::period::num
         / std::chrono::system_clock::period::den;  // returns seconds since epoch time
}

FlowController::CommanderTuple Function::date() {
    FlowController::CommanderTuple dataToReturn;

    std::chrono::system_clock::time_point timeMethodCalled = std::chrono::system_clock::now();
    std::time_t end_time = std::chrono::system_clock::to_time_t(timeMethodCalled);
    std::string timeAndDate = std::ctime(&end_time);

    // string r = s1.substr(3, 2);
    dataToReturn.push_back(std::make_any<FlowController::CommanderString>(
            timeAndDate.substr(0, 3)));  // should be the current day (e.g. Mon)
    dataToReturn.push_back(
            std::make_any<FlowController::CommanderString>(timeAndDate.substr(4, 3)));  // should be the month (e.g. Nov)
    dataToReturn.push_back(
            std::make_any<FlowController::CommanderInt>(stoi(timeAndDate.substr(8, 2))));  // should be the date (e.g. 30)
    dataToReturn.push_back(std::make_any<FlowController::CommanderInt>(
            stoi(timeAndDate.substr(20, 4))));  // should be the year (e.g. 2024)
    return dataToReturn;
}

void Function::sleep(FlowController::CommanderInt timeToSleep) {
    std::this_thread::sleep_for(std::chrono::milliseconds(timeToSleep));
}

// STRING METHODS
FlowController::CommanderString Function::charAt(FlowController::CommanderInt index,
                                              FlowController::CommanderString sourceString) {
    return std::to_string(sourceString[index]);
}

FlowController::CommanderBool Function::startsWith(FlowController::CommanderString expected,
                                                FlowController::CommanderString sourceString) {
    return sourceString.rfind(expected, 0);  // rfind(str, 0) checks the beginning of the string
}

FlowController::CommanderBool Function::endsWith(FlowController::CommanderString expected,
                                              FlowController::CommanderString sourceString) {
    if (expected.size() == sourceString.size()) { return sourceString == expected; }
    if (sourceString.find_last_of(expected) == std::string::npos) { return false; }
    return true;
}

FlowController::CommanderBool Function::includes(FlowController::CommanderString expected,
                                              FlowController::CommanderString sourceString) {
    return sourceString.find(expected);
}

FlowController::CommanderInt Function::indexOf(FlowController::CommanderString expected,
                                            FlowController::CommanderString sourceString) {
    size_t startingIndex = sourceString.find_first_of(expected);
    if (startingIndex == std::string::npos) { return -1; }
    return startingIndex;
}

FlowController::CommanderInt Function::length(FlowController::CommanderString sourceString) { return sourceString.size(); }

FlowController::CommanderString Function::replace(FlowController::CommanderString oldPhrase,
                                               FlowController::CommanderString newPhrase,
                                               FlowController::CommanderString sourceString) {  // TODO: refactor?
    size_t replaceStartsAt = sourceString.find(oldPhrase);
    if (replaceStartsAt == std::string::npos) { return sourceString; }
    FlowController::CommanderString firstHalf = sourceString.substr(
            0, replaceStartsAt);  // Technically may not be half of the string
    FlowController::CommanderString secondHalf = sourceString.substr(replaceStartsAt + oldPhrase.size() - 1,
                                                                  sourceString.size() - 1);

    return firstHalf + newPhrase + secondHalf;
}

FlowController::CommanderString Function::replaceAll(FlowController::CommanderString oldPhrase,
                                                  FlowController::CommanderString newPhrase,
                                                  FlowController::CommanderString sourceString) {
    FlowController::CommanderString newString = std::string(sourceString);
    newString.replace(oldPhrase.begin(), oldPhrase.end(), newPhrase);
    return newString;
}

FlowController::CommanderString Function::substring(FlowController::CommanderInt startingIndex,
                                                 FlowController::CommanderString sourceString) {
    return sourceString.substr(startingIndex);
}

FlowController::CommanderString Function::substring(FlowController::CommanderInt startingIndex,
                                                 FlowController::CommanderInt endingIndex,
                                                 FlowController::CommanderString sourceString) {
    return sourceString.substr(startingIndex, endingIndex);
}

FlowController::CommanderString Function::trim(FlowController::CommanderString sourceString) {
    std::string whitespaceChars = "\n\r\t\f\v";
    size_t startIndex = sourceString.find_first_of(whitespaceChars);
    size_t endIndex = sourceString.find_last_of(whitespaceChars);
    return sourceString.value.substr(startIndex + 1, endIndex);
}

FlowController::CommanderString Function::lower(FlowController::CommanderString sourceString) {
    FlowController::CommanderString newString;
    for (char currentChar : sourceString) { newString += std::tolower(currentChar); }
    return newString;
}

FlowController::CommanderString Function::upper(FlowController::CommanderString sourceString) {
    FlowController::CommanderString newString;
    for (char currentChar : sourceString) { newString += std::toupper(currentChar); }
    return newString;
}

FlowController::CommanderArray<FlowController::CommanderString> Function::split(FlowController::CommanderString splitToken,
                                                                          FlowController::CommanderString sourceString) {
    FlowController::CommanderArray<FlowController::CommanderString> strings
            = FlowController::CommanderArray<FlowController::CommanderString>();
    FlowController::CommanderString workingString = std::string(sourceString);
    size_t currentTokenLocation = 1;  // temp value

    while (currentTokenLocation != 0) {
        currentTokenLocation = workingString.find(splitToken);
        // TODO: incomplete
        // find an occurrence of the token to remove, make a substring, and remove from workingString
    }
    return strings;
}

FlowController::CommanderStringPtr Function::toString(FlowController::CommanderTypePtr type) {
    return std::make_shared<FlowController::CommanderString>(type->getStringRepresentation());
}

FlowController::CommanderBoolPtr Function::includes(FlowController::CommanderTuplePtr tuple, FlowController::CommanderTypePtr data) {
    for(const auto &currentValue : tuple->values) {
        if(FlowController::equalOperation(currentValue, data)) {
            return std::make_shared<FlowController::CommanderBool>(true);
        }
    }
    return std::make_shared<FlowController::CommanderBool>(false);
}

