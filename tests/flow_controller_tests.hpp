/**
 * @file flow_controller_tests.hpp
 * @brief Contains header content for flow_controller_tests.cpp
 *
 */

#ifndef COMMANDER_FLOW_CONTROLLER_TESTS_HPP
#define COMMANDER_FLOW_CONTROLLER_TESTS_HPP

#include "source/flow_controller/flow_controller.hpp"
#include "source/job_runner/job_runner_linux.hpp"
#include "source/lexer/lexer.hpp"
#include "source/parser/parser.hpp"
#include "source/type_checker/type_checker.hpp"
#include "source/util/commander_exception.hpp"
#include "source/util/io.hpp"
#include <fstream>
#include <gtest/gtest.h>

/**
 * Parser used in the tests (initialized here to prevent initializing it for every test)
 */
Parser::Parser parser;

/**
 * @brief Represents the format for flow controller tests that successfully run. Tests take in a file path to file to
 * run, and another file path to file with expected output.
 */
struct FlowControllerPassTests : ::testing::TestWithParam<std::tuple<std::string, std::string>> {};

/**
 * @brief Represents the format for flow controller tests that should fail (i.e. throw an exception). Tests take in a
 * file to run.
 */
struct FlowControllerFailTests : ::testing::TestWithParam<std::string> {};

/**
 * Creates test suite, with list of all the files that need to be ran with their expected output
 */
