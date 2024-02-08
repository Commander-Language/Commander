#pragma once

#include <string>
#include <tuple>
#include <vector>

using List = std::vector<std::string>;
using ReturnInfo = std::tuple<std::string, std::string, int>;

const int SUCCESS = 0;
const int FAIL = 1;
const int ERROR = 2;
