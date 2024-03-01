/**
 * @file io.hpp
 * @brief Contains helpers for file i/o
 *
 */

#ifndef IO_HPP
#define IO_HPP

#include "source/util/commander_exception.hpp"
#include <fstream>
#include <string>

namespace Util {

    /**
     * @brief Helper that reads in a file given a path and returns it as a string.
     * @param filePath Path to the file
     * @return The string representing the file contents
     */
    std::string readFile(const std::string& filePath);

    /**
     * @brief Helper that writes to a file given data and a path
     * @param fileData Data of the file
     * @param filePath Path to the file
     */
    void writeToFile(const std::string& fileData, const std::string& filePath);

}  // namespace Util

#endif  // IO_HPP
