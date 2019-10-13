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

#-----------------------------------------------
# std.mk - Standard make targets for all builds
#-----------------------------------------------

ifndef ETHINZA_DIR
$(error You must define ETHINZA_DIR before including this Makefile!)
endif

#------------------------------------------
# Generic make targets
#------------------------------------------

.PHONY: $(BUILD_DIR)
$(BUILD_DIR):
	$(QUIET) mkdir -p $(BUILD_DIR)

distclean:
	$(QUIET) rm -f *~ .*.*.swp
	$(QUIET) $(MAKE) $(MAKE_OPTS) clean

clean::
ifneq ($(BUILD_DIR),)
	$(QUIET) rm -rf $(BUILD_DIR)
else
	$(QUIET) rm -f *.o
endif

nightly::
	@echo > /dev/null

.c.o: $(BUILD_DIR)
	$(QUIET) gcc -c $(CFLAGS) $< -o $(BUILD_DIR)/$(notdir $@)

.cpp.o: $(BUILD_DIR)
	$(QUIET) g++ -c $(CFLAGS) $< -o $(BUILD_DIR)/$(notdir $@)


