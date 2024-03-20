/**
 * @file functions.cpp
 * @brief Implementations for functions.hpp
 */

#include "functions.hpp"

namespace Function {

    FlowController::CommanderTuplePtr VOID = std::make_shared<FlowController::CommanderTuple>(
            std::vector<FlowController::CommanderTypePtr> {});

    TypeChecker::FunctionTyPtr getFunctionTy(TypeChecker::TyPtr arg) {
        const std::vector<TypeChecker::TyPtr> params;
        return std::make_shared<TypeChecker::FunctionTy>(params, arg);
    }

    FlowController::CommanderIntPtr parseInt(FlowController::CommanderTypePtr intValue) {
        switch (intValue->getType()) {
            case TypeChecker::INT:
                return std::static_pointer_cast<FlowController::CommanderInt>(intValue);
            case TypeChecker::FLOAT:
                return std::make_shared<FlowController::CommanderInt>(
                        std::static_pointer_cast<FlowController::CommanderFloat>(intValue)->value);
            case TypeChecker::BOOL:
                return std::make_shared<FlowController::CommanderInt>(
                        std::static_pointer_cast<FlowController::CommanderBool>(intValue)->value ? 1 : 0);
            case TypeChecker::STRING:
                return std::make_shared<FlowController::CommanderInt>(
                        std::stoi(std::static_pointer_cast<FlowController::CommanderString>(intValue)->value));
            default:
                // TODO: Improve error to have location in file
                throw Util::CommanderException("Unable to parse int from type "
                                               + TypeChecker::typeToString(intValue->getType()));
        }
    }

    FlowController::CommanderFloatPtr parseFloat(FlowController::CommanderTypePtr floatValue) {
        switch (floatValue->getType()) {
            case TypeChecker::INT:
                return std::make_shared<FlowController::CommanderFloat>(
                        std::static_pointer_cast<FlowController::CommanderInt>(floatValue)->value);
            case TypeChecker::FLOAT:
                return std::static_pointer_cast<FlowController::CommanderFloat>(floatValue);
            case TypeChecker::BOOL:
                return std::make_shared<FlowController::CommanderFloat>(
                        std::static_pointer_cast<FlowController::CommanderBool>(floatValue)->value ? 1.0 : 0.0);
            case TypeChecker::STRING:
                return std::make_shared<FlowController::CommanderFloat>(
                        std::stod(std::static_pointer_cast<FlowController::CommanderString>(floatValue)->value));
            default:
                // TODO: Improve error to have location in file
                throw Util::CommanderException("Unable to parse float from type "
                                               + TypeChecker::typeToString(floatValue->getType()));
        }
    }

    FlowController::CommanderBoolPtr parseBool(FlowController::CommanderTypePtr boolValue) {
        switch (boolValue->getType()) {
            case TypeChecker::INT:
                return std::make_shared<FlowController::CommanderBool>(
                        std::static_pointer_cast<FlowController::CommanderInt>(boolValue)->value != 0);
            case TypeChecker::FLOAT:
                return std::make_shared<FlowController::CommanderBool>(
                        std::static_pointer_cast<FlowController::CommanderFloat>(boolValue)->value != 0);
            case TypeChecker::BOOL:
                return std::static_pointer_cast<FlowController::CommanderBool>(boolValue);
            case TypeChecker::STRING:
                return std::make_shared<FlowController::CommanderBool>(
                        std::static_pointer_cast<FlowController::CommanderString>(boolValue)->value == "true");
            default:
                // TODO: Improve error to have location in file
                throw Util::CommanderException("Unable to parse bool from type "
                                               + TypeChecker::typeToString(boolValue->getType()));
        }
    }

    FlowController::CommanderStringPtr toString(FlowController::CommanderTypePtr value) {
        return std::make_shared<FlowController::CommanderString>(value->getStringRepresentation());
    }

    FlowController::CommanderFloatPtr sqrt(FlowController::CommanderTypePtr numberValue) {
        switch (numberValue->getType()) {
            case TypeChecker::INT:
                return std::make_shared<FlowController::CommanderFloat>(
                        std::sqrt(std::static_pointer_cast<FlowController::CommanderInt>(numberValue)->value));
            case TypeChecker::FLOAT:
                return std::make_shared<FlowController::CommanderFloat>(
                        std::sqrt(std::static_pointer_cast<FlowController::CommanderFloat>(numberValue)->value));
            default:
                // TODO: Improve error to have location in file
                throw Util::CommanderException("Unable to take sqrt of type "
                                               + TypeChecker::typeToString(numberValue->getType()));
        }
    }

