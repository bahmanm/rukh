ifndef __rukh_etc_make_common.mk
__rukh_etc_make_common.mk := 1

SHELL := bash
.SHELLFLAGS := -euo pipefail -c

RUKH_BUILD_ROOT ?= $(ROOT)_build/

RUKH_BMAKELIB_VERSION ?= v0.9.1
RUKH_BMAKELIB_DIR ?= $(RUKH_BUILD_ROOT).bmakelib
RUKH_BMAKELIB ?= $(RUKH_BMAKELIB_DIR)/bmakelib.mk

bmakelib.conf.help.variables ?= no
bmakelib.conf.help.scope ?= local

-include $(RUKH_BMAKELIB)

$(RUKH_BMAKELIB) :
	@mkdir -p "$(@D)"
	@curl -fsSL "https://github.com/bahmanm/bmakelib/releases/download/$(RUKH_BMAKELIB_VERSION)/bmakelib-portable.tar.gz" \
		| tar -xz -C "$(@D)" --strip-components=3

export RUKH_BMAKELIB

$(call bmakelib.enum.define,RUKH_PROFILE/debug,release)
ifeq ($(filter rukh.install rukh.engine.install,$(MAKECMDGOALS)),)
RUKH_PROFILE ?= debug
else
RUKH_PROFILE ?= release
endif
$(call bmakelib.enum.error-unless-member,RUKH_PROFILE,RUKH_PROFILE)

RUKH_ENGINE_BUILD_DIR ?= $(RUKH_BUILD_ROOT)engine/$(RUKH_PROFILE)/
RUKH_ENGINE_INSTALL_DIR ?= $(RUKH_BUILD_ROOT)install/engine/$(RUKH_PROFILE)/

RUKH_CMAKE ?= cmake
RUKH_CMAKE_BUILD_ARGS ?=
RUKH_CMAKE_CONFIGURE_ARGS ?=
RUKH_CMAKE_GENERATOR ?=
RUKH_CMAKE_BUILD_TYPE ?= $(if $(filter release,$(RUKH_PROFILE)),Release,Debug)

endif