INSTANTIATE_TEST_SUITE_P(
        FlowControllerTestFiles, FlowControllerPassTests,
        ::testing::Values(
                std::make_tuple("000.cmdr", "000.cmdr.expected"), std::make_tuple("001.cmdr", "001.cmdr.expected"),
                std::make_tuple("002.cmdr", "002.cmdr.expected"), std::make_tuple("003.cmdr", "003.cmdr.expected"),
                std::make_tuple("004.cmdr", "004.cmdr.expected"), std::make_tuple("005.cmdr", "005.cmdr.expected"),
                std::make_tuple("006.cmdr", "006.cmdr.expected"), std::make_tuple("007.cmdr", "007.cmdr.expected"),
                std::make_tuple("008.cmdr", "008.cmdr.expected"), std::make_tuple("009.cmdr", "009.cmdr.expected"),
                std::make_tuple("010.cmdr", "010.cmdr.expected"), std::make_tuple("011.cmdr", "011.cmdr.expected"),
                std::make_tuple("012.cmdr", "012.cmdr.expected"), std::make_tuple("013.cmdr", "013.cmdr.expected"),
                std::make_tuple("014.cmdr", "014.cmdr.expected"), std::make_tuple("015.cmdr", "015.cmdr.expected"),
                std::make_tuple("016.cmdr", "016.cmdr.expected"), std::make_tuple("017.cmdr", "017.cmdr.expected"),
                std::make_tuple("018.cmdr", "018.cmdr.expected"), std::make_tuple("019.cmdr", "019.cmdr.expected"),
                std::make_tuple("020.cmdr", "020.cmdr.expected"), std::make_tuple("021.cmdr", "021.cmdr.expected"),
                std::make_tuple("022.cmdr", "022.cmdr.expected"), std::make_tuple("023.cmdr", "023.cmdr.expected"),
                std::make_tuple("024.cmdr", "024.cmdr.expected"), std::make_tuple("025.cmdr", "025.cmdr.expected"),
                std::make_tuple("026.cmdr", "026.cmdr.expected"), std::make_tuple("027.cmdr", "027.cmdr.expected"),
                std::make_tuple("028.cmdr", "028.cmdr.expected"), std::make_tuple("029.cmdr", "029.cmdr.expected"),
                std::make_tuple("030.cmdr", "030.cmdr.expected"), std::make_tuple("031.cmdr", "031.cmdr.expected"),
                std::make_tuple("032.cmdr", "032.cmdr.expected"), std::make_tuple("033.cmdr", "033.cmdr.expected"),
                std::make_tuple("034.cmdr", "034.cmdr.expected"), std::make_tuple("035.cmdr", "035.cmdr.expected"),
                std::make_tuple("036.cmdr", "036.cmdr.expected"), std::make_tuple("037.cmdr", "037.cmdr.expected"),
                std::make_tuple("038.cmdr", "038.cmdr.expected"), std::make_tuple("039.cmdr", "039.cmdr.expected"),
                std::make_tuple("040.cmdr", "040.cmdr.expected"), std::make_tuple("041.cmdr", "041.cmdr.expected"),
                std::make_tuple("042.cmdr", "042.cmdr.expected"), std::make_tuple("043.cmdr", "043.cmdr.expected"),
                std::make_tuple("044.cmdr", "044.cmdr.expected"), std::make_tuple("045.cmdr", "045.cmdr.expected"),
                std::make_tuple("046.cmdr", "046.cmdr.expected"), std::make_tuple("047.cmdr", "047.cmdr.expected"),
                std::make_tuple("048.cmdr", "048.cmdr.expected"), std::make_tuple("049.cmdr", "049.cmdr.expected"),
                std::make_tuple("050.cmdr", "050.cmdr.expected"), std::make_tuple("051.cmdr", "051.cmdr.expected"),
                std::make_tuple("052.cmdr", "052.cmdr.expected"), std::make_tuple("053.cmdr", "053.cmdr.expected"),
                std::make_tuple("054.cmdr", "054.cmdr.expected"), std::make_tuple("055.cmdr", "055.cmdr.expected"),
                std::make_tuple("056.cmdr", "056.cmdr.expected"), std::make_tuple("057.cmdr", "057.cmdr.expected"),
                std::make_tuple("058.cmdr", "058.cmdr.expected"), std::make_tuple("059.cmdr", "059.cmdr.expected"),
                std::make_tuple("060.cmdr", "060.cmdr.expected"), std::make_tuple("061.cmdr", "061.cmdr.expected"),
                std::make_tuple("062.cmdr", "062.cmdr.expected"), std::make_tuple("063.cmdr", "063.cmdr.expected"),
                std::make_tuple("064.cmdr", "064.cmdr.expected"), std::make_tuple("065.cmdr", "065.cmdr.expected"),
                std::make_tuple("066.cmdr", "066.cmdr.expected"), std::make_tuple("067.cmdr", "067.cmdr.expected"),
                std::make_tuple("068.cmdr", "068.cmdr.expected"), std::make_tuple("069.cmdr", "069.cmdr.expected"),
                std::make_tuple("070.cmdr", "070.cmdr.expected"), std::make_tuple("071.cmdr", "071.cmdr.expected"),
                std::make_tuple("072.cmdr", "072.cmdr.expected"), std::make_tuple("073.cmdr", "073.cmdr.expected"),
                std::make_tuple("074.cmdr", "074.cmdr.expected"), std::make_tuple("075.cmdr", "075.cmdr.expected"),
                std::make_tuple("076.cmdr", "076.cmdr.expected"), std::make_tuple("077.cmdr", "077.cmdr.expected"),
                std::make_tuple("078.cmdr", "078.cmdr.expected"), std::make_tuple("079.cmdr", "079.cmdr.expected"),
                std::make_tuple("080.cmdr", "080.cmdr.expected"), std::make_tuple("081.cmdr", "081.cmdr.expected"),
                std::make_tuple("082.cmdr", "082.cmdr.expected"), std::make_tuple("083.cmdr", "083.cmdr.expected"),
                std::make_tuple("084.cmdr", "084.cmdr.expected"), std::make_tuple("085.cmdr", "085.cmdr.expected"),
                std::make_tuple("086.cmdr", "086.cmdr.expected"), std::make_tuple("087.cmdr", "087.cmdr.expected"),
                std::make_tuple("088.cmdr", "088.cmdr.expected"), std::make_tuple("089.cmdr", "089.cmdr.expected"),
                std::make_tuple("090.cmdr", "090.cmdr.expected"), std::make_tuple("091.cmdr", "091.cmdr.expected"),
                std::make_tuple("092.cmdr", "092.cmdr.expected"), std::make_tuple("093.cmdr", "093.cmdr.expected"),
                std::make_tuple("094.cmdr", "094.cmdr.expected"), std::make_tuple("095.cmdr", "095.cmdr.expected"),
                std::make_tuple("096.cmdr", "096.cmdr.expected"), std::make_tuple("097.cmdr", "097.cmdr.expected"),
                std::make_tuple("098.cmdr", "098.cmdr.expected"), std::make_tuple("099.cmdr", "099.cmdr.expected"),
                std::make_tuple("100.cmdr", "100.cmdr.expected"), std::make_tuple("101.cmdr", "101.cmdr.expected"),
                std::make_tuple("102.cmdr", "102.cmdr.expected"), std::make_tuple("103.cmdr", "103.cmdr.expected"),
                std::make_tuple("104.cmdr", "104.cmdr.expected"), std::make_tuple("105.cmdr", "105.cmdr.expected"),
                std::make_tuple("106.cmdr", "106.cmdr.expected"), std::make_tuple("107.cmdr", "107.cmdr.expected"),
                std::make_tuple("108.cmdr", "108.cmdr.expected"), std::make_tuple("109.cmdr", "109.cmdr.expected"),
                std::make_tuple("110.cmdr", "110.cmdr.expected"), std::make_tuple("111.cmdr", "111.cmdr.expected"),
                std::make_tuple("112.cmdr", "112.cmdr.expected"), std::make_tuple("113.cmdr", "113.cmdr.expected"),
                std::make_tuple("114.cmdr", "114.cmdr.expected"), std::make_tuple("115.cmdr", "115.cmdr.expected"),
                std::make_tuple("116.cmdr", "116.cmdr.expected"), std::make_tuple("117.cmdr", "117.cmdr.expected"),
                std::make_tuple("118.cmdr", "118.cmdr.expected"), std::make_tuple("119.cmdr", "119.cmdr.expected"),
                std::make_tuple("120.cmdr", "120.cmdr.expected")));