    FlowController::CommanderFloatPtr ln(FlowController::CommanderTypePtr numberValue) {
        switch (numberValue->getType()) {
            case TypeChecker::INT:
                return std::make_shared<FlowController::CommanderFloat>(
                        std::log(std::static_pointer_cast<FlowController::CommanderInt>(numberValue)->value));
            case TypeChecker::FLOAT:
                return std::make_shared<FlowController::CommanderFloat>(
                        std::log(std::static_pointer_cast<FlowController::CommanderFloat>(numberValue)->value));
            default:
                // TODO: Improve error to have location in file
                throw Util::CommanderException("Unable to take ln of type "
                                               + TypeChecker::typeToString(numberValue->getType()));
        }
    }

    FlowController::CommanderFloatPtr log(FlowController::CommanderTypePtr numberValue) {
        switch (numberValue->getType()) {
            case TypeChecker::INT:
                return std::make_shared<FlowController::CommanderFloat>(
                        std::log10(std::static_pointer_cast<FlowController::CommanderInt>(numberValue)->value));
            case TypeChecker::FLOAT:
                return std::make_shared<FlowController::CommanderFloat>(
                        std::log10(std::static_pointer_cast<FlowController::CommanderFloat>(numberValue)->value));
            default:
                // TODO: Improve error to have location in file
                throw Util::CommanderException("Unable to take log of type "
                                               + TypeChecker::typeToString(numberValue->getType()));
        }
    }

    FlowController::CommanderFloatPtr abs(FlowController::CommanderTypePtr numberValue) {
        switch (numberValue->getType()) {
            case TypeChecker::INT:
                return std::make_shared<FlowController::CommanderFloat>(
                        std::abs(std::static_pointer_cast<FlowController::CommanderInt>(numberValue)->value));
            case TypeChecker::FLOAT:
                return std::make_shared<FlowController::CommanderFloat>(
                        std::abs(std::static_pointer_cast<FlowController::CommanderFloat>(numberValue)->value));
            default:
                // TODO: Improve error to have location in file
                throw Util::CommanderException("Unable to take abs of type "
                                               + TypeChecker::typeToString(numberValue->getType()));
        }
    }

    FlowController::CommanderIntPtr floor(FlowController::CommanderTypePtr numberValue) {
        switch (numberValue->getType()) {
            case TypeChecker::INT:
                return std::static_pointer_cast<FlowController::CommanderInt>(numberValue);
            case TypeChecker::FLOAT:
                return std::make_shared<FlowController::CommanderInt>(
                        std::floor(std::static_pointer_cast<FlowController::CommanderFloat>(numberValue)->value));
            default:
                // TODO: Improve error to have location in file
                throw Util::CommanderException("Unable to take floor of type "
                                               + TypeChecker::typeToString(numberValue->getType()));
        }
    }

    FlowController::CommanderIntPtr ceil(FlowController::CommanderTypePtr numberValue) {
        switch (numberValue->getType()) {
            case TypeChecker::INT:
                return std::static_pointer_cast<FlowController::CommanderInt>(numberValue);
            case TypeChecker::FLOAT:
                return std::make_shared<FlowController::CommanderInt>(
                        std::ceil(std::static_pointer_cast<FlowController::CommanderFloat>(numberValue)->value));
            default:
                // TODO: Improve error to have location in file
                throw Util::CommanderException("Unable to take ceil of type "
                                               + TypeChecker::typeToString(numberValue->getType()));
        }
    }

    FlowController::CommanderIntPtr round(FlowController::CommanderTypePtr numberValue) {
        switch (numberValue->getType()) {
            case TypeChecker::INT:
                return std::static_pointer_cast<FlowController::CommanderInt>(numberValue);
            case TypeChecker::FLOAT:
                return std::make_shared<FlowController::CommanderInt>(
                        std::round(std::static_pointer_cast<FlowController::CommanderFloat>(numberValue)->value));
            default:
                // TODO: Improve error to have location in file
                throw Util::CommanderException("Unable to take round of type "
                                               + TypeChecker::typeToString(numberValue->getType()));
        }
    }

