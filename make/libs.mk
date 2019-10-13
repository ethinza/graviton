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
# libs.mk - Generic make rules for libraries
#---------------------------------------------------------------

ifndef ETHINZA_DIR
$(error You must define ETHINZA_DIR before including this Makefile!)
endif

ifndef TARGET
$(error You must define TARGET in your library Makefile)
endif

ifndef SRCS
$(error You must set SRCS to your librarys C files)
endif

# Avoid repetitious declrations in the library's Makefile
TARGET_LIBS += $(TARGET)

include $(ETHINZA_DIR)/make/common.mk

# Allow users to override where the library gets installed to.
ifndef LIB_BIN_DIR
LIB_BIN_DIR=$(ETHINZA_LIBS_BIN)
endif

LIB_NAME = lib$(TARGET).a

.PHONY: default all install reinstall
default all: install

include $(ETHINZA_DIR)/make/common.mk

ifeq ($(BUILD_DIR),)
BUILT_LIB  = $(LIB_NAME)
else
BUILT_LIB  = $(BUILD_DIR)/$(LIB_NAME)
endif

$(BUILT_LIB):: $(BUILD_DIR) $(LOG_DEPS) $(OBJS)
	$(QUIET) $(AR) crs $@ $(BUILT_OBJS)
	@echo "---------------------------------------"
	@echo " :: $(TARGET) :: library created."
	@echo "---------------------------------------"

install: $(BUILT_LIB)
	$(QUIET) mkdir -p $(LIB_BIN_DIR)
	$(QUIET) cp -f $(BUILT_LIB) $(LIB_BIN_DIR)

uninstall:
	$(QUIET) rm -f $(LIB_BIN_DIR)/$(LIB_NAME)

reinstall: clean install

include $(ETHINZA_DIR)/make/std.mk

