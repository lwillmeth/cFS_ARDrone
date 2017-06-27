#!/bin/bash
#
##############################################################################
#
#  Copyright 2016 United States Government as represented by the
#  Administrator of the National Aeronautics and Space Administration.
#  All Other Rights Reserved.
#
#  This software was created at NASA's Johnson Space Center.
#  This software is governed by the NASA Open Source Agreement and may be
#  used, distributed and modified only pursuant to the terms of that
#  agreement.
#
#  Author: Allen Brown, Odyssey Space Research, LLC
#
##############################################################################
#
# Specifies and executes application-level tests for elf2cfetbl.
#
# Invoke with run_tests.sh CPUNAME and this script will test against
# the contents found in ./$CPUNAME.
#
# Optional invocation: "run_tests.sh CPUNAME clean" to clean up built tables.
#
# Run gen_test_tables.sh first to create the .o files and the 
# elf2cfetbl executable for testing.
#

# Platform/Toolchain-specific programs
HEXDUMP=/usr/bin/hexdump

#
# Functions
#

# A command to test expect a zero return value.
# Returns 0=success, 1=failure
function expect0 {

	NUMTESTS=`expr $NUMTESTS + 1`
	echo "========================="
	echo "Running test: expect0 $@"
	"$@"
	local retval=$?
	if [ $retval -ne 0 ]; then
		echo "FAIL: Test failure with test:  $1" >&2
		echo "FAIL: Return value was $retval"
		echo "========================="
		NUMFAIL=`expr $NUMFAIL + 1`
		return 1
	fi
	echo "...test passed."
	echo "========================="
	return 0
}

# A command to expect a non-zero return value.
# Returns 0=success, 1=failure
function expectFail {

	NUMTESTS=`expr $NUMTESTS + 1`
	echo "========================="
	echo "Running test: expectFail $@"
	"$@"
	local retval=$?
	if [ $retval -eq 0 ]; then
		echo "FAIL: Test failure with test:  $1 (expected fail but succeeded)" >&2
		echo "FAIL: Return value was $retval"
		echo "========================="
		NUMFAIL=`expr $NUMFAIL + 1`
		return 1
	fi
	echo "...test passed."
	echo "========================="
	return 0
}

# Tests for equal file size between two files.
# Returns 0=success, 1=failure
function expectEqualFileSize {

	NUMTESTS=`expr $NUMTESTS + 1`
	echo "========================="
	echo "Running test: expectEqualFileSize $@"

	if ! [ -f "$1" ]; then
		echo "FAIL: Test failure: checking file size on $1." >&2
		echo "FAIL: $1 is missing - can't test." >&2
		echo "========================="
		NUMFAIL=`expr $NUMFAIL + 1`
		return 1
	fi

	if ! [ -f "$2" ]; then
		echo "FAIL: Test failure: checking file size on $2." >&2
		echo "FAIL: $2 is missing - can't test." >&2
		echo "========================="
		NUMFAIL=`expr $NUMFAIL + 1`
		return 1
	fi

	sz1=$(wc -c <"$1")
	local retval=$?
	if [ $retval -ne 0 ]; then
		echo "FAIL: Test failure: checking file size on $1." >&2
		echo "FAIL: There was something wrong with the test harness itself." >&2
		echo "FAIL: Return value was $retval"
		echo "========================="
		NUMFAIL=`expr $NUMFAIL + 1`
		return 1
	fi
	sz2=$(wc -c <"$2")
	local retval=$?
	if [ $retval -ne 0 ]; then
		echo "FAIL: Test failure: checking file size on $1." >&2
		echo "FAIL: There was something wrong with the test harness itself." >&2
		echo "FAIL: Return value was $retval"
		echo "========================="
		NUMFAIL=`expr $NUMFAIL + 1`
		return 1
	fi
	if [ $sz1 -ne $sz2 ]; then
		echo "FAIL: Test failure: file sizes are not the same between $1 and $2." >&2
		echo "FAIL: $1 is $sz1 bytes."
		echo "FAIL: $2 is $sz2 bytes."
		echo "========================="
		NUMFAIL=`expr $NUMFAIL + 1`
		return 1
	fi
	echo "...test passed."
	echo "========================="
	return 0
}

