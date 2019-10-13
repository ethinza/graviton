#//////////////////////////////////////////////////////////////////////////////
#/
#/ Copyright (c) 2018-2019 Ethinza Inc, All rights reserved
#/
#/ This file contains TRADE SECRETs of ETHINZA INC and is CONFIDENTIAL.
#/ Without written consent from ETHINZA INC, it is prohibited to disclose or
#/ reproduce its contents, or to manufacture, use or sell it in whole or part.
#/ Any reproduction of this file without written consent of ETHINZA INC is a 
#/ violation of the copyright laws and is subject to civil liability and 
#/ criminal prosecution.
#/
#//////////////////////////////////////////////////////////////////////////////

# If Setenv Not Sourced
ifndef ETHINZA_DIR
export ETHINZA_DIR = $(PWD)
endif

export VERBOSE := 1

# Find Build Machine Type
export MTYPE := $(shell uname -m)

# Determine UserID
export QUI := $(shell whoami)

# Use Date As Release Version For Now
export REL := $(shell date +%y.%m.%d.%H%M)

SUBDIRS =	\
		libs \
		apps \
		#xparty \
		#mods \
		#firmware \
		#tools \
		#tests \
		# Add others here

include $(ETHINZA_DIR)/make/subdirs.mk

# Add some custom rules for the top level

clean::
	$(QUIET) rm -rf $(ETHINZA_LIBS_BIN)

.PHONY: tags libs apps clobber cvsclean rpms
tags:
	@ctags -R 

libs:
	$(QUIET) $(MAKE) $(MAKE_OPTS) -C libs/ all
	$(QUIET) $(MAKE) $(MAKE_OPTS) -C xparty/ all

clobber:
	$(QUIET) $(MAKE) clean 

rpms:
	$(ETHINZA_DIR)/make/rpm.sh 

# CAUTION - This Step Will REALLY Clean Your Source - Use With Care!
# "CVS Clean" Command Removes Files Not Under Source Control!

cvsclean:
	$(MAKE) clean ; \
	$(ETHINZA_DIR)/make/cvs-clean.sh 
