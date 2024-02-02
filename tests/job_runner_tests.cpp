/**
 * @file job_runner_tests.cpp
 * @brief Tests for the job runner
 */
#include "source/job_runner/job_runner.hpp"
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
/**
 * @brief Run a pipeline of more than one with a builtin command
 */
TEST(JobRunnerTests, RunBuiltinInPipe) {
    jobRunner::Command cmdPrintln("println", jobRunner::commandType::BUILT_IN);
    cmdPrintln.addArg("cat and dog");

    jobRunner::Command cmdGrep("grep", jobRunner::commandType::EXEC);
    cmdGrep.addArg("cat");

    jobRunner::Command cmdWC("wc", jobRunner::commandType::EXEC);
    cmdWC.addArg("-l");

    jobRunner::Job job;
    job.addCommandToPipeline(&cmdPrintln);
    job.addCommandToPipeline(&cmdGrep);
    job.addCommandToPipeline(&cmdWC);

    job.runJob();
    SUCCEED();
}
/**
 * @brief Run the builtin println
 */
TEST(JobRunnerTests, RunBuiltin3) {
    jobRunner::Command cmdPrintln("println", jobRunner::commandType::BUILT_IN);
    cmdPrintln.addArg("Hello, world!");

    jobRunner::Job job;
    job.addCommandToPipeline(&cmdPrintln);

    job.runJob();
    SUCCEED();
}
/**
 * @brief Run the builtin scan
 */
TEST(JobRunnerTests, RunBuiltin2) {
    std::streambuf* cinSave = std::cin.rdbuf();
    std::istringstream const input("Cats");
    std::cin.rdbuf(input.rdbuf());

    jobRunner::Command cmdScan("scan", jobRunner::commandType::BUILT_IN);
    cmdScan.addArg("Hello, what is your favorite animal? ");

    jobRunner::Job job;
    job.addCommandToPipeline(&cmdScan);
    job.runJob();

    std::cin.rdbuf(cinSave);
}

/**
 * @brief Run the builtin print
 */
TEST(JobRunnerTests, RunBuiltin) {
    jobRunner::Command cmdPrint("print", jobRunner::commandType::BUILT_IN);
    cmdPrint.addArg("Hello, world!");
    cmdPrint.addArg("Hello");

    jobRunner::Job job;
    job.addCommandToPipeline(&cmdPrint);

    job.runJob();
    SUCCEED();
}
/**
 * @brief Run a simple command
 */
TEST(JobRunnerTests, RunSimpleJobCat) {
    jobRunner::Command cmdCat("cat", jobRunner::commandType::EXEC);
    cmdCat.addArg("../tests/files/job_runner_tests/testDirectory/cat.txt");

    jobRunner::Job job;
    job.addCommandToPipeline(&cmdCat);

    job.runJob();
    SUCCEED();
}
/**
 * @brief Run a simple command with more than one argument
 */
TEST(JobRunnerTests, RunSimpleJobLS) {
    jobRunner::Command cmdLS("ls", jobRunner::commandType::EXEC);
    cmdLS.addArg("-Ggh");
    cmdLS.addArg("--time-style=+");
    cmdLS.addArg("../tests/files/job_runner_tests/testDirectory");

    jobRunner::Job job;
    job.addCommandToPipeline(&cmdLS);

    job.runJob();
    SUCCEED();
}
/**
 * Run a simple command with big output to terminal
 */
TEST(JobRunnerTests, RunSimpleJobCat2) {
    jobRunner::Command cmdCat("cat", jobRunner::commandType::EXEC);
    cmdCat.addArg("../tests/files/job_runner_tests/testDirectory/cat.txt");

    jobRunner::Job job;
    job.addCommandToPipeline(&cmdCat);

    job.runJob();

    SUCCEED();
}
/**
 * @brief Run a command in the background
 */
TEST(JobRunnerTests, RunBackgroundJob) {
    jobRunner::Command cmdSleep("sleep", jobRunner::commandType::EXEC);
    cmdSleep.addArg("20s");

    jobRunner::Job job;
    job.addCommandToPipeline(&cmdSleep);

    SUCCEED();
}
/**
 * @brief Run a pipe of commands of size two
 */
TEST(JobRunnerTests, RunPipeJob1) {
    jobRunner::Command cmdLS("ls", jobRunner::commandType::EXEC);
    cmdLS.addArg("-la");
    cmdLS.addArg("../tests/files/job_runner_tests/testDirectory");

    jobRunner::Command cmdWC("wc", jobRunner::commandType::EXEC);
    cmdWC.addArg("-l");

    jobRunner::Job job;
    job.addCommandToPipeline(&cmdLS);
    job.addCommandToPipeline(&cmdWC);

    job.runJob();
    SUCCEED();
}
/**
 * @brief Run a pipe of commands of size three
 */