# Tests for binary file difference between two files.
# Returns 0=success, 1=failure
function expectIdenticalFiles {

	NUMTESTS=`expr $NUMTESTS + 1`
	echo "========================="
	echo "Running test: expectIdenticalFiles $@"

	if ! [ -f "$1" ]; then
		echo "FAIL: Test failure: $1 and $2, checking binary differences." >&2
		echo "FAIL: $1 is missing - can't test." >&2
		echo "========================="
		NUMFAIL=`expr $NUMFAIL + 1`
		return 1
	fi

	if ! [ -f "$2" ]; then
		echo "FAIL: Test failure: $1 and $2, checking binary differences." >&2
		echo "FAIL: $2 is missing - can't test." >&2
		echo "========================="
		NUMFAIL=`expr $NUMFAIL + 1`
		return 1
	fi

	diff $1 $2 > /dev/null 2>&1
	local retval=$?
	if [ $retval -gt 0 ]; then
		echo "FAIL: Test failure: $1 and $2, checking binary differences." >&2
		echo "FAIL: Return value was $retval"
		echo "========================="
		NUMFAIL=`expr $NUMFAIL + 1`
		return 1
	fi
	echo "...test passed."
	echo "========================="
	return 0
}

# Tests for binary file difference between two files and 
# makes a binary object dump of each when there is a failure.
# Returns 0=success, 1=failure
function expectIdenticalFilesAndReport {

	NUMTESTS=`expr $NUMTESTS + 1`
	echo "========================="
	echo "Running test: expectIdenticalFilesAndReport $@"

	if ! [ -f "$1" ]; then
		echo "FAIL: Test failure: $1 and $2,"
		echo "FAIL:    checking binary differences (with report)." >&2
		echo "FAIL: $1 is missing - can't test." >&2
		echo "========================="
		NUMFAIL=`expr $NUMFAIL + 1`
		return 1
	fi

	if ! [ -f "$2" ]; then
		echo "FAIL: Test failure: $1 and $2,"
		echo "FAIL:    checking binary differences (with report)." >&2
		echo "FAIL: $2 is missing - can't test." >&2
		echo "========================="
		NUMFAIL=`expr $NUMFAIL + 1`
		return 1
	fi

	diff $1 $2 > /dev/null 2>&1
	local retval=$?
	if [ $retval -gt 0 ]; then
		echo "FAIL: Test failure: $1 and $2,"
		echo "FAIL:    checking binary differences (with report)." >&2
		echo "FAIL: Return value was $retval"
		echo "FAIL: Dumping $1 to $1.dump for analysis"
		$HEXDUMP -C "$1" > "$1.dump"
		echo "FAIL: Dumping $2 to $2.dump for analysis"
		$HEXDUMP -C "$2" > "$2.dump"
		echo "========================="
		NUMFAIL=`expr $NUMFAIL + 1`
		return 1
	fi
	echo "...test passed."
	echo "========================="
	return 0
}

# Tests that a file exists
# Returns 0=success, 1=failure
function expectFile {
	echo "========================="
	echo "Running test: expectFile $@"

	NUMTESTS=`expr $NUMTESTS + 1`
	if ! [ -f "$1" ]; then
		echo "========================="
		echo "FAIL: Test failure: $1 does not exist." >&2
		echo "========================="
		NUMFAIL=`expr $NUMFAIL + 1`
		return 1
	fi
	echo "...test passed."
	echo "========================="
	return 0
}

# Cleanup function
function cleanTestDir {
	if [ -d "$1" ]; then 
		cd "$1"
		rm -f *.tbl *.bin *.dump
		echo "Cleaned $1."
	fi
	return 0
}

# Invocation
function invocation {
	echo "Error: invoke as run_tests.sh CPUNAME"
	echo "              or run_tests.sh CPUNAME clean."
	return 0
}

#
# Tests
#

# Test harness
NUMTESTS=0
NUMFAIL=0

# The app under test
APP=./elf2cfetbl

if [ "$#" -lt 1 ]; then
	invocation
	exit 1
fi

if [ -d "$1" ]; then
	cd "$1"
else
	echo "Error: $1 not found."
	invocation
	exit 1
fi

if [ "$2" == "clean" ]; then
	cleanTestDir "$1"
	exit 0
