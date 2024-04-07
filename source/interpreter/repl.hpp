/**
 * @file repl.hpp
 * @brief Defines the `REPL` class for the Commander REPL environment.
 */

#ifndef REPL_HPP
#define REPL_HPP

#include <functional>
#include <string>
#include <vector>

/**
 * @brief A class representing the Commander REPL environment.
 */
class REPL {
public:
    /**
     * @brief Class constructor.
     *
     * @param interpretFunc A callback for interpreting (string) program text.
     */
    explicit REPL(const std::function<void(const std::string&)>& interpretFunc);

    /**
     * @brief Runs the REPL.
     */
    void run();

private:
    /**
     * @brief A list of previously-run commands.
     */
    std::vector<std::string> _history;

    /**
     * @brief The callback interpreting (string) program text.
     */
    std::function<void(const std::string&)> _interpretFunc;
};

#endif  // REPL_HPP