    FlowController::CommanderFloatPtr sin(FlowController::CommanderTypePtr numberValue) {
        switch (numberValue->getType()) {
            case TypeChecker::INT:
                return std::make_shared<FlowController::CommanderFloat>(
                        std::sin(std::static_pointer_cast<FlowController::CommanderInt>(numberValue)->value));
            case TypeChecker::FLOAT:
                return std::make_shared<FlowController::CommanderFloat>(
                        std::sin(std::static_pointer_cast<FlowController::CommanderFloat>(numberValue)->value));
            default:
                // TODO: Improve error to have location in file
                throw Util::CommanderException("Unable to take sin of type "
                                               + TypeChecker::typeToString(numberValue->getType()));
        }
    }

    FlowController::CommanderFloatPtr cos(FlowController::CommanderTypePtr numberValue) {
        switch (numberValue->getType()) {
            case TypeChecker::INT:
                return std::make_shared<FlowController::CommanderFloat>(
                        std::cos(std::static_pointer_cast<FlowController::CommanderInt>(numberValue)->value));
            case TypeChecker::FLOAT:
                return std::make_shared<FlowController::CommanderFloat>(
                        std::cos(std::static_pointer_cast<FlowController::CommanderFloat>(numberValue)->value));
            default:
                // TODO: Improve error to have location in file
                throw Util::CommanderException("Unable to take cos of type "
                                               + TypeChecker::typeToString(numberValue->getType()));
        }
    }

    FlowController::CommanderFloatPtr tan(FlowController::CommanderTypePtr numberValue) {
        switch (numberValue->getType()) {
            case TypeChecker::INT:
                return std::make_shared<FlowController::CommanderFloat>(
                        std::tan(std::static_pointer_cast<FlowController::CommanderInt>(numberValue)->value));
            case TypeChecker::FLOAT:
                return std::make_shared<FlowController::CommanderFloat>(
                        std::tan(std::static_pointer_cast<FlowController::CommanderFloat>(numberValue)->value));
            default:
                // TODO: Improve error to have location in file
                throw Util::CommanderException("Unable to take tan of type "
                                               + TypeChecker::typeToString(numberValue->getType()));
        }
    }

    FlowController::CommanderFloatPtr csc(FlowController::CommanderTypePtr numberValue) {
        if (numberValue->getType() != TypeChecker::INT && numberValue->getType() != TypeChecker::FLOAT) {
            // TODO: Improve error to have location in file
            throw Util::CommanderException("Unable to take csc of type "
                                           + TypeChecker::typeToString(numberValue->getType()));
        }
        FlowController::CommanderFloatPtr val = sin(numberValue);
        val->value = 1.0 / val->value;
        return val;
    }

    FlowController::CommanderFloatPtr sec(FlowController::CommanderTypePtr numberValue) {
        if (numberValue->getType() != TypeChecker::INT && numberValue->getType() != TypeChecker::FLOAT) {
            // TODO: Improve error to have location in file
            throw Util::CommanderException("Unable to take sec of type "
                                           + TypeChecker::typeToString(numberValue->getType()));
        }
        FlowController::CommanderFloatPtr val = cos(numberValue);
        val->value = 1.0 / val->value;
        return val;
    }

    FlowController::CommanderFloatPtr cot(FlowController::CommanderTypePtr numberValue) {
        if (numberValue->getType() != TypeChecker::INT && numberValue->getType() != TypeChecker::FLOAT) {
            // TODO: Improve error to have location in file
            throw Util::CommanderException("Unable to take cot of type "
                                           + TypeChecker::typeToString(numberValue->getType()));
        }
        FlowController::CommanderFloatPtr val = tan(numberValue);
        val->value = 1.0 / val->value;
        return val;
    }

    FlowController::CommanderFloatPtr sinh(FlowController::CommanderTypePtr numberValue) {
        switch (numberValue->getType()) {
            case TypeChecker::INT:
                return std::make_shared<FlowController::CommanderFloat>(
                        std::sinh(std::static_pointer_cast<FlowController::CommanderInt>(numberValue)->value));
            case TypeChecker::FLOAT:
                return std::make_shared<FlowController::CommanderFloat>(
                        std::sinh(std::static_pointer_cast<FlowController::CommanderFloat>(numberValue)->value));
            default:
                // TODO: Improve error to have location in file
                throw Util::CommanderException("Unable to take sinh of type "
                                               + TypeChecker::typeToString(numberValue->getType()));
        }
    }

