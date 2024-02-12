/**
 * @file job_runner_tests.cpp
 * @brief Tests for the job runner
 */
#include "source/job_runner/job_runner.hpp"
#include "source/util/commander_exception.hpp"
#include <gtest/gtest.h>
#include <fstream>
#include <iostream>

std::string getFileContents(const std::string &filePath) {
    std::ifstream file(filePath);
    if (!file.is_open()) throw Util::CommanderException("Job Runner Tests: Can't find file");
    std::stringstream buf;
    buf << file.rdbuf();
    return buf.str();
}

using Args = std::vector<std::string>;
const std::string testLocation = "../tests/files/job_runner_tests/";

/**
 * @brief Run the builtin println and print
 */
TEST(JobRunnerTests, RunBuiltinPrint) {
    Args arg1{"println", "Hello from println builtin"};
    JobRunner::Process proc1(arg1, JobRunner::ProcessType::BUILTIN, false, true);

    JobRunner::JobRunner runner1(&proc1);
    JobRunner::JobInfo output1 = runner1.execProcess();

    Args arg2{"print", "Hello from print builtin"};
    JobRunner::Process proc2(arg2, JobRunner::ProcessType::BUILTIN, false, true);

    JobRunner::JobRunner runner2(&proc2);
    JobRunner::JobInfo output2 = runner2.execProcess();

    EXPECT_EQ(std::get<0>(output1), "Hello from println builtin\n");
    EXPECT_EQ(std::get<0>(output2), "Hello from print builtin");
}
/**
 * @brief Run a simple external command
 */
TEST(JobRunnerTests, RunSimpleJobCat) {
    Args arg1{"cat", testLocation + "testDirectory/cat.txt"};
    JobRunner::Process proc1(arg1, JobRunner::ProcessType::EXTERNAL, false, true);
    JobRunner::JobRunner runner1(&proc1);
    JobRunner::JobInfo output1 = runner1.execProcess();

    std::string expected = getFileContents(testLocation + "testDirectory/cat.txt");
    EXPECT_EQ(std::get<0>(output1), expected);
}
/**
 * Run a simple external command with big output to terminal
 */
TEST(JobRunnerTests, RunSimpleJobCat2) {
    Args arg1{"cat", testLocation + "testDirectory/cat2.txt"};
    JobRunner::Process proc1(arg1, JobRunner::ProcessType::EXTERNAL, false, true);
    JobRunner::JobRunner runner1(&proc1);
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
    Args arg1{"ls", "-Ggh", "--time-style=+", testLocation + "testDirectory"};
    JobRunner::Process proc1(arg1, JobRunner::ProcessType::EXTERNAL, false, true);
    JobRunner::JobRunner runner1(&proc1);
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
    Args arg1{"sleep", "20s"};
    JobRunner::Process proc1(arg1, JobRunner::ProcessType::EXTERNAL, true);
    JobRunner::JobRunner runner1(&proc1);
    runner1.execProcess();
    SUCCEED();
}
/**
 * @brief Run a pipe of commands of size two
 */
TEST(JobRunnerTests, RunPipeJob1) {
    Args arg1{"ls", "-la"};
    JobRunner::Process proc1(arg1, JobRunner::ProcessType::EXTERNAL);

    Args arg2{"grep", "ninja"};
    JobRunner::Process proc2(arg2, JobRunner::ProcessType::EXTERNAL);

    std::vector<JobRunner::Process *> pipe;
    pipe.push_back(&proc1);
    pipe.push_back(&proc2);

    JobRunner::Process pipeArgs(pipe);
    JobRunner::JobRunner runner(&pipeArgs);
    runner.execProcess();
}
/**
 * @brief Run a pipe of commands of size three
 */
TEST(JobRunnerTests, RunPipeJob2) {
    Args arg1{"ls", "-la"};
    JobRunner::Process proc1(arg1, JobRunner::ProcessType::EXTERNAL);

    Args arg2{"grep", "ninja"};
    JobRunner::Process proc2(arg2, JobRunner::ProcessType::EXTERNAL);

    Args arg3{"wc"};
    JobRunner::Process proc3(arg3, JobRunner::ProcessType::EXTERNAL);

    std::vector<JobRunner::Process *> pipe;
    pipe.push_back(&proc1);
    pipe.push_back(&proc2);
    pipe.push_back(&proc3);

    JobRunner::Process pipeArgs(pipe);
    JobRunner::JobRunner runner(&pipeArgs);
    runner.execProcess();
}
/**
 * @brief Run a pipeline with a builtin command
 */
TEST(JobRunnerTests, RunBuiltinInPipe) {
    Args arg1{"print", "12345678"};
    JobRunner::Process proc1(arg1, JobRunner::ProcessType::BUILTIN);

    Args arg2{"wc"};
    JobRunner::Process proc2(arg2, JobRunner::ProcessType::EXTERNAL);
    std::vector<JobRunner::Process *> pipe;

    pipe.push_back(&proc1);
    pipe.push_back(&proc2);

    JobRunner::Process pipeArgs(pipe);
    JobRunner::JobRunner runner(&pipeArgs);
    runner.execProcess();
}
/**
 * @brief Run a command job where we want to save return info
 */
