#####################################################################################
##
##        Copyright (c) 2016, Odyssey Space Research, LLC.
##
##        Software developed under contract NNJ14HA64B, subcontract
##        NNJ14HA64B-ODY1.
##
##        All rights reserved.  Odyssey Space Research grants
##        to the Government, and other acting on its behalf, a paid-up,
##        nonexclusive, irrevocable, worldwide license in such copyrighted
##        computer software to reproduce, prepare derivative works, and perform
##        publicly and display publicly (but not to distribute copies to the
##        public) by or on behalf of the Government.
##
##        Created by Allen Brown, abrown@odysseysr.com
##
#####################################################################################

##############################################################################
## compiler-opts.mak - compiler definitions and options for building the cFE 
##
## Target: Linux on 32-bit (little-endian) ARM
##
## Modifications:
## Similar to pc-linux, except compiler tools set for cross-compiler prefix and
## x86-specific defines and options were removed and replaced with arm.
##
###############################################################################
## 
## Warning Level Configuration
##
# WARNINGS=-Wall -ansi -pedantic -Wstrict-prototypes
WARNINGS=-Wall -Wstrict-prototypes

## 
## Host OS Include Paths ( be sure to put the -I switch in front of each directory )
##
SYSINCS=

##
## Target Defines for the OS, Hardware Arch, etc..
##
TARGET_DEFS+=-D__ARM__ -D_arm_ -D_LINUX_OS_ -D$(OS) -DBUILD=$(BUILD) -D_REENTRANT -D _EMBED_  

## 
## Endian Defines
##
ENDIAN_DEFS=-D_EL -DENDIAN=_EL -DSOFTWARE_LITTLE_BIT_ORDER 

##
## Compiler Architecture Switches
## 
ARCH_OPTS = 

##
## Application specific compiler switches 
##
ifeq ($(BUILD_TYPE),CFE_APP)
   APP_COPTS= 
   APP_ASOPTS=
else
   APP_COPTS=
   APP_ASOPTS=
endif

##
## Extra Cflags for Assembly listings, etc.
##
LIST_OPTS = 

##
## gcc options for dependancy generation
## 
COPTS_D = $(APP_COPTS) $(ENDIAN_DEFS) $(TARGET_DEFS) $(ARCH_OPTS) $(SYSINCS) $(WARNINGS)

## 
## General gcc options that apply to compiling and dependency generation.
##
COPTS=$(LIST_OPTS) $(COPTS_D)

##
## Extra defines and switches for assembly code
##
ASOPTS = $(APP_ASOPTS) -P -xassembler-with-cpp 

##---------------------------------------------------------
## Application file extention type
## This is the defined application extention.
## Known extentions: Mac OS X: .bundle, Linux: .so, RTEMS:
##   .s3r, vxWorks: .o etc.. 
##---------------------------------------------------------
APP_EXT = so

#################################################################################
## Host Development System and Toolchain defintions
##

##
## Host OS utils
##
RM=rm -f
CP=cp

##
## Compiler tools
##
COMPILER=$(CC)gcc
ASSEMBLER=$(CC)as
LINKER=$(CC)ld
AR=$(CC)ar
NM=$(CC)nm
SIZE=$(CC)size
OBJCOPY=$(CC)objcopy
OBJDUMP=$(CC)objdump
TABLE_BIN = elf2cfetbl