    FlowController::CommanderFloatPtr cosh(FlowController::CommanderTypePtr numberValue) {
        switch (numberValue->getType()) {
            case TypeChecker::INT:
                return std::make_shared<FlowController::CommanderFloat>(
                        std::cosh(std::static_pointer_cast<FlowController::CommanderInt>(numberValue)->value));
            case TypeChecker::FLOAT:
                return std::make_shared<FlowController::CommanderFloat>(
                        std::cosh(std::static_pointer_cast<FlowController::CommanderFloat>(numberValue)->value));
            default:
                // TODO: Improve error to have location in file
                throw Util::CommanderException("Unable to take cosh of type "
                                               + TypeChecker::typeToString(numberValue->getType()));
        }
    }

    FlowController::CommanderFloatPtr tanh(FlowController::CommanderTypePtr numberValue) {
        switch (numberValue->getType()) {
            case TypeChecker::INT:
                return std::make_shared<FlowController::CommanderFloat>(
                        std::tanh(std::static_pointer_cast<FlowController::CommanderInt>(numberValue)->value));
            case TypeChecker::FLOAT:
                return std::make_shared<FlowController::CommanderFloat>(
                        std::tanh(std::static_pointer_cast<FlowController::CommanderFloat>(numberValue)->value));
            default:
                // TODO: Improve error to have location in file
                throw Util::CommanderException("Unable to take tanh of type "
                                               + TypeChecker::typeToString(numberValue->getType()));
        }
    }

    FlowController::CommanderFloatPtr csch(FlowController::CommanderTypePtr numberValue) {
        if (numberValue->getType() != TypeChecker::INT && numberValue->getType() != TypeChecker::FLOAT) {
            // TODO: Improve error to have location in file
            throw Util::CommanderException("Unable to take csch of type "
                                           + TypeChecker::typeToString(numberValue->getType()));
        }
        FlowController::CommanderFloatPtr val = sinh(numberValue);
        val->value = 1.0 / val->value;
        return val;
    }

    FlowController::CommanderFloatPtr sech(FlowController::CommanderTypePtr numberValue) {
        if (numberValue->getType() != TypeChecker::INT && numberValue->getType() != TypeChecker::FLOAT) {
            // TODO: Improve error to have location in file
            throw Util::CommanderException("Unable to take sech of type "
                                           + TypeChecker::typeToString(numberValue->getType()));
        }
        FlowController::CommanderFloatPtr val = cosh(numberValue);
        val->value = 1.0 / val->value;
        return val;
    }

    FlowController::CommanderFloatPtr coth(FlowController::CommanderTypePtr numberValue) {
        if (numberValue->getType() != TypeChecker::INT && numberValue->getType() != TypeChecker::FLOAT) {
            // TODO: Improve error to have location in file
            throw Util::CommanderException("Unable to take coth of type "
                                           + TypeChecker::typeToString(numberValue->getType()));
        }
        FlowController::CommanderFloatPtr val = tanh(numberValue);
        val->value = 1.0 / val->value;
        return val;
    }

    FlowController::CommanderFloatPtr arcsin(FlowController::CommanderTypePtr numberValue) {
        switch (numberValue->getType()) {
            case TypeChecker::INT:
                return std::make_shared<FlowController::CommanderFloat>(
                        std::asin(std::static_pointer_cast<FlowController::CommanderInt>(numberValue)->value));
            case TypeChecker::FLOAT:
                return std::make_shared<FlowController::CommanderFloat>(
                        std::asin(std::static_pointer_cast<FlowController::CommanderFloat>(numberValue)->value));
            default:
                // TODO: Improve error to have location in file
                throw Util::CommanderException("Unable to take arcsin of type "
                                               + TypeChecker::typeToString(numberValue->getType()));
        }
    }

    FlowController::CommanderFloatPtr arccos(FlowController::CommanderTypePtr numberValue) {
        switch (numberValue->getType()) {
            case TypeChecker::INT:
                return std::make_shared<FlowController::CommanderFloat>(
                        std::acos(std::static_pointer_cast<FlowController::CommanderInt>(numberValue)->value));
            case TypeChecker::FLOAT:
                return std::make_shared<FlowController::CommanderFloat>(
                        std::acos(std::static_pointer_cast<FlowController::CommanderFloat>(numberValue)->value));
            default:
                // TODO: Improve error to have location in file
                throw Util::CommanderException("Unable to take arccos of type "
                                               + TypeChecker::typeToString(numberValue->getType()));
        }
    }

