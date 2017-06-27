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
# Builds out a set of Elf object files using the platform toolchain's standard
# build system with slight mods to SC's makefile & using a dummy elf2cfetbl
# app to get the build system through all of the .c source files.
#
# The goal is to produce platform-specific .o files for the elf2cfetbl
# tests.  This script must start with a workable build that includes SC.
# WARNING: Yeah, if this thing bombs it'll mess up your build.  Use source control.
#
# Invocation: gen_test_tables.sh CPUNAME
#             where the arg is your platform
#             gen_test_tables.sh linux
#             gen_test_tables.sh ut699-vxworks6
#             or, to clean out files under test:
#             gen_test_tables.sh linux clean
#
# Steps to generate the test files:
#1a) copy the unit test .c table files to apps/sc/fsw/tables location.  Rename or move your 
#   current SC files so they aren't overwritten or use source control to restore
#   them afterwards.  (OR copy SC apps to backup dir)
#1b) copy the SampleTblImg.c to apps/sc/fsw/for_build as well.
#2) copy the unit test SC table make file to SC build location.  Note, this will
#   conflict with the mission table make file so be sure to preserve your mission
#   table make file or use source control to restore them afterwards.
#3) In the SC build location, remove any existing .o and .tbl table files
#4) make the build (builds SC .o table files) from the top so 
#   it uses the full build system
#5) copy all the unit test SC .o table files (not the .tbl files) 
#   back to this unit_test directory
#6) restore the SC apps & build directory locations
#7) clean & re-make the elf2cfetbl app in the same build system, then
#   bring it over as well.

function cleanTestDir {
	if [ -d "$TEST_DIR" ]; then 
		cd "$TEST_DIR"
		rm -f *.o *.tbl *.bin elf2cfetbl *.dump
		echo "ELF2CFETBL UNIT TEST: cleaned $TEST_DIR."
	fi
	return 0
}

function checkDirBail {
	if [ ! -d "$1" ]; then 
		echo "Directory $1 not found."
		exit 1
	fi
}

function checkFileBail {
	if [ ! -f "$1" ]; then 
		echo "File $1 not found."
		exit 1
	fi
}

# EDIT THESE: Per-deployment setup
CFS_MISSION="/home/abrown4/repos/cfs_tst/CFS_TST"
CFS_APP_SRC="/home/abrown4/repos/cfs_tst/CFS_TST/apps"
CFE_TOOLS="/home/abrown4/repos/COP_CFS/tools"
BUILD_CMD=gmake
CPUNAME="$1"

# This test script's conventions:
UTDIR=`pwd`
UT_SUFFIX="_elft2blUT"

# Fairly standard build & test locations, shouldn't have to edit these.
ELF2CFETBL_SRC=$UTDIR/../elf2cfetbl.c
CFS_BUILD_DIR="$CFS_MISSION/build/$CPUNAME"
SC_BUILD_DIR="$CFS_BUILD_DIR/sc"
SC_APP_DIR="$CFS_APP_SRC/sc"

# Per-platform test area
TEST_DIR="$UTDIR/$CPUNAME"

# Arg checks
if [[ -z "$CPUNAME" ]]; then 
	echo "ELF2CFETBL UNIT TEST: Invoke with the platform CPUNAME or 'CPUNAME clean' as arguments. Aborting."
	exit 1
fi

if [ "$2" == "clean" ]; then
	cleanTestDir
	exit 0
fi

# Check for these locations/files
checkDirBail "$CFS_MISSION"
checkDirBail "$CFS_APP_SRC"
checkDirBail "$CFE_TOOLS"
checkFileBail "$ELF2CFETBL_SRC"
checkDirBail "$CFS_BUILD_DIR"
checkDirBail "$SC_BUILD_DIR"
checkDirBail "$SC_APP_DIR"

# Fix up platform test directory
if [ ! -d "$TEST_DIR" ]; then
	mkdir -p "$TEST_DIR"
else
	rm -rf "$TEST_DIR/*"
fi

echo "ELF2CFETBL UNIT TEST: Preserving original build directories..."

