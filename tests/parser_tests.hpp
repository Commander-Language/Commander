/**
 * @file parser_tests.hpp
 * @brief Contains header content for parser_tests.cpp
 *
 */

#ifndef COMMANDER_PARSER_TESTS_HPP
#define COMMANDER_PARSER_TESTS_HPP

#include "source/parser/ast_node.hpp"
#include "source/parser/parser.hpp"
#include <gtest/gtest.h>

/**
 * Parser used in the tests (initialized here to prevent initializing it for every test)
 */
Parser::Parser parser;

/**
 * @brief Represents the format for parser tests that successfully parse. Tests take in a file path to file to parse,
 * and another file path to file with expected S-Expression output.
 */
struct ParserParseTests : ::testing::TestWithParam<std::tuple<std::string, std::string>> {};

/**
 * @brief Represents the format for parser tests that should fail (i.e. throw an exception). Tests take in a file to
 * parse.
 */
struct ParserFailTests : ::testing::TestWithParam<std::string> {};

/**
 * Creates test suite, with list of all the files that need to be parsed with their expected S-Expression output
 */
INSTANTIATE_TEST_SUITE_P(
        ParserTestFiles, ParserParseTests,
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
                std::make_tuple("120.cmdr", "120.cmdr.expected"), std::make_tuple("121.cmdr", "121.cmdr.expected"),
                std::make_tuple("122.cmdr", "122.cmdr.expected"), std::make_tuple("123.cmdr", "123.cmdr.expected"),
                std::make_tuple("124.cmdr", "124.cmdr.expected"), std::make_tuple("125.cmdr", "125.cmdr.expected"),
                std::make_tuple("126.cmdr", "126.cmdr.expected"), std::make_tuple("127.cmdr", "127.cmdr.expected"),
                std::make_tuple("128.cmdr", "128.cmdr.expected"), std::make_tuple("129.cmdr", "129.cmdr.expected"),
                std::make_tuple("130.cmdr", "130.cmdr.expected"), std::make_tuple("131.cmdr", "131.cmdr.expected"),
                std::make_tuple("132.cmdr", "132.cmdr.expected"), std::make_tuple("133.cmdr", "133.cmdr.expected"),
                std::make_tuple("134.cmdr", "134.cmdr.expected"), std::make_tuple("135.cmdr", "135.cmdr.expected"),
                std::make_tuple("136.cmdr", "136.cmdr.expected"), std::make_tuple("137.cmdr", "137.cmdr.expected"),
                std::make_tuple("138.cmdr", "138.cmdr.expected"), std::make_tuple("139.cmdr", "139.cmdr.expected"),
                std::make_tuple("140.cmdr", "140.cmdr.expected"), std::make_tuple("141.cmdr", "141.cmdr.expected"),
                std::make_tuple("142.cmdr", "142.cmdr.expected"), std::make_tuple("143.cmdr", "143.cmdr.expected"),
                std::make_tuple("144.cmdr", "144.cmdr.expected"), std::make_tuple("145.cmdr", "145.cmdr.expected"),
                std::make_tuple("146.cmdr", "146.cmdr.expected"), std::make_tuple("147.cmdr", "147.cmdr.expected"),
                std::make_tuple("148.cmdr", "148.cmdr.expected"), std::make_tuple("149.cmdr", "149.cmdr.expected"),
                std::make_tuple("150.cmdr", "150.cmdr.expected"), std::make_tuple("151.cmdr", "151.cmdr.expected"),
                std::make_tuple("152.cmdr", "152.cmdr.expected"), std::make_tuple("153.cmdr", "153.cmdr.expected"),
                std::make_tuple("154.cmdr", "154.cmdr.expected"), std::make_tuple("155.cmdr", "155.cmdr.expected"),
                std::make_tuple("156.cmdr", "156.cmdr.expected"), std::make_tuple("157.cmdr", "157.cmdr.expected"),
                std::make_tuple("158.cmdr", "158.cmdr.expected"), std::make_tuple("159.cmdr", "159.cmdr.expected"),
                std::make_tuple("160.cmdr", "160.cmdr.expected"), std::make_tuple("161.cmdr", "161.cmdr.expected"),
                std::make_tuple("162.cmdr", "162.cmdr.expected"), std::make_tuple("163.cmdr", "163.cmdr.expected"),
                std::make_tuple("164.cmdr", "164.cmdr.expected"), std::make_tuple("165.cmdr", "165.cmdr.expected"),
                std::make_tuple("166.cmdr", "166.cmdr.expected"), std::make_tuple("167.cmdr", "167.cmdr.expected"),
                std::make_tuple("168.cmdr", "168.cmdr.expected"), std::make_tuple("169.cmdr", "169.cmdr.expected"),
                std::make_tuple("170.cmdr", "170.cmdr.expected"), std::make_tuple("171.cmdr", "171.cmdr.expected"),
                std::make_tuple("172.cmdr", "172.cmdr.expected"), std::make_tuple("173.cmdr", "173.cmdr.expected"),
                std::make_tuple("174.cmdr", "174.cmdr.expected"), std::make_tuple("175.cmdr", "175.cmdr.expected"),
                std::make_tuple("176.cmdr", "176.cmdr.expected"), std::make_tuple("177.cmdr", "177.cmdr.expected"),
                std::make_tuple("178.cmdr", "178.cmdr.expected"), std::make_tuple("179.cmdr", "179.cmdr.expected"),
                std::make_tuple("180.cmdr", "180.cmdr.expected"), std::make_tuple("181.cmdr", "181.cmdr.expected"),
                std::make_tuple("182.cmdr", "182.cmdr.expected"), std::make_tuple("183.cmdr", "183.cmdr.expected"),
                std::make_tuple("184.cmdr", "184.cmdr.expected"), std::make_tuple("185.cmdr", "185.cmdr.expected"),
                std::make_tuple("186.cmdr", "186.cmdr.expected"), std::make_tuple("187.cmdr", "187.cmdr.expected"),
                std::make_tuple("188.cmdr", "188.cmdr.expected"), std::make_tuple("189.cmdr", "189.cmdr.expected"),
                std::make_tuple("190.cmdr", "190.cmdr.expected"), std::make_tuple("191.cmdr", "191.cmdr.expected"),
                std::make_tuple("192.cmdr", "192.cmdr.expected"), std::make_tuple("193.cmdr", "193.cmdr.expected"),
                std::make_tuple("194.cmdr", "194.cmdr.expected"), std::make_tuple("195.cmdr", "195.cmdr.expected"),
                std::make_tuple("196.cmdr", "196.cmdr.expected"), std::make_tuple("197.cmdr", "197.cmdr.expected"),
                std::make_tuple("198.cmdr", "198.cmdr.expected"), std::make_tuple("199.cmdr", "199.cmdr.expected"),
                std::make_tuple("200.cmdr", "200.cmdr.expected"), std::make_tuple("201.cmdr", "201.cmdr.expected"),
                std::make_tuple("202.cmdr", "202.cmdr.expected"), std::make_tuple("203.cmdr", "203.cmdr.expected"),
                std::make_tuple("204.cmdr", "204.cmdr.expected"), std::make_tuple("205.cmdr", "205.cmdr.expected"),
                std::make_tuple("206.cmdr", "206.cmdr.expected"), std::make_tuple("207.cmdr", "207.cmdr.expected"),
                std::make_tuple("208.cmdr", "208.cmdr.expected"), std::make_tuple("209.cmdr", "209.cmdr.expected"),
                std::make_tuple("210.cmdr", "210.cmdr.expected"), std::make_tuple("211.cmdr", "211.cmdr.expected"),
                std::make_tuple("212.cmdr", "212.cmdr.expected"), std::make_tuple("213.cmdr", "213.cmdr.expected"),
                std::make_tuple("214.cmdr", "214.cmdr.expected"), std::make_tuple("215.cmdr", "215.cmdr.expected"),
                std::make_tuple("216.cmdr", "216.cmdr.expected"), std::make_tuple("217.cmdr", "217.cmdr.expected"),
                std::make_tuple("218.cmdr", "218.cmdr.expected"), std::make_tuple("219.cmdr", "219.cmdr.expected"),
                std::make_tuple("220.cmdr", "220.cmdr.expected"), std::make_tuple("221.cmdr", "221.cmdr.expected"),
                std::make_tuple("222.cmdr", "222.cmdr.expected"), std::make_tuple("223.cmdr", "223.cmdr.expected"),
                std::make_tuple("224.cmdr", "224.cmdr.expected"), std::make_tuple("225.cmdr", "225.cmdr.expected"),
                std::make_tuple("226.cmdr", "226.cmdr.expected"), std::make_tuple("227.cmdr", "227.cmdr.expected"),
                std::make_tuple("228.cmdr", "228.cmdr.expected"), std::make_tuple("229.cmdr", "229.cmdr.expected"),
                std::make_tuple("230.cmdr", "230.cmdr.expected"), std::make_tuple("231.cmdr", "231.cmdr.expected"),
                std::make_tuple("232.cmdr", "232.cmdr.expected"), std::make_tuple("233.cmdr", "233.cmdr.expected"),
                std::make_tuple("234.cmdr", "234.cmdr.expected"), std::make_tuple("235.cmdr", "235.cmdr.expected"),
                std::make_tuple("236.cmdr", "236.cmdr.expected"), std::make_tuple("237.cmdr", "237.cmdr.expected"),
                std::make_tuple("238.cmdr", "238.cmdr.expected"), std::make_tuple("239.cmdr", "239.cmdr.expected"),
                std::make_tuple("240.cmdr", "240.cmdr.expected"), std::make_tuple("241.cmdr", "241.cmdr.expected"),
                std::make_tuple("242.cmdr", "242.cmdr.expected"), std::make_tuple("243.cmdr", "243.cmdr.expected"),
                std::make_tuple("244.cmdr", "244.cmdr.expected"), std::make_tuple("245.cmdr", "245.cmdr.expected"),
                std::make_tuple("246.cmdr", "246.cmdr.expected"), std::make_tuple("247.cmdr", "247.cmdr.expected"),
                std::make_tuple("248.cmdr", "248.cmdr.expected"), std::make_tuple("249.cmdr", "249.cmdr.expected"),
                std::make_tuple("250.cmdr", "250.cmdr.expected"), std::make_tuple("251.cmdr", "251.cmdr.expected"),
                std::make_tuple("252.cmdr", "252.cmdr.expected"), std::make_tuple("253.cmdr", "253.cmdr.expected"),
                std::make_tuple("254.cmdr", "254.cmdr.expected"), std::make_tuple("255.cmdr", "255.cmdr.expected"),
                std::make_tuple("256.cmdr", "256.cmdr.expected"), std::make_tuple("257.cmdr", "257.cmdr.expected"),
                std::make_tuple("258.cmdr", "258.cmdr.expected"), std::make_tuple("259.cmdr", "259.cmdr.expected"),
                std::make_tuple("260.cmdr", "260.cmdr.expected"), std::make_tuple("261.cmdr", "261.cmdr.expected"),
                std::make_tuple("262.cmdr", "262.cmdr.expected"), std::make_tuple("263.cmdr", "263.cmdr.expected"),
                std::make_tuple("264.cmdr", "264.cmdr.expected"), std::make_tuple("265.cmdr", "265.cmdr.expected"),
                std::make_tuple("266.cmdr", "266.cmdr.expected"), std::make_tuple("267.cmdr", "267.cmdr.expected"),
                std::make_tuple("268.cmdr", "268.cmdr.expected"), std::make_tuple("269.cmdr", "269.cmdr.expected"),
                std::make_tuple("270.cmdr", "270.cmdr.expected"), std::make_tuple("271.cmdr", "271.cmdr.expected"),
                std::make_tuple("272.cmdr", "272.cmdr.expected"), std::make_tuple("273.cmdr", "273.cmdr.expected"),
                std::make_tuple("274.cmdr", "274.cmdr.expected"), std::make_tuple("275.cmdr", "275.cmdr.expected"),
                std::make_tuple("276.cmdr", "276.cmdr.expected"), std::make_tuple("277.cmdr", "277.cmdr.expected"),
                std::make_tuple("278.cmdr", "278.cmdr.expected"), std::make_tuple("279.cmdr", "279.cmdr.expected"),
                std::make_tuple("280.cmdr", "280.cmdr.expected"), std::make_tuple("281.cmdr", "281.cmdr.expected"),
                std::make_tuple("282.cmdr", "282.cmdr.expected"), std::make_tuple("283.cmdr", "283.cmdr.expected"),
                std::make_tuple("284.cmdr", "284.cmdr.expected"), std::make_tuple("285.cmdr", "285.cmdr.expected"),
                std::make_tuple("286.cmdr", "286.cmdr.expected"), std::make_tuple("287.cmdr", "287.cmdr.expected"),
                std::make_tuple("288.cmdr", "288.cmdr.expected"), std::make_tuple("289.cmdr", "289.cmdr.expected"),
                std::make_tuple("290.cmdr", "290.cmdr.expected"), std::make_tuple("291.cmdr", "291.cmdr.expected"),
                std::make_tuple("292.cmdr", "292.cmdr.expected"), std::make_tuple("293.cmdr", "293.cmdr.expected"),
                std::make_tuple("294.cmdr", "294.cmdr.expected"), std::make_tuple("295.cmdr", "295.cmdr.expected"),
                std::make_tuple("296.cmdr", "296.cmdr.expected"), std::make_tuple("297.cmdr", "297.cmdr.expected")));