    FlowController::CommanderFloatPtr arctan(FlowController::CommanderTypePtr numberValue) {
        switch (numberValue->getType()) {
            case TypeChecker::INT:
                return std::make_shared<FlowController::CommanderFloat>(
                        std::atan(std::static_pointer_cast<FlowController::CommanderInt>(numberValue)->value));
            case TypeChecker::FLOAT:
                return std::make_shared<FlowController::CommanderFloat>(
                        std::atan(std::static_pointer_cast<FlowController::CommanderFloat>(numberValue)->value));
            default:
                // TODO: Improve error to have location in file
                throw Util::CommanderException("Unable to take arctan of type "
                                               + TypeChecker::typeToString(numberValue->getType()));
        }
    }

    FlowController::CommanderFloatPtr arccsc(FlowController::CommanderTypePtr numberValue) {
        switch (numberValue->getType()) {
            case TypeChecker::INT:
                return std::make_shared<FlowController::CommanderFloat>(
                        std::asin(1.0 / std::static_pointer_cast<FlowController::CommanderInt>(numberValue)->value));
            case TypeChecker::FLOAT:
                return std::make_shared<FlowController::CommanderFloat>(
                        std::asin(1.0 / std::static_pointer_cast<FlowController::CommanderFloat>(numberValue)->value));
            default:
                // TODO: Improve error to have location in file
                throw Util::CommanderException("Unable to take arccsc of type "
                                               + TypeChecker::typeToString(numberValue->getType()));
        }
    }

    FlowController::CommanderFloatPtr arcsec(FlowController::CommanderTypePtr numberValue) {
        switch (numberValue->getType()) {
            case TypeChecker::INT:
                return std::make_shared<FlowController::CommanderFloat>(
                        std::acos(1.0 / std::static_pointer_cast<FlowController::CommanderInt>(numberValue)->value));
            case TypeChecker::FLOAT:
                return std::make_shared<FlowController::CommanderFloat>(
                        std::acos(1.0 / std::static_pointer_cast<FlowController::CommanderFloat>(numberValue)->value));
            default:
                // TODO: Improve error to have location in file
                throw Util::CommanderException("Unable to take arcsec of type "
                                               + TypeChecker::typeToString(numberValue->getType()));
        }
    }

    FlowController::CommanderFloatPtr arccot(FlowController::CommanderTypePtr numberValue) {
        switch (numberValue->getType()) {
            case TypeChecker::INT:
                return std::make_shared<FlowController::CommanderFloat>(
                        std::atan(1.0 / std::static_pointer_cast<FlowController::CommanderInt>(numberValue)->value));
            case TypeChecker::FLOAT:
                return std::make_shared<FlowController::CommanderFloat>(
                        std::atan(1.0 / std::static_pointer_cast<FlowController::CommanderFloat>(numberValue)->value));
            default:
                // TODO: Improve error to have location in file
                throw Util::CommanderException("Unable to take arccot of type "
                                               + TypeChecker::typeToString(numberValue->getType()));
        }
    }

    FlowController::CommanderFloatPtr arcsinh(FlowController::CommanderTypePtr numberValue) {
        switch (numberValue->getType()) {
            case TypeChecker::INT:
                return std::make_shared<FlowController::CommanderFloat>(
                        std::asinh(std::static_pointer_cast<FlowController::CommanderInt>(numberValue)->value));
            case TypeChecker::FLOAT:
                return std::make_shared<FlowController::CommanderFloat>(
                        std::asinh(std::static_pointer_cast<FlowController::CommanderFloat>(numberValue)->value));
            default:
                // TODO: Improve error to have location in file
                throw Util::CommanderException("Unable to take arcsinh of type "
                                               + TypeChecker::typeToString(numberValue->getType()));
        }
    }

    FlowController::CommanderFloatPtr arccosh(FlowController::CommanderTypePtr numberValue) {
        switch (numberValue->getType()) {
            case TypeChecker::INT:
                return std::make_shared<FlowController::CommanderFloat>(
                        std::acosh(std::static_pointer_cast<FlowController::CommanderInt>(numberValue)->value));
            case TypeChecker::FLOAT:
                return std::make_shared<FlowController::CommanderFloat>(
                        std::acosh(std::static_pointer_cast<FlowController::CommanderFloat>(numberValue)->value));
            default:
                // TODO: Improve error to have location in file
                throw Util::CommanderException("Unable to take arccosh of type "
                                               + TypeChecker::typeToString(numberValue->getType()));
        }
    }