fi

echo "=================================="
echo "$APP tests begin in $1."
echo "=================================="

# Clean up before tests.
cleanTestDir

# A few simple harness tests, if needed.
# These should fail:
#expect0 /bin/false
#expectFail /bin/true
# These should pass:
#expect0 /bin/true
#expectFail /bin/false
#expectEqualFileSize ./Readme.txt ./Readme.txt
#expectIdenticalFiles ./Readme.txt ./Readme.txt
#expectFile ./Readme.txt

# Test basic elf2cfetbl functionality: A basic table builds
expectFile SampleTblImg.o
expect0 "$APP" SampleTblImg.o
expectFile MyTblDefault.bin

# Test we get an empty simple SC RTS table
expectFile ut_rtsA.o
expect0 "$APP" ut_rtsA.o
expectFile ut_rtsA.tbl

# Tests for successful table builds:
SUCCESS_TABLES=(sc_rts001.o sc_rts002.o sc_rts001alt.o sc_rts002alt.o sc_ats1.o sc_ats1alt.o)

for i in ${!SUCCESS_TABLES[*]}
do
	TBLFILE="${SUCCESS_TABLES[$i]}"
	expectFile "$TBLFILE"
	expect0 "$APP" "$TBLFILE"
done

# Tests to check consistent file size (post-processing vs. manual defn.)
expectEqualFileSize sc_rts001.tbl ut_rtsA.tbl
expectEqualFileSize sc_rts001.tbl sc_rts002.tbl
expectEqualFileSize sc_rts001.tbl sc_rts001alt.tbl
expectEqualFileSize sc_rts002.tbl sc_rts002alt.tbl
expectEqualFileSize sc_ats1.tbl sc_ats1alt.tbl

# Test for binary file identity (post-processing vs. manual defn.)
# Dump them if there is a problem.
expectIdenticalFilesAndReport sc_rts001.tbl sc_rts001alt.tbl
expectIdenticalFilesAndReport sc_rts002.tbl sc_rts002alt.tbl
expectIdenticalFilesAndReport sc_ats1.tbl sc_ats1alt.tbl

# Error handling tests for improper SC RTS tables.
expectFile ut_rts_elist.o
expectFail "$APP" ut_rts_elist.o
expectFile ut_rts_ebad1.o
expectFail "$APP" ut_rts_ebad1.o

# Error handling tests for missing symbols
expectFile ut_rts_badmin.o
expectFail "$APP" ut_rts_badmin.o
expectFile ut_rts_badmax.o
expectFail "$APP" ut_rts_badmax.o
expectFile ut_rts_badnc.o
expectFail "$APP" ut_rts_badnc.o
expectFile ut_rts_badcmd.o
expectFail "$APP" ut_rts_badcmd.o

# Error handling for bad sizes
expectFile ut_rts_badcmd2.o
expectFail "$APP" ut_rts_badcmd2.o
expectFile ut_rts_c2small.o
expectFail "$APP" ut_rts_c2small.o
expectFile ut_rts_c2big.o
expectFail "$APP" ut_rts_c2big.o
expectFile ut_ats_badcmd.o
expectFail "$APP" ut_ats_badcmd.o

# Error handling test for out of range values
expectFile ut_rts_oorcmd.o
expectFail "$APP" ut_rts_oorcmd.o

# Error handling tests for exceeding the table size
expectFile ut_rts_2bigA.o
expectFail "$APP" ut_rts_2bigA.o
expectFile ut_ats_2big.o
expectFail "$APP" ut_ats_2big.o
expectFile ut_ats_swd.o
expectFail "$APP" ut_ats_swd.o

# Error handling for bad message types
expectFile ut_rts_cnoh2.o
expectFail "$APP" ut_rts_cnoh2.o
expectFile ut_rts_cnotc.o
expectFail "$APP" ut_rts_cnotc.o


echo "=================================="
echo "elf2cfetbl tests complete in $1."
echo
echo "$NUMTESTS tests."
NUMPASS=`expr $NUMTESTS - $NUMFAIL`
echo "$NUMPASS tests pass."
echo "$NUMFAIL tests fail."
echo "=================================="

# Return code is the number of failures, zero=all pass.
exit $NUMFAIL
