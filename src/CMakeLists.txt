cmake_minimum_required(VERSION 3.28)

if(CMAKE_HOST_SYSTEM_NAME STREQUAL "Darwin")
    set(CMAKE_OSX_DEPLOYMENT_TARGET "27.0" CACHE STRING "Minimum supported macOS version" FORCE)
    set(CMAKE_OSX_ARCHITECTURES "arm64" CACHE STRING "macOS target architecture" FORCE)
endif()

project(rukh LANGUAGES CXX)
include("${CMAKE_CURRENT_SOURCE_DIR}/../etc/cmake/platform-check.cmake")

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

find_package(Threads REQUIRED)
find_package(CURL REQUIRED)

set(RUKH_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}")

set(RUKH_SOURCES
    benchmark.cpp
    bitboard.cpp
    book/book.cpp
    book/book_manager.cpp
    book/ctg/ctg.cpp
    book/file_mapping.cpp
    book/polyglot/polyglot.cpp
    engine.cpp
    evaluate.cpp
    learn/learn.cpp
    livebook/BaseLivebook.cpp
    livebook/ChessDBContributor.cpp
    livebook/ChessDb.cpp
    livebook/LichessEndgame.cpp
    livebook/LichessGames.cpp
    livebook/LichessLivebook.cpp
    livebook/LichessMaster.cpp
    livebook/LichessOpening.cpp
    livebook/LichessPlayer.cpp
    livebook/LichessUsers.cpp
    livebook/Proxy.cpp
    livebook/analysis/Analysis.cpp
    livebook/analysis/Cp.cpp
    livebook/analysis/Mate.cpp
    livebook/analysis/Wdl.cpp
    main.cpp
    mcts/montecarlo.cpp
    memory.cpp
    misc.cpp
    movegen.cpp
    movepick.cpp
    nnue/features/full_threats.cpp
    nnue/features/half_ka_v2_hm.cpp
    nnue/network.cpp
    nnue/nnue_accumulator.cpp
    nnue/nnue_misc.cpp
    position.cpp
    score.cpp
    search.cpp
    shashin/moveconfig.cpp
    shashin/shashin_manager.cpp
    syzygy/tbprobe.cpp
    thread.cpp
    timeman.cpp
    tt.cpp
    tune.cpp
    uci.cpp
    ucioption.cpp
    wdl/win_probability.cpp
)

add_executable(rukh ${RUKH_SOURCES})
target_include_directories(rukh PRIVATE "${RUKH_SOURCE_DIR}")
target_compile_definitions(rukh PRIVATE USE_LIVEBOOK IS_64BIT)
target_compile_options(rukh PRIVATE
    $<$<CXX_COMPILER_ID:Clang>:-Wall;-Wextra;-Wpedantic;-Wcast-qual;-fexceptions>
    $<$<CONFIG:Release>:-O3;-funroll-loops>
)
target_link_libraries(rukh PRIVATE CURL::libcurl Threads::Threads)
include("${CMAKE_CURRENT_SOURCE_DIR}/../etc/cmake/download-network.cmake")

if(WIN32)
    target_link_libraries(rukh PRIVATE bcrypt advapi32 crypt32 gdi32 ws2_32)
endif()

install(TARGETS rukh RUNTIME DESTINATION bin)
