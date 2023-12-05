#!/bin/bash

# List of directories to format (recursively)
directories=("source" "tests")

# Use find to locate all .cpp and .hpp files recursively in specified directories
files=$(find "${directories[@]}" -type f \( -name "*.cpp" -or -name "*.hpp" \))

# Format all the found files
for file in $files; do
  clang-format -i -style=file "$file"
done