/**
 * Creates test suite, with list of all the files that need to be ran that are expected to fail (as a result of a
 * CommanderException being thrown)
 */
INSTANTIATE_TEST_SUITE_P(FlowControllerTestFiles, FlowControllerFailTests,
                         ::testing::Values("000.cmdr", "001.cmdr", "002.cmdr", "003.cmdr", "004.cmdr", "005.cmdr",
                                           "006.cmdr", "007.cmdr", "008.cmdr", "009.cmdr", "010.cmdr", "011.cmdr",
                                           "012.cmdr", "013.cmdr", "014.cmdr", "015.cmdr", "016.cmdr", "017.cmdr",
                                           "018.cmdr", "019.cmdr", "020.cmdr", "021.cmdr", "022.cmdr", "023.cmdr",
                                           "024.cmdr", "025.cmdr", "026.cmdr", "027.cmdr", "028.cmdr", "029.cmdr",
                                           "030.cmdr", "031.cmdr", "032.cmdr", "033.cmdr", "034.cmdr", "035.cmdr",
                                           "036.cmdr", "037.cmdr", "038.cmdr", "039.cmdr", "040.cmdr", "041.cmdr",
                                           "042.cmdr", "043.cmdr", "044.cmdr", "045.cmdr", "046.cmdr", "047.cmdr",
                                           "048.cmdr", "049.cmdr", "050.cmdr", "051.cmdr", "052.cmdr", "053.cmdr",
                                           "054.cmdr", "055.cmdr", "056.cmdr", "057.cmdr", "058.cmdr", "059.cmdr",
                                           "060.cmdr", "061.cmdr", "062.cmdr", "063.cmdr", "064.cmdr", "065.cmdr",
                                           "066.cmdr", "067.cmdr", "068.cmdr", "069.cmdr", "070.cmdr", "071.cmdr",
                                           "072.cmdr", "073.cmdr", "074.cmdr", "075.cmdr", "076.cmdr", "077.cmdr",
                                           "078.cmdr", "079.cmdr", "080.cmdr", "081.cmdr", "082.cmdr", "083.cmdr",
                                           "084.cmdr", "085.cmdr", "086.cmdr", "087.cmdr", "088.cmdr", "089.cmdr",
                                           "090.cmdr", "091.cmdr", "092.cmdr", "093.cmdr", "094.cmdr", "095.cmdr",
                                           "096.cmdr", "097.cmdr", "098.cmdr", "099.cmdr", "100.cmdr", "101.cmdr",
                                           "102.cmdr", "103.cmdr", "104.cmdr", "105.cmdr", "106.cmdr", "107.cmdr",
                                           "108.cmdr", "109.cmdr", "110.cmdr", "111.cmdr", "112.cmdr"));

#endif  // COMMANDER_FLOW_CONTROLLER_TESTS_HPP
