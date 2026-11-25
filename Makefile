.DEFAULT_GOAL := rukh.build

export ROOT := $(dir $(abspath $(lastword $(MAKEFILE_LIST))))

include $(ROOT)etc/make/common.mk
include $(ROOT)support/Makefile
include $(ROOT)wdl/Makefile
include $(ROOT)chess_core/Makefile
include $(ROOT)engine/Makefile

rukh.configure : rukh.support.configure rukh.wdl.configure rukh.chess-core.configure rukh.engine.configure ## Configure all Rukh components
.PHONY : rukh.configure

rukh.build : rukh.support.build rukh.wdl.build rukh.chess-core.build rukh.engine.build ## Build all Rukh components
.PHONY : rukh.build

rukh.test : rukh.support.test rukh.wdl.test rukh.chess-core.test rukh.engine.test ## Run Rukh tests
.PHONY : rukh.test

rukh.install : rukh.support.install rukh.wdl.install rukh.chess-core.install rukh.engine.install ## Install all Rukh components
.PHONY : rukh.install

rukh.clean : rukh.support.clean rukh.wdl.clean rukh.chess-core.clean rukh.engine.clean ## Remove Rukh build artefacts
.PHONY : rukh.clean

rukh.format : rukh.support.format rukh.wdl.format rukh.chess-core.format rukh.engine.format ## Format all Rukh C and C++ sources
.PHONY : rukh.format
