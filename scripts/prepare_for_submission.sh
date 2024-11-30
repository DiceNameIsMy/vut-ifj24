#!/bin/bash

set -e

./scripts/local_run_tests.sh

pytest

dest_dir="./submission"

find . -type f \( -name "*.c" -o -name "*.h" \) | python scripts/add_authors.py

find "./src" -type f -name "*.c" -exec cp {} "$dest_dir" \;
find "./include" -type f -name "*.h" -exec cp {} "$dest_dir" \;

# Discard unstaged changes. They appear when the script is ran to add authors to the files.
git restore .

cp ./docs/dokumentace.pdf ./${dest_dir}/dokumentace.pdf

content="xpopov10:25\nxturar00:25\nxkraso00:25\nxsavin00:25"
echo -e "$content" > ./${dest_dir}/rozdeleni

tar -czf xpopov10.tar.gz -C ./${dest_dir} .

remote_user="xturarn00"
remote_host="merlin.fit.vutbr.cz"
remote_dir="IFJ"

scp -r xpopov10.tar.gz "$remote_user@$remote_host:$remote_dir"

ssh "$remote_user@$remote_host" 'bash -s' <<'ENDSSH'
rm -Rf IFJ/tmp2 && echo -e "y\n" | bash /pub/courses/ifj/is_it_ok.sh IFJ/xpopov10.tar.gz IFJ/tmp2
ENDSSH
