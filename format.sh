#!/bin/bash
# This script runs clang-format on all source files given below
files=(
  "source/lexer/lexer.cpp"
  "source/lexer/lexer.hpp"
  "source/util/commander_exception.cpp"
  "source/util/commander_exception.hpp"
  "tests/lexer_tests.cpp"
  "tests/lexer_tests.hpp"
)
for file in "${files[@]}"; do
  clang-format -i -style=file "$file"
done