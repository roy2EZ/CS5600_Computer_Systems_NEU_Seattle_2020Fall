#!/bin/bash
# bash "strict mode"
set -euo pipefail
IFS=$'\n\t'

# change directory to the directory containing the script
cd "${0%/*}"

MISSING_DEPENDENCIES=$(cat <<-END
	One or more dependencies are missing.
	Run the following command and try again:

	sudo apt install build-essential cmake valgrind
END
)

# check for required commands and packages
command -v cmake >/dev/null 2>&1 || { echo >&2 "${MISSING_DEPENDENCIES}"; exit 1; }
command -v valgrind >/dev/null 2>&1 || { echo >&2 "${MISSING_DEPENDENCIES}"; exit 1; }
dpkg -l build-essential >/dev/null 2>&1 || { echo >&2 "${MISSING_DEPENDENCIES}"; exit 1; }

# generate a Makefile and download source dependencies (e.g. googletest)
cmake CMakeLists.txt

# build ELFTester from the generated Makefile
make

# create unique temporary files and clean them up when we exit
tempfile() {
    tempprefix=$(basename "$0")
    mktemp /tmp/${tempprefix}.XXXXXX
}

STD_TEMPFILE=$(tempfile)
ERR_TEMPFILE=$(tempfile)

trap 'rm -f $STD_TEMPFILE $ERR_TEMPFILE' EXIT

# run ELFTester under valgrind and save the output to a tempfile
# if we get any errors back at all, set a flag
if valgrind --leak-check=yes ./ELFTester > $STD_TEMPFILE  2> $ERR_TEMPFILE; then
    ALL_PASS=0
else
    ALL_PASS=1
fi

cat $ERR_TEMPFILE
cat $STD_TEMPFILE

# This string is only written if valgrind detects no issues
if grep -qF "All heap blocks were freed -- no leaks are possible" $ERR_TEMPFILE; then
    echo -e "[ \e[32mPASS\e[39m ]: Valgrind memory leak check"
else
    echo -e "[ \e[31mFAIL\e[39m ]: Valgrind memory leak check"
    ALL_PASS=1
fi

# This string means all 18 tests passed (which means 0 failed)
if grep -qF "[  PASSED  ] 18 tests." $STD_TEMPFILE; then
    echo -e "[ \e[32mPASS\e[39m ]: Functional tests"
else
    echo -e "[ \e[31mFAIL\e[39m ]: Functional tests"
    ALL_PASS=1
fi

# Check to see if all of the checksums match the originals
if md5sum -c integrity.md5 > /dev/null 2> /dev/null; then
    echo -e "[ \e[32mPASS\e[39m ]: File integrity check"
else
    echo -e "[ \e[31mFAIL\e[39m ]: File integrity check; only answer/answer.c should be modified:"
    ALL_PASS=1
    # This command will produce an error; || true prevents pipefail from halting execution
    md5sum -c integrity.md5 | grep -Fv ": OK" || true
fi

# Write a summary pass/fail at the end
if [ $ALL_PASS = 0 ]; then
    echo -e "[ \e[32mPASS\e[39m ]: All checks"
else
    echo -e "[ \e[31mFAIL\e[39m ]: Some checks failed; see details above"
    # Exit nonzero to indicate something is wrong
    exit 1
fi