/**
 * Creates test suite, with list of all the files that need to be parsed that are expected to fail (as a result of a
 * CommanderException being thrown)
 */
INSTANTIATE_TEST_SUITE_P(
        ParserTestFiles, ParserFailTests,
        ::testing::Values(
                "000.cmdr", "001.cmdr", "002.cmdr", "003.cmdr", "004.cmdr", "005.cmdr", "006.cmdr", "007.cmdr",
                "008.cmdr", "009.cmdr", "010.cmdr", "011.cmdr", "012.cmdr", "013.cmdr", "014.cmdr", "015.cmdr",
                "016.cmdr", "017.cmdr", "018.cmdr", "019.cmdr", "020.cmdr", "021.cmdr", "022.cmdr", "023.cmdr",
                "024.cmdr", "025.cmdr", "026.cmdr", "027.cmdr", "028.cmdr", "029.cmdr", "030.cmdr", "031.cmdr",
                "032.cmdr", "033.cmdr", "034.cmdr", "035.cmdr", "036.cmdr", "037.cmdr", "038.cmdr", "039.cmdr",
                "040.cmdr", "041.cmdr", "042.cmdr", "043.cmdr", "044.cmdr", "045.cmdr", "046.cmdr", "047.cmdr",
                "048.cmdr", "049.cmdr", "050.cmdr", "051.cmdr", "052.cmdr", "053.cmdr", "054.cmdr", "055.cmdr",
                "056.cmdr", "057.cmdr", "058.cmdr", "059.cmdr", "060.cmdr", "061.cmdr", "062.cmdr", "063.cmdr",
                "064.cmdr", "065.cmdr", "066.cmdr", "067.cmdr", "068.cmdr", "069.cmdr", "070.cmdr", "071.cmdr",
                "072.cmdr", "073.cmdr", "074.cmdr", "075.cmdr", "076.cmdr", "077.cmdr", "078.cmdr", "079.cmdr",
                "080.cmdr", "081.cmdr", "082.cmdr", "083.cmdr", "074.cmdr", "085.cmdr", "086.cmdr", "087.cmdr",
                "088.cmdr", "089.cmdr", "090.cmdr", "091.cmdr", "092.cmdr", "093.cmdr", "094.cmdr", "095.cmdr",
                "096.cmdr", "097.cmdr", "098.cmdr", "099.cmdr", "100.cmdr", "101.cmdr", "102.cmdr", "103.cmdr",
                "104.cmdr", "105.cmdr", "106.cmdr", "107.cmdr", "108.cmdr", "109.cmdr", "110.cmdr", "111.cmdr",
                "112.cmdr", "113.cmdr", "114.cmdr", "115.cmdr", "116.cmdr", "117.cmdr", "118.cmdr", "119.cmdr",
                "120.cmdr", "121.cmdr", "122.cmdr", "123.cmdr", "124.cmdr", "125.cmdr", "126.cmdr", "127.cmdr"));

/**
 * @brief Takes in an ASTNodeList, and ensures they match what is contained in the expected output string
 *
 * @param nodes The ASTNodeList
 * @param expectedOutput The string containing the expected S-Expression output
 */
void expectOutputEqualsSExpressions(const Parser::ASTNodeList& nodes, const std::string& expectedOutput);

/**
 * @brief Takes in a file path, and goes through lexing and parsing it
 * @param filePath The file path
 */
void lexAndParse(const std::string& filePath);

#endif  // COMMANDER_PARSER_TESTS_HPP