    FlowController::CommanderFloatPtr arctanh(FlowController::CommanderTypePtr numberValue) {
        switch (numberValue->getType()) {
            case TypeChecker::INT:
                return std::make_shared<FlowController::CommanderFloat>(
                        std::atanh(std::static_pointer_cast<FlowController::CommanderInt>(numberValue)->value));
            case TypeChecker::FLOAT:
                return std::make_shared<FlowController::CommanderFloat>(
                        std::atanh(std::static_pointer_cast<FlowController::CommanderFloat>(numberValue)->value));
            default:
                // TODO: Improve error to have location in file
                throw Util::CommanderException("Unable to take arctanh of type "
                                               + TypeChecker::typeToString(numberValue->getType()));
        }
    }

    FlowController::CommanderFloatPtr arccsch(FlowController::CommanderTypePtr numberValue) {
        switch (numberValue->getType()) {
            case TypeChecker::INT:
                return std::make_shared<FlowController::CommanderFloat>(
                        std::asinh(1.0 / std::static_pointer_cast<FlowController::CommanderInt>(numberValue)->value));
            case TypeChecker::FLOAT:
                return std::make_shared<FlowController::CommanderFloat>(
                        std::asinh(1.0 / std::static_pointer_cast<FlowController::CommanderFloat>(numberValue)->value));
            default:
                // TODO: Improve error to have location in file
                throw Util::CommanderException("Unable to take arccsch of type "
                                               + TypeChecker::typeToString(numberValue->getType()));
        }
    }

    FlowController::CommanderFloatPtr arcsech(FlowController::CommanderTypePtr numberValue) {
        switch (numberValue->getType()) {
            case TypeChecker::INT:
                return std::make_shared<FlowController::CommanderFloat>(
                        std::acosh(1.0 / std::static_pointer_cast<FlowController::CommanderInt>(numberValue)->value));
            case TypeChecker::FLOAT:
                return std::make_shared<FlowController::CommanderFloat>(
                        std::acosh(1.0 / std::static_pointer_cast<FlowController::CommanderFloat>(numberValue)->value));
            default:
                // TODO: Improve error to have location in file
                throw Util::CommanderException("Unable to take arcsech of type "
                                               + TypeChecker::typeToString(numberValue->getType()));
        }
    }

    FlowController::CommanderFloatPtr arccoth(FlowController::CommanderTypePtr numberValue) {
        switch (numberValue->getType()) {
            case TypeChecker::INT:
                return std::make_shared<FlowController::CommanderFloat>(
                        std::atanh(1.0 / std::static_pointer_cast<FlowController::CommanderInt>(numberValue)->value));
            case TypeChecker::FLOAT:
                return std::make_shared<FlowController::CommanderFloat>(
                        std::atanh(1.0 / std::static_pointer_cast<FlowController::CommanderFloat>(numberValue)->value));
            default:
                // TODO: Improve error to have location in file
                throw Util::CommanderException("Unable to take arccoth of type "
                                               + TypeChecker::typeToString(numberValue->getType()));
        }
    }

    FlowController::CommanderFloatPtr randomFloat() {
        return std::make_shared<FlowController::CommanderFloat>(((double)rand()) / ((double)RAND_MAX));
        // TODO: may generate an identical value every run
    }

    FlowController::CommanderIntPtr time() {
        std::chrono::system_clock::time_point timeMethodCalled = std::chrono::system_clock::now();
        std::chrono::system_clock::duration epochTime = timeMethodCalled.time_since_epoch();
        return std::make_shared<FlowController::CommanderInt>(
                epochTime.count() * std::chrono::system_clock::period::num
                / std::chrono::system_clock::period::den);  // returns seconds since epoch time
    }

    FlowController::CommanderTuplePtr date() {
        std::vector<FlowController::CommanderTypePtr> values;

        std::chrono::system_clock::time_point timeMethodCalled = std::chrono::system_clock::now();
        std::time_t end_time = std::chrono::system_clock::to_time_t(timeMethodCalled);
        std::string timeAndDate = std::ctime(&end_time);

        // string r = s1.substr(3, 2);
        values.push_back(std::make_shared<FlowController::CommanderString>(
                timeAndDate.substr(0, 3)));  // should be the current day (e.g. Mon)
        values.push_back(std::make_shared<FlowController::CommanderString>(
                timeAndDate.substr(4, 3)));  // should be the month (e.g. Nov)
        values.push_back(std::make_shared<FlowController::CommanderInt>(
                stoi(timeAndDate.substr(8, 2))));  // should be the date (e.g. 30)
        values.push_back(std::make_shared<FlowController::CommanderInt>(
                stoi(timeAndDate.substr(20, 4))));  // should be the year (e.g. 2024)
        return std::make_shared<FlowController::CommanderTuple>(values);
    }

