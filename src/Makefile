ifndef __rukh_src_Makefile
__rukh_src_Makefile := 1

rukh.engine.configure : ## Configure the engine CMake project
	$(RUKH_CMAKE) \
		-S "$(ROOT)src" \
		-B "$(RUKH_ENGINE_BUILD_DIR)" \
		$(if $(RUKH_CMAKE_GENERATOR),-G "$(RUKH_CMAKE_GENERATOR)") \
		-DCMAKE_BUILD_TYPE=$(RUKH_CMAKE_BUILD_TYPE) \
		$(RUKH_CMAKE_CONFIGURE_ARGS)
.PHONY : rukh.engine.configure

rukh.engine.build : rukh.engine.configure ## Build the engine
	$(RUKH_CMAKE) \
		--build "$(RUKH_ENGINE_BUILD_DIR)" \
		--config $(RUKH_CMAKE_BUILD_TYPE) \
		--parallel \
		$(RUKH_CMAKE_BUILD_ARGS)
.PHONY : rukh.engine.build

rukh.engine.install : rukh.engine.build ## Install the engine under _build/install
	$(RUKH_CMAKE) \
		--install "$(RUKH_ENGINE_BUILD_DIR)" \
		--config $(RUKH_CMAKE_BUILD_TYPE) \
		--prefix "$(RUKH_ENGINE_INSTALL_DIR)"
.PHONY : rukh.engine.install

rukh.engine.clean : ## Remove engine build and install artefacts
	rm -rf "$(RUKH_ENGINE_BUILD_DIR)" "$(RUKH_ENGINE_INSTALL_DIR)"
.PHONY : rukh.engine.clean

endif
