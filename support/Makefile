ifndef __rukh_support_Makefile
__rukh_support_Makefile := 1

rukh.support.configure : ## Configure the support CMake project
	$(RUKH_CMAKE) \
		-S "$(ROOT)support" \
		-B "$(RUKH_SUPPORT_BUILD_DIR)" \
		$(if $(RUKH_CMAKE_GENERATOR),-G "$(RUKH_CMAKE_GENERATOR)") \
		-DCMAKE_BUILD_TYPE=$(RUKH_CMAKE_BUILD_TYPE) \
		$(RUKH_CMAKE_CONFIGURE_ARGS)
.PHONY : rukh.support.configure

rukh.support.build : rukh.support.configure ## Build support
	$(RUKH_CMAKE) \
		--build "$(RUKH_SUPPORT_BUILD_DIR)" \
		--config $(RUKH_CMAKE_BUILD_TYPE) \
		--parallel \
		$(RUKH_CMAKE_BUILD_ARGS)
.PHONY : rukh.support.build

rukh.support.test : rukh.support.build ## Run support tests
	$(RUKH_CTEST) \
		--test-dir "$(RUKH_SUPPORT_BUILD_DIR)" \
		-C $(RUKH_CMAKE_BUILD_TYPE) \
		--output-on-failure
.PHONY : rukh.support.test

rukh.support.install : rukh.support.build ## Install support under _build/install
	$(RUKH_CMAKE) \
		--install "$(RUKH_SUPPORT_BUILD_DIR)" \
		--config $(RUKH_CMAKE_BUILD_TYPE) \
		--prefix "$(RUKH_SUPPORT_INSTALL_DIR)"
.PHONY : rukh.support.install

rukh.support.clean : ## Remove support build and install artefacts
	rm -rf "$(RUKH_SUPPORT_BUILD_DIR)" "$(RUKH_SUPPORT_INSTALL_DIR)"
.PHONY : rukh.support.clean

rukh.support.format : ## Format support C and C++ sources
	cd "$(ROOT)" && git ls-files -z --cached --others --exclude-standard -- \
		':(glob)support/**/*.c' ':(glob)support/**/*.cc' ':(glob)support/**/*.cpp' ':(glob)support/**/*.cxx' \
		':(glob)support/**/*.h' ':(glob)support/**/*.hh' ':(glob)support/**/*.hpp' ':(glob)support/**/*.hxx' \
	| xargs -0 "$(RUKH_CLANG_FORMAT)" -i
.PHONY : rukh.support.format

endif
