#!/bin/bash

if [ $# -ne 1 ]; then
    echo "Usage: ./get_authors_str.sh (filename)
    Provide 1 filename as an argument & it will output a comment that can be put into the beginning of a file.
Example:
  /*
   *  Project: IFJ24
   *
   *  Authors:
   *      - Fundgod (fundgod.savin@gmail.com)
   *      - Nurdaulet Turar (nurdaulet.turar.cz@gmail.com)
   *
   */
"
    exit
fi

authors=$(git log --pretty=format:"%h|%an|%ae" --follow -- "$1" | awk -F '|' '!/^b84d05d/ { print $2 " (" $3 ")" }' | sort | uniq)

modified_date=$(LC_TIME=en_US.UTF-8 date +"%B %d, %Y")

echo "/*
 *  Project: IFJ24
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

