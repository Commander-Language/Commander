/**
 * @file repl.hpp
 * @brief Defines the `REPL` class for the commander REPL environment.
 */

#ifndef REPL_HPP
#define REPL_HPP

#include <functional>
#include <string>
#include <vector>

class REPL {
public:
    explicit REPL(const std::function<void(const std::string&)>& interpretFunc);

    void run();

private:
    std::string _readLine();

    std::vector<std::string> _history;

    std::function<void(const std::string&)> _interpretFunc;
};

#endif  // REPL_HPP
