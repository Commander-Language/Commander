#include "source/job_runner/job_runner.hpp"
#include <gtest/gtest.h>
TEST(RunSimpleJobs, RunLS) {
    jobRunner::ProcessArgs args;
    args.pushArg("ls");
    args.pushArg("-la");
    jobRunner::Process processLS;
    processLS.name = "ls";
    processLS.args = args;
    processLS.flags = jobRunner::NONE;

    EXPECT_NO_FATAL_FAILURE(jobRunner::runJob(&processLS));
}
int main(int argc, char** argv) {
    testing::InitGoogleTest(&argc, argv);
    RUN_ALL_TESTS();
}