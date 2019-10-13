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

#---------------------------------------------------------------
# app.mk - Generic make rules for applications
#---------------------------------------------------------------

ifndef ETHINZA_DIR
$(error You must define ETHINZA_DIR before including this Makefile!)
endif

include $(ETHINZA_DIR)/make/common.mk

ifndef TARGET
$(error You must define TARGET in your application Makefile)
endif

ifndef SRCS
$(error You must set SRCS to your applications c/c++ files)
endif

default all:
ifneq ($(MAKE_ALSO),)
	# Function Below Defined In common.mk
	$(QUIET) $(call alsoMake)
endif
	$(QUIET) $(MAKE) $(MAKE_OPTS) $(TARGET)

include $(ETHINZA_DIR)/make/logs.mk

$(TARGET): $(BUILD_DIR) $(LOG_DEPS) $(OBJS)
	$(QUIET) $(CC) $(CFLAGS) $(BUILT_OBJS) $(LDFLAGS) -o $@
	$(QUIET) cp -f $(TARGET) $(ETHINZA_DIR)/
	@echo "---------------------------------------"
	@echo " +++ $(TARGET) +++ application created."
	@echo "---------------------------------------"

install: $(TARGET)
ifneq ($(MAKE_ALSO),)
	# Function Below Defined In common.mk
	$(QUIET) $(call alsoMake)
endif
	$(QUIET) mkdir -p $(ETHINZA_APPS_BIN)
	$(QUIET) cp -f $(TARGET) $(ETHINZA_APPS_BIN)/

clean::
ifneq ($(MAKE_ALSO),)
	# Function Below Defined In common.mk
	$(QUIET) $(call alsoMake)
endif
	$(QUIET) rm -f $(TARGET)

nightly::
ifneq ($(MAKE_ALSO),)
	# Function Below Defined In common.mk
	$(QUIET) $(call alsoMake)
endif
	@echo > /dev/null

include $(ETHINZA_DIR)/make/std.mk

