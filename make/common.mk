#//////////////////////////////////////////////////////////////////////////////
#/                                                                           //
#/ Copyright (c) 2018 Ethinza Inc., All rights reserved                      //
#/                                                                           //
#/ This file contains TRADE SECRETs of ETHINZA INC. and is CONFIDENTIAL.     //
#/ Without written consent from ETHINZA INC., it is prohibited to disclose   //
#/ or reproduce its contents, or to manufacture, use or sell it in whole or  //
#/ part. Any reproduction of this file without written consent of VEEJANSH   //
#/ INC. is a violation of the copyright laws and is subject to civil         //
#/ liability and criminal prosecution.                                       //
#/                                                                           //
#//////////////////////////////////////////////////////////////////////////////

#------------------------------------------
# common.mk - Global defines and make targets
#------------------------------------------

ifndef ETHINZA_DIR
$(error You must define ETHINZA_DIR before including this Makefile! Check Readme)
endif

#------------------------------------------
# Define layout of the tree
#------------------------------------------

# Userspace application folders
ETHINZA_APPS		= $(ETHINZA_DIR)/apps/
ETHINZA_APPS_BIN	= $(ETHINZA_APPS)/bin/

# Userspace library folders
ETHINZA_LIBS		= $(ETHINZA_DIR)/libs/
ETHINZA_LIBS_BIN	= $(ETHINZA_LIBS)/bin/

# Kernel modules
ETHINZA_MODULES		= $(ETHINZA_DIR)/mods/

# Build tools directories
ETHINZA_TOOLS		= $(ETHINZA_DIR)/tools/
ETHINZA_TOOLS_BIN	= $(ETHINZA_TOOLS)/bin/

#---------------------------------------------------
# Determine which vendor libraries are included.
#---------------------------------------------------

VENDOR_DIR			= $(ETHINZA_DIR)/xparty/
#VENDOR_LIBS = tinyxml ticpp

#---------------------------------------------------
# Support STOP_ON_ERROR
#---------------------------------------------------
ERROR_EXIT_STATUS=127

ifeq ($(STOP_ON_ERROR),0)
# Don't stop on error
TEST_EARLY_EXIT=if [ $$? -ne 0 ]; then export CHILD_ERROR_STATUS=1; fi
TEST_FINAL_EXIT=if [ "$$CHILD_ERROR_STATUS" = "1" ]; then exit $(ERROR_EXIT_STATUS); fi
else
# Stop on first error
TEST_EARLY_EXIT=if test $$? -ne 0; then exit $(ERROR_EXIT_STATUS); fi
TEST_FINAL_EXIT=
endif

#---------------------------------------------------
# Compiler defines
#---------------------------------------------------

include $(ETHINZA_DIR)/make/ver.mk

BUILD_DIR	= .objs

CC = g++
AR = ar

CFLAGS  += -I $(ETHINZA_LIBS)
CFLAGS  += -I $(ETHINZA_MODULES)
CFLAGS  += $(foreach lib, $(VENDOR_LIBS), -I $(VENDOR_DIR)/$(lib)/)
CFLAGS  += -DETHINZA -D_THREAD_SAFE
#CFLAGS  += -g -Wall
CFLAGS  += -g -std=c++11 -Wall -pedantic
CFLAGS  += -D_GNU_SOURCE   # Get GNU extensions
#CFLAGS  += -fpermissive

LDFLAGS += -L $(ETHINZA_LIBS_BIN)
LDFLAGS += $(foreach lib, $(TARGET_LIBS), -l $(lib))

ifdef ETHINZA_PROFILE_ENABLE
CFLAGS += -p
endif

ifndef QUIET
QUIET = @
endif
ifeq ($(VERBOSE),1)
QUIET =
endif

ifeq ($(QUIET),@)
MAKE_OPTS = --no-print-directory
endif

#--------------------------------------------------
# Generate a set of objects
#--------------------------------------------------

ifneq ($(SRCS),)

# Need to do this in 2 steps:
CPP_OBJS	= $(SRCS:.cpp=.o)
OBJS		= $(CPP_OBJS:.c=.o)

ifeq ($(BUILD_DIR),)
BUILT_OBJS = $(OBJS)
else
BUILT_OBJS = $(foreach obj,$(OBJS), $(BUILD_DIR)/$(notdir $(obj)))
endif

endif

define alsoMake
		@for dir in $(MAKE_ALSO); do \
			if [ -r $$dir/Makefile ]; then \
				$(MAKE) $(MAKE_OPTS) -C $$dir $@ ; \
				$(TEST_EARLY_EXIT) \
			fi ; \
		done ; \
		$(TEST_FINAL_EXIT)
endef
