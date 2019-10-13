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
# tools.mk - Generic make rules for tools
#---------------------------------------------------------------

ifndef ETHINZA_DIR
$(error You must define ETHINZA_DIR before including this Makefile!)
endif

include $(ETHINZA_DIR)/make/common.mk

ifndef TARGET
$(error You must define TARGET in your tool Makefile)
endif

ifndef SRCS
$(error You must set SRCS to your tool c/c++ files)
endif

default all:
	$(QUIET) $(MAKE) $(MAKE_OPTS) install

include $(ETHINZA_DIR)/make/logs.mk

$(TARGET): $(BUILD_DIR) $(LOG_DEPS) $(OBJS)
	$(QUIET) $(CC) $(CFLAGS) $(BUILT_OBJS) $(LDFLAGS) -o $@
	@echo " +- $(TARGET) -+ tool created."

install: $(TARGET)
ifneq ($(MAKE_ALSO),)
	# Function Below Defined In common.mk
	$(QUIET) $(call alsoMake)
endif
	$(QUIET) mkdir -p $(ETHINZA_TOOLS_BIN)
	$(QUIET) cp -f $(TARGET) $(ETHINZA_TOOLS_BIN)/

clean::
ifneq ($(MAKE_ALSO),)
	# Function Below Defined In common.mk
	$(QUIET) $(call alsoMake)
endif
	$(QUIET) rm -f $(TARGET)
	$(QUIET) rm -f $(SCINTILLA_TOOL_BIN)/$(TARGET)

nightly::
ifneq ($(MAKE_ALSO),)
	# Function Below Defined In common.mk
	$(QUIET) $(call alsoMake)
endif
	@echo > /dev/null

include $(ETHINZA_DIR)/make/std.mk