    FlowController::CommanderTuplePtr sleep(FlowController::CommanderIntPtr timeToSleep) {
        std::this_thread::sleep_for(std::chrono::milliseconds(timeToSleep->value));
        return VOID;
    }

    FlowController::CommanderStringPtr charAt(FlowController::CommanderStringPtr sourceString,
                                              FlowController::CommanderIntPtr index) {
        return std::make_shared<FlowController::CommanderString>(std::to_string(sourceString->value[index->value]));
    }

    FlowController::CommanderBoolPtr startsWith(FlowController::CommanderStringPtr sourceString,
                                                FlowController::CommanderStringPtr expected) {
        return std::make_shared<FlowController::CommanderBool>(
                sourceString->value.rfind(expected->value, 0));  // rfind(str, 0) checks the beginning of the string
    }

    FlowController::CommanderBoolPtr endsWith(FlowController::CommanderStringPtr sourceString,
                                              FlowController::CommanderStringPtr expected) {
        if (expected->value.size() == sourceString->value.size()) {
            return std::make_shared<FlowController::CommanderBool>(sourceString->value == expected->value);
        }
        return std::make_shared<FlowController::CommanderBool>(sourceString->value.find_last_of(expected->value)
                                                               != std::string::npos);
    }

    FlowController::CommanderIntPtr length(FlowController::CommanderTypePtr source) {
        switch (source->getType()) {
            case TypeChecker::ARRAY:
                return std::make_shared<FlowController::CommanderInt>(
                        std::static_pointer_cast<FlowController::CommanderArray>(source)->values.size());
            case TypeChecker::TUPLE:
                return std::make_shared<FlowController::CommanderInt>(
                        std::static_pointer_cast<FlowController::CommanderTuple>(source)->values.size());
            case TypeChecker::STRING:
                return std::make_shared<FlowController::CommanderInt>(
                        std::static_pointer_cast<FlowController::CommanderString>(source)->value.length());
            default:
                // TODO: Improve error to have location in file
                throw Util::CommanderException("Unable to get length from type "
                                               + TypeChecker::typeToString(source->getType()));
        }
    }

    FlowController::CommanderStringPtr replace(FlowController::CommanderStringPtr sourceString,
                                               FlowController::CommanderStringPtr oldPhrase,
                                               FlowController::CommanderStringPtr newPhrase) {
        size_t replaceStartsAt = sourceString->value.find(oldPhrase->value);
        if (replaceStartsAt == std::string::npos) { return sourceString; }
        std::string firstHalf = sourceString->value.substr(
                0, replaceStartsAt);  // Technically may not be half of the string
        std::string secondHalf = sourceString->value.substr(replaceStartsAt + oldPhrase->value.size() - 1,
                                                            sourceString->value.size() - 1);

        return std::make_shared<FlowController::CommanderString>(firstHalf + newPhrase->value + secondHalf);
    }

    FlowController::CommanderStringPtr replaceAll(FlowController::CommanderStringPtr sourceString,
                                                  FlowController::CommanderStringPtr oldPhrase,
                                                  FlowController::CommanderStringPtr newPhrase) {
        std::string newString = std::string(sourceString->value);
        newString.replace(oldPhrase->value.begin(), oldPhrase->value.end(), newPhrase->value);
        return std::make_shared<FlowController::CommanderString>(newString);
    }

    FlowController::CommanderStringPtr substring(FlowController::CommanderStringPtr sourceString,
                                                 FlowController::CommanderIntPtr startingIndex) {
        return std::make_shared<FlowController::CommanderString>(sourceString->value.substr(startingIndex->value));
    }

    FlowController::CommanderStringPtr substring(FlowController::CommanderStringPtr sourceString,
                                                 FlowController::CommanderIntPtr startingIndex,
                                                 FlowController::CommanderIntPtr endingIndex) {
        return std::make_shared<FlowController::CommanderString>(
                sourceString->value.substr(startingIndex->value, endingIndex->value));
    }

