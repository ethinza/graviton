#//////////////////////////////////////////////////////////////////////////////
#/                                                                           //
#/ Copyright (c) 2019 Ethinza Inc, All rights reserved                       //
#/                                                                           //
#/ This file contains TRADE SECRETs of ETHINZA INC. and is CONFIDENTIAL.     //
#/ Without written consent from ETHINZA INC., it is prohibited to disclose   //
#/ or reproduce its contents, or to manufacture, use or sell it in whole or  //
#/ part. Any reproduction of this file without written consent of VEEJANSH   //
#/ INC. is a violation of the copyright laws and is subject to civil         //
#/ liability and criminal prosecution.                                       //
#/                                                                           //
#//////////////////////////////////////////////////////////////////////////////

TARGET = graviton0
.PHONY: $(TARGET)

SRCS    =	\
			main.cpp \
			# Add more source files here.

CFLAGS		+= -g
LDFLAGS		+= -lethinza
LDFLAGS		+= -lrt -lpthread

include $(ETHINZA_DIR)/make/apps.mk

clean::
	@ rm -f $(TARGET) .objs/*.o
