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
# docs.mk - Generate documentation from doxygen comments
#---------------------------------------------------------------

ifndef ETHINZA_DIR
$(error You must define ETHINZA_DIR before including this Makefile!)
endif

include $(ETHINZA_DIR)/make/common.mk

DOCS_CONFIG = Doxyfile

.PHONY: docs html

default all docs: $(DOCS_CONFIG)
	@( which doxygen && doxygen $(DOCS_CONFIG) || exit 0)

install:

clean::
	rm -rf html/

viewdocs:
	konqueror html/index.html

