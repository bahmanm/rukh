.DEFAULT_GOAL := rukh.build

export ROOT := $(dir $(abspath $(lastword $(MAKEFILE_LIST))))

include $(ROOT)etc/make/common.mk
include $(ROOT)src/Makefile

rukh.configure : rukh.engine.configure ## Configure all Rukh components
.PHONY : rukh.configure

rukh.build : rukh.engine.build ## Build all Rukh components
.PHONY : rukh.build

rukh.test : rukh.engine.test ## Run Rukh smoke tests
.PHONY : rukh.test

rukh.install : rukh.engine.install ## Install all Rukh components
.PHONY : rukh.install

rukh.clean : rukh.engine.clean ## Remove Rukh build artefacts
.PHONY : rukh.clean
