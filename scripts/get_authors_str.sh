#!/bin/bash

if [ $# -ne 1 ]; then
    echo "Usage: ./get_authors_str.sh (filename)
    Provide 1 filename as an argument & it will output a comment that can be put into the beginning of a file.
Example:
  /*
   *  Filename: ./src/lexer/token.c
   *
   *  Last Modified Date: October 13, 2024
   *
   *  Authors:
   *      - Fundgod (fundgod.savin@gmail.com)
   *      - Nurdaulet Turar (nurdaulet.turar.cz@gmail.com)
   *
   */
"
    exit
fi

authors=$(git log --follow -- "$1" | awk 'match($0, "Author: (.*) <(.*)>", arr) { printf("%s (%s)\n", arr[1], arr[2]) }' | sort | uniq)

modified_date=$(LC_TIME=en_US.UTF-8 date +"%B %d, %Y")

echo "/*
 *  Filename: $1
 *
 *  Last Modified Date: $modified_date
 *
 *  Authors:"

# Set IFS to newline
IFS=$'\n'
# Iterate over each line in the string
for author in $authors; do
    echo " *      - $author"
done
# Reset IFS to its default value (space, tab, newline)
unset IFS

echo " *
 */"