TEST(JobRunnerTests, RunPipeJob2) {
    jobRunner::Command cmdLS("ls", jobRunner::commandType::EXEC);
    cmdLS.addArg("-Ggh");
    cmdLS.addArg("-la");
    cmdLS.addArg("../tests/files/job_runner_tests/testDirectory");

    jobRunner::Command cmdGrep("grep", jobRunner::commandType::EXEC);
    cmdGrep.addArg("cat");

    jobRunner::Command cmdWC("wc", jobRunner::commandType::EXEC);
    cmdWC.addArg("-l");

    jobRunner::Job job;
    job.addCommandToPipeline(&cmdLS);
    job.addCommandToPipeline(&cmdGrep);
    job.addCommandToPipeline(&cmdWC);

    job.runJob();
    SUCCEED();
}
/**
 * @brief Run a command job where we want to save return info
 */
TEST(JobRunnerTests, RunSaveReturnJob) {
    jobRunner::Command cmdLS("ls", jobRunner::commandType::EXEC);
    cmdLS.addArg("-Ggh");
    cmdLS.addArg("--time-style=+");
    cmdLS.addArg("../tests/files/job_runner_tests/testDirectory");

    jobRunner::Job job;
    job.addCommandToPipeline(&cmdLS);
    job.setJobToSave(true);

    jobRunner::JobInfo returnInfo = job.runJob();
    std::cout << "Standard Output:\n" << std::get<0>(returnInfo) << "\n";
    std::cout << "Standard Error:\n" << std::get<1>(returnInfo) << "\n";
    std::cout << "Return Code: \n" << std::get<2>(returnInfo) << "\n";
    SUCCEED();
}
/**
 * @brief Run a command job where we want to save return info and big output
 */
TEST(JobRunnerTests, RunSaveReturnJob2) {
    jobRunner::Command cmdCat("cat", jobRunner::commandType::EXEC);
    cmdCat.addArg("../tests/files/job_runner_tests/testDirectory/cat.txt");

    jobRunner::Job job;
    job.addCommandToPipeline(&cmdCat);
    job.setJobToSave(true);

    jobRunner::JobInfo returnInfo = job.runJob();
    std::cout << "Standard Output:\n" << std::get<0>(returnInfo) << "\n";
    std::cout << "Standard Error:\n" << std::get<1>(returnInfo) << "\n";
    std::cout << "Return Code: \n" << std::get<2>(returnInfo) << "\n";
    SUCCEED();
}
/**
 * @brief Run a command job where we want to save return info, but
 *        the command returns an error code
 */
TEST(JobRunnerTests, RunReturnJob3) {
    jobRunner::Command cmdLS("ls", jobRunner::commandType::EXEC);
    cmdLS.addArg("badDirectory/");

    jobRunner::Job job;
    job.addCommandToPipeline(&cmdLS);
    job.setJobToSave(true);

    jobRunner::JobInfo returnInfo = job.runJob();
    std::cout << "Standard Output:\n" << std::get<0>(returnInfo) << "\n";
    std::cout << "Standard Error:\n" << std::get<1>(returnInfo) << "\n";
    std::cout << "Return Code: \n" << std::get<2>(returnInfo) << "\n";
    SUCCEED();
}
/**
 * @brief Run a command job where we want to save return info and output bigger than
 *        buffer size
 */
TEST(JobRunnerTests, RunSaveReturnJob4) {
    // Need to save in a much bigger buffer for this cat
    jobRunner::Command cmdCat("cat", jobRunner::commandType::EXEC);
    cmdCat.addArg("../tests/files/job_runner_tests/testDirectory/cat2.txt");

    jobRunner::Job job;
    job.addCommandToPipeline(&cmdCat);
    job.setJobToSave(true);

    jobRunner::JobInfo returnInfo = job.runJob();
    std::cout << "Standard Output:\n" << std::get<0>(returnInfo) << "\n";
    std::cout << "Standard Error:\n" << std::get<1>(returnInfo) << "\n";
    std::cout << "Return Code: \n" << std::get<2>(returnInfo) << "\n";
    SUCCEED();
}
/**
 * @brief Run a command job where we want to save return info and output bigger than
 *        buffer size
 */
TEST(JobRunnerTests, RunMockedCommand) {
    jobRunner::Command cmdLS("ls", jobRunner::EXEC);
    cmdLS.addArg("-la");
    cmdLS.addArg("-h");

    jobRunner::Command cmdGrep("grep", jobRunner::EXEC);
    cmdGrep.addArg("C");

    jobRunner::Command cmdWC("wc", jobRunner::EXEC);
    cmdWC.addArg("-l");

    jobRunner::Job job;
    job.addCommandToPipeline(&cmdLS);
    job.addCommandToPipeline(&cmdGrep);
    job.addCommandToPipeline(&cmdWC);
    job.setJobToMock(true);

    job.runJob();
}
int main(int argc, char** argv) {
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}