/**
 * @file commander_exception.cpp
 * @brief Contains implementation for functions in commander_exception.hpp
 *
 */
#include "commander_exception.hpp"

namespace util {
    const char* CommanderException::what() const noexcept { return _errorMessage.c_str(); }
}  // namespace util