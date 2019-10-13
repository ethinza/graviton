#//////////////////////////////////////////////////////////////////////////////
#/                                                                           //
#/ Copyright (c) 2018 Ethinza Inc., All rights reserved                      //
#/                                                                           //
#/ This file contains TRADE SECRETs of ETHINZA INC. and is CONFIDENTIAL.     //
#/ Without written consent from ETHINZA INC., it is prohibited to disclose   //
#/ or reproduce its contents, or to manufacture, use or sell it in whole or  //
#/ part. Any reproduction of this file without written consent of ETHINZA    //
#/ INC. is a violation of the copyright laws and is subject to civil         //
#/ liability and criminal prosecution.                                       //
#/                                                                           //
#//////////////////////////////////////////////////////////////////////////////

#--------------------------------------------------------------------------
# subdirs.mk - Simply pass the make target to all subdirs.
#--------------------------------------------------------------------------

ifndef ETHINZA_DIR
$(error You must define ETHINZA_DIR before including this Makefile!)
endif

include $(ETHINZA_DIR)/make/common.mk

ifeq ($(SUBDIRS),)
SUBDIRS := $(shell find . -mindepth 1 -maxdepth 1 -type d)
endif

# JFS: Do it this way, because for some reason this:
#   default: all
# would cause each subfolder to get built 2x.
default:
	$(QUIET) $(MAKE) $(MAKE_OPTS) all

clean::
	$(QUIET) $(call stdMake)

nightly::
	$(QUIET) $(call stdMake)

%:
	$(QUIET) $(call stdMake)

define stdMake
	@for dir in $(SUBDIRS); do \
		if [ -r $$dir/Makefile ]; then \
			$(MAKE) $(MAKE_OPTS) -C $$dir $@ ; \
			$(TEST_EARLY_EXIT) \
		fi ; \
	done ; \
	$(TEST_FINAL_EXIT)
endef

