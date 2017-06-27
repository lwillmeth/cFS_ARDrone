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

###############################################################################
# File: link-rules.mak
#
# Purpose:
#   Makefile for linking code and producing the cFE Core executable image.
#
# History:
#
###############################################################################
##
## Executable target. This is target specific
##
EXE_TARGET=arm-linux.bin

CORE_INSTALL_FILES = $(EXE_TARGET)


##
## Linker flags that are needed
##
LDFLAGS = -Wl,-export-dynamic

##
## Libraries to link in
##
LIBS =  -lm -lpthread -ldl -lrt
##
## Uncomment the following line to link in C++ standard libs
## LIBS += -lstdc++
## 

##
## cFE Core Link Rule
## 
$(EXE_TARGET): $(CORE_OBJS)
	$(COMPILER) $(DEBUG_FLAGS) -o $(EXE_TARGET) $(CORE_OBJS) $(LDFLAGS) $(LIBS)
	
##
## Application Link Rule
##
$(APPTARGET).$(APP_EXT): $(OBJS)
	$(COMPILER) -shared -o $@ $(OBJS) 
