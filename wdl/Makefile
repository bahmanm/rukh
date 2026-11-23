ifndef __rukh_wdl_Makefile
__rukh_wdl_Makefile := 1

rukh.wdl.configure : ## Configure the WDL project
	$(RUKH_CMAKE) \
		-S "$(ROOT)wdl" \
		-B "$(RUKH_WDL_BUILD_DIR)" \
		$(if $(RUKH_CMAKE_GENERATOR),-G "$(RUKH_CMAKE_GENERATOR)") \
		-DCMAKE_BUILD_TYPE=$(RUKH_CMAKE_BUILD_TYPE) \
		$(RUKH_CMAKE_CONFIGURE_ARGS)
.PHONY : rukh.wdl.configure

rukh.wdl.build : rukh.wdl.configure ## Build WDL
	$(RUKH_CMAKE) \
		--build "$(RUKH_WDL_BUILD_DIR)" \
		--config $(RUKH_CMAKE_BUILD_TYPE) \
		--parallel \
		$(RUKH_CMAKE_BUILD_ARGS)
.PHONY : rukh.wdl.build

rukh.wdl.test : rukh.wdl.build ## Run WDL tests
	$(RUKH_CTEST) \
		--test-dir "$(RUKH_WDL_BUILD_DIR)" \
		-C $(RUKH_CMAKE_BUILD_TYPE) \
		--output-on-failure
.PHONY : rukh.wdl.test

rukh.wdl.install : rukh.wdl.build ## Install WDL under _build/install
	$(RUKH_CMAKE) \
		--install "$(RUKH_WDL_BUILD_DIR)" \
		--config $(RUKH_CMAKE_BUILD_TYPE) \
		--prefix "$(RUKH_WDL_INSTALL_DIR)"
.PHONY : rukh.wdl.install

rukh.wdl.clean : ## Remove WDL build and install artefacts
	rm -rf "$(RUKH_WDL_BUILD_DIR)" "$(RUKH_WDL_INSTALL_DIR)"
.PHONY : rukh.wdl.clean

rukh.wdl.format : ## Format WDL C++ sources
	cd "$(ROOT)" && git ls-files -z --cached --others --exclude-standard -- \
		':(glob)wdl/**/*.cc' ':(glob)wdl/**/*.cpp' ':(glob)wdl/**/*.h' ':(glob)wdl/**/*.hpp' \
	| xargs -0 -r -n 1 sh -c 'test ! -f "$$1" || exec "$(RUKH_CLANG_FORMAT)" -i "$$1"' sh
.PHONY : rukh.wdl.format

endif
