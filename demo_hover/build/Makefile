###############################################################################
# File: makefile
#
# Purpose:
#   Main makefile for Mission build and all apps
#
# $Id: Makefile 1.5 2014/06/27 15:44:43GMT-05:00 rmcgraw Exp  $
#
# $Log: Makefile  $
# Revision 1.5 2014/06/27 15:44:43GMT-05:00 rmcgraw 
# DCR22713:1 Removed extra tabs and inserted history log
#
###############################################################################
##
## Top Level rules:
## If you want to build the testapps every time, just add the testapps targets
## to the primary targets below ( or uncomment the testapps versions )
##
export MISSIONNAME:=Mission

##############################################################################
## Build rules

# Builds each platform.  Assumes make config has already been run.
all ::
	$(MAKE) -C linux all
	$(MAKE) -C ardrone all


clean ::
	$(MAKE) -C linux clean
	$(MAKE) -C ardrone clean

install ::
	$(MAKE) -C linux install
	$(MAKE) -C ardrone install

depend ::
	$(MAKE) -C linux depend
	$(MAKE) -C ardrone depend

# Added config make option to build both ardrone and linux deployments "Only for Education Outreach"
config ::
	$(MAKE) -C linux config
	$(MAKE) -C ardrone config

#-------------------------------------------------------------------------------
# Doxygen rules


usersguide ::
	$(MAKE) -C linux  usersguide
	$(MAKE) -C ardrone  usersguide

detaileddocs ::
	$(MAKE) -C linux  detaileddocs
	$(MAKE) -C ardrone  detaileddocs

installdocs ::
	$(MAKE) -C linux  installdocs
	$(MAKE) -C ardrone  installdocs

cleandocs ::
	$(MAKE) -C linux  cleandocs
	$(MAKE) -C ardrone  cleandocs