    FlowController::CommanderStringPtr trim(FlowController::CommanderStringPtr sourceString) {
        std::string whitespaceChars = "\n\r\t\f\v";
        size_t startIndex = sourceString->value.find_first_of(whitespaceChars);
        size_t endIndex = sourceString->value.find_last_of(whitespaceChars);
        return std::make_shared<FlowController::CommanderString>(sourceString->value.substr(startIndex + 1, endIndex));
    }

    FlowController::CommanderStringPtr lower(FlowController::CommanderStringPtr sourceString) {
        std::string newString;
        for (char currentChar : sourceString->value) { newString += std::tolower(currentChar); }
        return std::make_shared<FlowController::CommanderString>(newString);
    }

    FlowController::CommanderStringPtr upper(FlowController::CommanderStringPtr sourceString) {
        std::string newString;
        for (char currentChar : sourceString->value) { newString += std::toupper(currentChar); }
        return std::make_shared<FlowController::CommanderString>(newString);
    }

    FlowController::CommanderArrayPtr split(FlowController::CommanderStringPtr sourceString,
                                            FlowController::CommanderStringPtr splitToken) {
        std::vector<FlowController::CommanderTypePtr> values;
        std::string workingString = std::string(sourceString->value);
        size_t currentTokenLocation = 1;  // temp value

        while (currentTokenLocation != 0) {
            currentTokenLocation = workingString.find(splitToken->value);
            // TODO: incomplete
            // find an occurrence of the token to remove, make a substring, and remove from workingString
        }
        return std::make_shared<FlowController::CommanderArray>(values);
    }

    FlowController::CommanderIntPtr indexOf(FlowController::CommanderTypePtr obj,
                                            FlowController::CommanderTypePtr data) {
        std::vector<FlowController::CommanderTypePtr> values;
        switch (obj->getType()) {
            case TypeChecker::TUPLE:
                values = std::static_pointer_cast<FlowController::CommanderTuple>(obj)->values;
                break;
            case TypeChecker::ARRAY:
                values = std::static_pointer_cast<FlowController::CommanderArray>(obj)->values;
                break;
            case TypeChecker::STRING:
                return std::make_shared<FlowController::CommanderInt>(
                        std::static_pointer_cast<FlowController::CommanderString>(obj)->value.find(
                                std::static_pointer_cast<FlowController::CommanderString>(obj)->value));
            default:
                // TODO: Improve error to have location in file
                throw Util::CommanderException("Unable to get index from type "
                                               + TypeChecker::typeToString(obj->getType()));
        }
        for (int i = 0; i < values.size(); i++) {
            if (FlowController::equalOperation(values[i], data)->value) {
                return std::make_shared<FlowController::CommanderInt>(i);
            }
        }
        return std::make_shared<FlowController::CommanderInt>(-1);
    }

    FlowController::CommanderBoolPtr includes(FlowController::CommanderTypePtr obj,
                                              FlowController::CommanderTypePtr data) {
        if (obj->getType() != TypeChecker::TUPLE && obj->getType() != TypeChecker::ARRAY
            && obj->getType() != TypeChecker::STRING) {
            // TODO: Improve error to have location in file
            throw Util::CommanderException("Unable to get includes from type "
                                           + TypeChecker::typeToString(obj->getType()));
        }
        return std::make_shared<FlowController::CommanderBool>(indexOf(obj, data)->value != -1);
    }

    FlowController::CommanderArrayPtr sort(FlowController::CommanderArrayPtr array,
                                           FlowController::CommanderLambdaPtr function) {
        // TODO: Implement
        return nullptr;
    }

    FlowController::CommanderArrayPtr filter(FlowController::CommanderArrayPtr array,
                                             FlowController::CommanderLambdaPtr function) {
        // TODO: Implement
        return nullptr;
    }

    FlowController::CommanderArrayPtr map(FlowController::CommanderArrayPtr array,
                                          FlowController::CommanderLambdaPtr function) {
        // TODO: Implement
        return nullptr;
    }

    FlowController::CommanderTuplePtr foreach(FlowController::CommanderArrayPtr array,
                                              FlowController::CommanderLambdaPtr function) {
        // TODO: Implement
        return VOID;
    }

    FlowController::CommanderTuplePtr append(FlowController::CommanderArrayPtr array,
                                             FlowController::CommanderTypePtr data) {
        array->values.push_back(data);
        return VOID;
    }

    FlowController::CommanderTuplePtr remove(FlowController::CommanderArrayPtr array,
                                             FlowController::CommanderTypePtr data) {
        array->values.push_back(data);
        return VOID;
    }

}  // namespace Function