#ifndef JOBRUNNER_HPP
#define JOBRUNNER_HPP
/**
 * @file job_runner.hpp
 * @brief Job Runner header
 * @details A job could be one process or a pipe line of process.
 *          Examples of a job could be:
 *              1) ls -la
 *              2) ls -la | grep *.txt | wc -l
 *              3) ls -la | grep *.txt > output.txt | wc -l
 *
 */

#include <string>
#include <tuple>
#include <vector>

namespace jobRunner {

}  // namespace jobRunner
#endif