/**
 * @file job_runner_tests.cpp
 * @brief Tests for the job runner
 */
#include "source/job_runner/job_runner_linux.hpp"
#include "source/util/commander_exception.hpp"
#include <fstream>
#include <gtest/gtest.h>
#include <iostream>

std::string getFileContents(const std::string& filePath) {
    std::ifstream file(filePath);
    if (!file.is_open()) throw Util::CommanderException("Job Runner Tests: Can't find file");
    std::stringstream buf;
    buf << file.rdbuf();
    return buf.str();
}

using Args = std::vector<std::string>;
const std::string testLocation = "../tests/files/job_runner_tests/";

/**
 * @brief Run a simple external command
 */
TEST(JobRunnerTests, RunSimpleJobCat) {
    Args arg1 {"cat", testLocation + "testDirectory/cat.txt"};
    Process::Process proc1(arg1, Process::ProcessType::EXTERNAL, false, true);
    JobRunner::JobRunnerLinux runner1(std::make_shared<Process::Process>(proc1));
    JobRunner::JobInfo output1 = runner1.execProcess();

    std::string expected = getFileContents(testLocation + "testDirectory/cat.txt");
    EXPECT_EQ(std::get<0>(output1), expected);
}
/**
 * Run a simple external command with big output to terminal
 */
TEST(JobRunnerTests, RunSimpleJobCat2) {
    Args arg1 {"cat", testLocation + "testDirectory/cat2.txt"};
    Process::Process proc1(arg1, Process::ProcessType::EXTERNAL, false, true);
    JobRunner::JobRunnerLinux runner1(std::make_shared<Process::Process>(proc1));
    JobRunner::JobInfo output1 = runner1.execProcess();

    std::string expected = getFileContents(testLocation + "testDirectory/cat2.txt");
    EXPECT_EQ(std::get<0>(output1), expected);
}
/**
 * @brief Run a simple external command with more than one argument
 */
TEST(JobRunnerTests, RunSimpleJobLS) {
    // arguments to ls to hopefully make it be the
    // same output across different systems
    //  (gets rid of user info and time)
    Args arg1 {"ls", "-Ggh", "--time-style=+", testLocation + "testDirectory"};
    Process::Process proc1(arg1, Process::ProcessType::EXTERNAL, false, true);
    JobRunner::JobRunnerLinux runner1(std::make_shared<Process::Process>(proc1));
    JobRunner::JobInfo output1 = runner1.execProcess();

    std::string expected = getFileContents(testLocation + "lsOutput.txt");
    EXPECT_EQ(std::get<0>(output1), expected);
}
/**
 * @brief Run a external command in the background
 * @details This test should pass immediately
 *          if you pull up htop/top you should see the job running
 */
TEST(JobRunnerTests, RunBackgroundJob) {
    Args arg1 {"sleep", "20s"};
    Process::Process proc1(arg1, Process::ProcessType::EXTERNAL, true);
    JobRunner::JobRunnerLinux runner1(std::make_shared<Process::Process>(proc1));
    runner1.execProcess();
    SUCCEED();
}
/**
 * @brief Run a command job where we want to save return info
 */
TEST(JobRunnerTests, RunSaveReturnJob) {
    Args arg1 {"ls", "-l", "-a"};
    Process::Process proc1(arg1, Process::ProcessType::EXTERNAL, false, true);
    JobRunner::JobRunnerLinux runner1(std::make_shared<Process::Process>(proc1));
    JobRunner::JobInfo info = runner1.execProcess();
    std::cout << "Standard Output is:\n" << std::get<0>(info) << "\n";
    std::cout << "Standard Error is:\n" << std::get<1>(info) << "\n";
    std::cout << "Return Code is:\n" << std::get<2>(info) << "\n";
}
/**
 * @brief Run a command job where we want to save return info and big output
 */
TEST(JobRunnerTests, RunSaveReturnJob2) {
    Args arg1 {"cat", testLocation + "testDirectory/cat.txt"};
    Process::Process proc1(arg1, Process::ProcessType::EXTERNAL, false, true);
    JobRunner::JobRunnerLinux runner1(std::make_shared<Process::Process>(proc1));
    JobRunner::JobInfo info = runner1.execProcess();
    std::cout << "Standard Output is:\n" << std::get<0>(info) << "\n";
    std::cout << "Standard Error is:\n" << std::get<1>(info) << "\n";
    std::cout << "Return Code is:\n" << std::get<2>(info) << "\n";
}
/**
 * @brief Run a command job where we want to save return info, but
 *        the command returns an error code
 */
TEST(JobRunnerTests, RunReturnJob3) {
    Args arg1 {"cat", "-badarg"};
    Process::Process proc1(arg1, Process::ProcessType::EXTERNAL, false, true);
    JobRunner::JobRunnerLinux runner1(std::make_shared<Process::Process>(proc1));
    JobRunner::JobInfo info = runner1.execProcess();
    std::cout << "Standard Output is:\n" << std::get<0>(info) << "\n";
    std::cout << "Standard Error is:\n" << std::get<1>(info) << "\n";
    std::cout << "Return Code is:\n" << std::get<2>(info) << "\n";
}
/**
 * @brief Run a command job where we want to save return info and output bigger than
 *        buffer size
 */
TEST(JobRunnerTests, RunSaveReturnJob4) {
    Args arg1 {"cat", testLocation + "testDirectory/cat2.txt"};
    Process::Process proc1(arg1, Process::ProcessType::EXTERNAL, false, true);
    JobRunner::JobRunnerLinux runner1(std::make_shared<Process::Process>(proc1));
    JobRunner::JobInfo info = runner1.execProcess();
    std::cout << "Standard Output is:\n" << std::get<0>(info) << "\n";
    std::cout << "Standard Error is:\n" << std::get<1>(info) << "\n";
    std::cout << "Return Code is:\n" << std::get<2>(info) << "\n";
}

int main(int argc, char** argv) {
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
