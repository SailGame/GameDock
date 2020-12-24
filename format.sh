#!/bin/bash
format_files=`find games common -type f -name "*.cpp" -o -name "*.hpp"`

for file in $format_files
do
  clang-format -i "$file"
done