TEST(JobRunnerTests, RunSaveReturnJob) {
    Args arg1{"ls", "-l", "-a"};
    JobRunner::Process proc1(arg1, JobRunner::ProcessType::EXTERNAL, false, true);
    JobRunner::JobRunner runner1(&proc1);
    JobRunner::JobInfo info = runner1.execProcess();
    std::cout << "Standard Output is:\n" << std::get<0>(info) << "\n";
    std::cout << "Standard Error is:\n" << std::get<1>(info) << "\n";
    std::cout << "Return Code is:\n" << std::get<2>(info) << "\n";
}
/**
 * @brief Run a command job where we want to save return info and big output
 */
TEST(JobRunnerTests, RunSaveReturnJob2) {
    Args arg1{"cat", testLocation + "testDirectory/cat.txt"};
    JobRunner::Process proc1(arg1, JobRunner::ProcessType::EXTERNAL, false, true);
    JobRunner::JobRunner runner1(&proc1);
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
    Args arg1{"cat", "-badarg"};
    JobRunner::Process proc1(arg1, JobRunner::ProcessType::EXTERNAL, false, true);
    JobRunner::JobRunner runner1(&proc1);
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
    Args arg1{"cat", testLocation + "testDirectory/cat2.txt"};
    JobRunner::Process proc1(arg1, JobRunner::ProcessType::EXTERNAL, false, true);
    JobRunner::JobRunner runner1(&proc1);
    JobRunner::JobInfo info = runner1.execProcess();
    std::cout << "Standard Output is:\n" << std::get<0>(info) << "\n";
    std::cout << "Standard Error is:\n" << std::get<1>(info) << "\n";
    std::cout << "Return Code is:\n" << std::get<2>(info) << "\n";
}
/**
 *  @brief Test saving output with a pipe of size two
 */
TEST(JobRunnerTests, RunSaveReturnJob5) {
    Args arg1{"cat", testLocation + "testDirectory/cat2.txt"};
    JobRunner::Process proc1(arg1, JobRunner::ProcessType::EXTERNAL, false, false);

    Args arg2{"wc"};
    JobRunner::Process proc2(arg2, JobRunner::ProcessType::EXTERNAL, false, true);

    std::vector<JobRunner::Process *> pipe;
    pipe.push_back(&proc1);
    pipe.push_back(&proc2);

    JobRunner::Process pipeArgs(pipe);
    JobRunner::JobRunner runner1(&pipeArgs);

    JobRunner::JobInfo info = runner1.execProcess();
    std::cout << "Standard Output is:\n" << std::get<0>(info) << "\n";
    std::cout << "Standard Error is:\n" << std::get<1>(info) << "\n";
    std::cout << "Return Code is:\n" << std::get<2>(info) << "\n";
}

/**
 * @brief test saving output with a pipe of size three
 */
TEST(JobRunnerTests, RunSaveReturnJob6) {
    Args arg1{"cat", testLocation + "testDirectory/cat2.txt"};
    JobRunner::Process proc1(arg1, JobRunner::ProcessType::EXTERNAL, false, false);

    Args arg2{"grep", "a"};
    JobRunner::Process proc2(arg2, JobRunner::ProcessType::EXTERNAL, false, false);

    Args arg3{"wc"};
    JobRunner::Process proc3(arg3, JobRunner::ProcessType::EXTERNAL, false, true);

    std::vector<JobRunner::Process *> pipe;
    pipe.push_back(&proc1);
    pipe.push_back(&proc2);
    pipe.push_back(&proc3);

    JobRunner::Process pipeArgs(pipe);
    JobRunner::JobRunner runner1(&pipeArgs);

    JobRunner::JobInfo info = runner1.execProcess();
    std::cout << "Standard Output is:\n" << std::get<0>(info) << "\n";
    std::cout << "Standard Error is:\n" << std::get<1>(info) << "\n";
    std::cout << "Return Code is:\n" << std::get<2>(info) << "\n";
}
/**
 * @brief Test builtin in middle
 * @details The println builtin doesn't take
 *          the input from cat, but still outputs to wc.
 *          (This behavior is similar to zsh)
 */
TEST(JobRunnerTests, RunSaveReturnJob7) {
    Args arg1{"cat", testLocation + "testDirectory/cat2.txt"};
    JobRunner::Process proc1(arg1, JobRunner::ProcessType::EXTERNAL, false, false);

    Args arg2{"println", "abc"};
    JobRunner::Process proc2(arg2, JobRunner::ProcessType::BUILTIN, false, false);

    Args arg3{"wc"};
    JobRunner::Process proc3(arg3, JobRunner::ProcessType::EXTERNAL, false, true);

    std::vector<JobRunner::Process *> pipe;
    pipe.push_back(&proc1);
    pipe.push_back(&proc2);
    pipe.push_back(&proc3);

    JobRunner::Process pipeArgs(pipe);
    JobRunner::JobRunner runner1(&pipeArgs);

    JobRunner::JobInfo info = runner1.execProcess();
    std::cout << "Standard Output is:\n" << std::get<0>(info) << "\n";
    std::cout << "Standard Error is:\n" << std::get<1>(info) << "\n";
    std::cout << "Return Code is:\n" << std::get<2>(info) << "\n";
}

int main(int argc, char **argv) {
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}