#include "source/job_runner/job_runner.hpp"
#include <fstream>
#include <gtest/gtest.h>
#include <iostream>
std::string getFileContents(std::string filePath){
    std::ifstream file(filePath);
    if(!file.is_open())
        throw "Can't find file";
    std::stringstream buf;
    buf << file.rdbuf();
    return buf.str();
}
TEST(JobRunnerTests, RunSimpleJobCat){
    jobRunner::Command cmdCat("cat", jobRunner::commandType::EXEC);
    cmdCat.addArg("../tests/files/job_runner_tests/testDirectory/cat.txt");

    jobRunner::Job job;
    job.addCommandToPipeline(&cmdCat);

    job.runJob();
    SUCCEED();
}
TEST(JobRunnerTests, RunSimpleJobLS){
    jobRunner::Command cmdLS("ls", jobRunner::commandType::EXEC);
    cmdLS.addArg("-Ggh");
    cmdLS.addArg("--time-style=+");
    cmdLS.addArg("../tests/files/job_runner_tests/testDirectory");

    jobRunner::Job job;
    job.addCommandToPipeline(&cmdLS);

    job.runJob();
    SUCCEED();
}
TEST(JobRunnerTests, RunSimpleJobCat2){
    jobRunner::Command cmdCat("cat", jobRunner::commandType::EXEC);
    cmdCat.addArg("../tests/files/job_runner_tests/testDirectory/cat.txt");

    jobRunner::Job job;
    job.addCommandToPipeline(&cmdCat);

    job.runJob();

    SUCCEED();
}
TEST(JobRunnerTests, RunBackgroundJob){
    jobRunner::Command cmdSleep("sleep", jobRunner::commandType::EXEC);
    cmdSleep.addArg("20s");

    jobRunner::Job job;
    job.addCommandToPipeline(&cmdSleep);

    SUCCEED();
}
TEST(JobRunnerTests, RunPipeJob1){
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
TEST(JobRunnerTests, RunPipeJob2){
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
TEST(JobRunnerTests, RunSaveReturnJob){
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
TEST(JobRunnerTests, RunSaveReturnJob2){
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
TEST(JobRunnerTests, RunReturnJob3){
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
TEST(JobRunnerTests, RunSaveReturnJob4){
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
int main(int argc, char** argv) {
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}