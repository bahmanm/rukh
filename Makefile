.DEFAULT_GOAL := rukh.build

export ROOT := $(dir $(abspath $(lastword $(MAKEFILE_LIST))))

include $(ROOT)etc/make/common.mk
include $(ROOT)support/Makefile
include $(ROOT)engine/Makefile

rukh.configure : rukh.support.configure rukh.engine.configure ## Configure all Rukh components
.PHONY : rukh.configure

rukh.build : rukh.support.build rukh.engine.build ## Build all Rukh components
.PHONY : rukh.build

rukh.test : rukh.support.test rukh.engine.test ## Run Rukh smoke tests
.PHONY : rukh.test

rukh.install : rukh.support.install rukh.engine.install ## Install all Rukh components
.PHONY : rukh.install

rukh.clean : rukh.support.clean rukh.engine.clean ## Remove Rukh build artefacts
.PHONY : rukh.clean

rukh.format : rukh.support.format rukh.engine.format ## Format all Rukh C and C++ sources
.PHONY : rukh.format