# copy the original build directory
rm -rf "$SC_BUILD_DIR$UT_SUFFIX"
cp -a "$SC_BUILD_DIR" "$SC_BUILD_DIR$UT_SUFFIX"

# copy the original apps directory
rm -rf "$SC_APP_DIR$UT_SUFFIX"
cp -a "$SC_APP_DIR" "$SC_APP_DIR$UT_SUFFIX"

echo "ELF2CFETBL UNIT TEST: Setting for for $CPUNAME platform build..."

# clean out the app tables & place the UT table files
cd "$UTDIR"
rm -rf "$SC_APP_DIR/fsw/tables/*.c"
cp *.c *.h "$SC_APP_DIR/fsw/tables"
cp sctables.mak "$SC_APP_DIR/fsw/for_build"

# clean out the build location to force table rebuild
rm -f "$SC_BUILD_DIR/*.tbl"
rm -f "$SC_BUILD_DIR/*.o"

# remove any platform-build elf2cfetbl and replace
# with a dummy program that always returns exit code of 0
# to keep the gfsc_build system going.  We'll use /bin/true.
if [ -f "$CFS_BUILD_DIR/elf2cfetbl/elf2cfetbl" ]; then
	mv -f "$CFS_BUILD_DIR/elf2cfetbl/elf2cfetbl" "$CFS_BUILD_DIR/elf2cfetbl/elf2cfetbl_orig"
fi
cp /bin/true "$CFS_BUILD_DIR/elf2cfetbl/elf2cfetbl"

echo "ELF2CFETBL UNIT TEST: Invoking $CPUNAME platform build for tables..."
# Invoke platform build
cd "$CFS_BUILD_DIR"
"$BUILD_CMD"

# We kind of expect the build to fail if it calls elf2cfetbl
# on some of our unit test tables.  But that should occur
# after the objects are built.

echo "ELF2CFETBL UNIT TEST: ... table build complete (failures with elf2cfetbl & .tbl files are OK)"
echo "ELF2CFETBL UNIT TEST: (Note: failures with building the SC table .o's are not OK.)"
echo "ELF2CFETBL UNIT TEST: Copying back the tables & application..."
# copy products back (just the .o's, we'll gen the tables
# as part of our unit testing)
cleanTestDir
cd "$SC_BUILD_DIR"
cp *.o "$TEST_DIR"

echo "ELF2CFETBL UNIT TEST: Cleaning up build..."
# CLEANUP
# restore the original build & app directories
rm -rf "$SC_BUILD_DIR"
mv "$SC_BUILD_DIR$UT_SUFFIX" "$SC_BUILD_DIR"
rm -rf "$SC_APP_DIR"
mv "$SC_APP_DIR$UT_SUFFIX" "$SC_APP_DIR"

# build our app for testing
echo "ELF2CFETBL UNIT TEST: Invoking $CPUNAME platform build for elf2cfetbl..."
cd "$CFS_BUILD_DIR/elf2cfetbl"
$BUILD_CMD clean

# Invoke platform build (again)
cd "$CFS_BUILD_DIR"
"$BUILD_CMD"
echo "ELF2CFETBL UNIT TEST: ... elf2cfetbl build complete"

# copy back the built application
echo "ELF2CFETBL UNIT TEST: Copying over elf2cfetbl"
cd "$CFS_BUILD_DIR/elf2cfetbl"
cp ./elf2cfetbl "$TEST_DIR"

# clean up the original application
echo "ELF2CFETBL UNIT TEST: Cleaning up elf2cfetbl"
if [ -f "$CFS_BUILD_DIR/elf2cfetbl/elf2cfetbl_orig" ]; then
	mv -f "$CFS_BUILD_DIR/elf2cfetbl/elf2cfetbl_orig" "$CFS_BUILD_DIR/elf2cfetbl/elf2cfetbl"
else
	# at least remove our test version
	rm -f "$CFS_BUILD_DIR/elf2cfetbl/elf2cfetbl"
fi

echo "ELF2CFETBL UNIT TEST: test data in: $TEST_DIR"
echo "ELF2CFETBL UNIT TEST: Complete."

