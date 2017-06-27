This unit test directory is for unit testing the elf2cfetbl for a particular 
build.  

In order to properly exercise all unit tests for a particular platform 
two things must occur:
1) The elf2cfetbl executable must be built in your mission buid/<cpux>/elf2cfetbl.
This executable is built for the host with the applicable toolchain and 
configuration.
2) The SC unit test table object files must be built in your mission build/<cpux>/sc.
These table object files are built by the target's toolchain with that (probably
different) configuration.  Note, we only want the ELF .o files.

RUNNING THE TESTS:

1) Make sure your build platform is set up and properly builds in a configuration
   that already includes SC.
2) Set your environment variables correctly for your build system before running
   these tests (the build must work from these scripts)
3) Open and edit the mission and build env vars in gen_test_tables.sh
4) gen_test_tables.sh CPUNAME
      Where CPUNAME is the build target, e.g. gen_test_tables.sh ut699-vxworks6 (or linux)
5) When the build has run (twice) run: run_tests.sh CPUNAME, e.g. run_tests.sh ut699-vxworks6

The build products and test results are run and kept separately for each CPUNAME platform.
You can clean up a CPUNAME subdir with run_tests.sh linux clean

Be sure to run these tests on both a big endian and little endian platform.

Allen Brown, Odyssey Space Research LLC
