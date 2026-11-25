ifndef __rukh_chess_core_Makefile
__rukh_chess_core_Makefile := 1

rukh.chess-core.configure : rukh.support.install ## Configure chess_core
	$(RUKH_CMAKE) -S "$(ROOT)chess_core" -B "$(RUKH_CHESS_CORE_BUILD_DIR)" \
		$(if $(RUKH_CMAKE_GENERATOR),-G "$(RUKH_CMAKE_GENERATOR)") \
		-DCMAKE_BUILD_TYPE=$(RUKH_CMAKE_BUILD_TYPE) \
		-DCMAKE_PREFIX_PATH="$(RUKH_SUPPORT_INSTALL_DIR)" $(RUKH_CMAKE_CONFIGURE_ARGS)
.PHONY : rukh.chess-core.configure

rukh.chess-core.build : rukh.chess-core.configure ## Build chess_core
	$(RUKH_CMAKE) --build "$(RUKH_CHESS_CORE_BUILD_DIR)" --config $(RUKH_CMAKE_BUILD_TYPE) --parallel $(RUKH_CMAKE_BUILD_ARGS)
.PHONY : rukh.chess-core.build

rukh.chess-core.test : rukh.chess-core.build ## Run chess_core tests
	$(RUKH_CTEST) --test-dir "$(RUKH_CHESS_CORE_BUILD_DIR)" -C $(RUKH_CMAKE_BUILD_TYPE) --output-on-failure
.PHONY : rukh.chess-core.test

rukh.chess-core.install : rukh.chess-core.build ## Install chess_core
	$(RUKH_CMAKE) --install "$(RUKH_CHESS_CORE_BUILD_DIR)" --config $(RUKH_CMAKE_BUILD_TYPE) --prefix "$(RUKH_CHESS_CORE_INSTALL_DIR)"
.PHONY : rukh.chess-core.install

rukh.chess-core.clean : ## Remove chess_core build and install artefacts
	rm -rf "$(RUKH_CHESS_CORE_BUILD_DIR)" "$(RUKH_CHESS_CORE_INSTALL_DIR)"
.PHONY : rukh.chess-core.clean

rukh.chess-core.format : ## Format chess_core C and C++ sources
	cd "$(ROOT)" && git ls-files -z --cached --others --exclude-standard -- \
		':(glob)chess_core/**/*.cc' ':(glob)chess_core/**/*.cpp' ':(glob)chess_core/**/*.h' ':(glob)chess_core/**/*.hpp' \
	| xargs -0 -r -n 1 sh -c 'test ! -f "$$1" || exec "$(RUKH_CLANG_FORMAT)" -i "$$1"' sh
.PHONY : rukh.chess-core.format

endif
