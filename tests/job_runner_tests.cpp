#include "source/job_runner/job_runner.hpp"
#include <gtest/gtest.h>
TEST(RunSimpleJobs, RunLS) {
    // >> ls
    jobRunner::ProcessArgs args;
    args.pushArg("ls");
    args.pushArg("-la");
    jobRunner::Process processLS;
    processLS.name = "ls";
    processLS.args = args;
    processLS.flags = jobRunner::NO_PIPE;

    EXPECT_NO_FATAL_FAILURE(jobRunner::runJob(&processLS));
}
TEST(RunBackgroundJob, RunSleep) {
    // >> sleep 20s
    jobRunner::ProcessArgs args;
    args.pushArg("sleep");
    args.pushArg("20s");
    jobRunner::Process processSleep;
    processSleep.name = "sleep";
    processSleep.args = args;
    processSleep.flags = jobRunner::BACKGROUND;

    EXPECT_NO_FATAL_FAILURE(jobRunner::runJob(&processSleep));
}
TEST(RunPipeJobs, RunLSWC) {
    // >> ls -la | wc -l
    jobRunner::ProcessArgs argsWC;
    argsWC.pushArg("wc");
    argsWC.pushArg("-l");
    jobRunner::Process processWC;
    processWC.name = "wc";
    processWC.args = argsWC;
    processWC.flags = jobRunner::PIPE;

    jobRunner::ProcessArgs argsLS;
    argsLS.pushArg("ls");
    argsLS.pushArg("-la");
    jobRunner::Process processLS;
    processLS.name = "ls";
    processLS.args = argsLS;
    processLS.pipe = &processWC;
    processLS.flags = jobRunner::PIPE;

    EXPECT_NO_FATAL_FAILURE(jobRunner::runJob(&processLS));
}

TEST(RunPipeJobs, RunLSGrepWC) {
    // >> ls -la | grep ninja | wc -l
    jobRunner::ProcessArgs argsWC;
    argsWC.pushArg("wc");
    argsWC.pushArg("-l");
    jobRunner::Process processWC;
    processWC.name = "wc";
    processWC.args = argsWC;
    processWC.pipe = nullptr;
    processWC.flags = jobRunner::PIPE;

    jobRunner::ProcessArgs argsGrep;
    argsGrep.pushArg("grep");
    argsGrep.pushArg("ninja");
    jobRunner::Process processGrep;
    processGrep.name = "grep";
    processGrep.args = argsGrep;
    processGrep.pipe = &processWC;
    processGrep.flags = jobRunner::PIPE;

    jobRunner::ProcessArgs argsLS;
    argsLS.pushArg("ls");
    argsLS.pushArg("-la");
    jobRunner::Process processLS;
    processLS.name = "ls";
    processLS.args = argsLS;
    processLS.pipe = &processGrep;
    processLS.flags = jobRunner::PIPE;

    EXPECT_NO_FATAL_FAILURE(jobRunner::runJob(&processLS));
}
int main(int argc, char** argv) {
    testing::InitGoogleTest(&argc, argv);
    RUN_ALL_TESTS();
}