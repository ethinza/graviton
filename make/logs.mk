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
# logs.mk - Rules for dealing with the logging system.
#---------------------------------------------------------------

ifndef ETHINZA_DIR
$(error ETHINZA_DIR not defined!)
endif

BUILD_MSG = perl $(ETHINZA_DIR)/make/msgs.pl

# Only include logging definitions if user has identified 1 or more .msg files
ifdef LOG_MSGS

LOG_HDRS = $(LOG_MSGS:.msg=_Messages.h)
LOG_DEPS = $(LOG_MSGS) $(LOG_HDRS)

.PHONY: $(LOG_MSGS)
$(LOG_MSGS):
	( pushd $(dir $@); \
		$(BUILD_MSG) $(notdir $@) || exit 1; \
		popd; )

clean::
	rm -f $(LOG_HDRS)
endif

