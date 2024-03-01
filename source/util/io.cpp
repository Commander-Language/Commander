/**
 * @file io.cpp
 * @brief Contains implementation for io.hpp
 *
 */

#include "source/util/io.hpp"

namespace Util {

    std::string readFile(const std::string& filePath) {
        std::ifstream input(filePath);
        if (!input.is_open()) { throw Util::CommanderException("File not found at " + filePath); }
        return {std::istreambuf_iterator<char>(input), std::istreambuf_iterator<char>()};
    }

    void writeToFile(const std::string& fileData, const std::string& filePath) {
        std::ofstream out(filePath);
        if (!out.is_open()) { throw Util::CommanderException("Unable to write to file at " + filePath); }
        out << fileData;
        out.close();
    }

}  // namespace Util
