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

ifeq ($(BUILD_PARENT),)

#export BUILD_PARENT  = "\"$(shell cvs log | awk -F' ' '{print $$1}')\""
export BUILD_USER    = "\"$(shell whoami)\""
#export BUILD_CVSUSER = "\"$(shell cvs config user.name)\""
export BUILD_HOST    = "\"$(shell hostname)\""
export BUILD_DATE    = "\"$(shell date +%y.%m.%d.%H%M)\""
export BUILD_MACH    = "\"$(shell uname -m)\""
export BUILD_KERNEL  = "\"$(shell uname -r)\""

endif

#CFLAGS += -DBUILD_PARENT=$(BUILD_PARENT)
CFLAGS += -DBUILD_USER=$(BUILD_USER)
#CFLAGS += -DBUILD_CVSUSER=$(BUILD_CVSUSER)
CFLAGS += -DBUILD_HOST=$(BUILD_HOST)
CFLAGS += -DBUILD_DATE=$(BUILD_DATE)
CFLAGS += -DBUILD_MACH=$(BUILD_MACH)
CFLAGS += -DBUILD_KERNEL=$(BUILD_KERNEL)

