cmake_minimum_required(VERSION 3.28)

project(rukh-chess-core VERSION 1.0.0 LANGUAGES CXX)
include("${CMAKE_CURRENT_SOURCE_DIR}/../etc/cmake/platform-check.cmake")
include(CTest)
include(GNUInstallDirs)
include(CMakePackageConfigHelpers)
find_package(rukh_support CONFIG REQUIRED)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_library(rukh_chess_core src/bitboard.cpp)
add_library(RukhChessCore::RukhChessCore ALIAS rukh_chess_core)
set_target_properties(rukh_chess_core PROPERTIES EXPORT_NAME RukhChessCore)
target_include_directories(rukh_chess_core PUBLIC
    "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>"
    "$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>")
target_include_directories(rukh_chess_core PRIVATE
    "$<TARGET_PROPERTY:RukhSupport::RukhSupport,INTERFACE_INCLUDE_DIRECTORIES>")
target_compile_features(rukh_chess_core PUBLIC cxx_std_23)
target_compile_definitions(rukh_chess_core PUBLIC IS_64BIT)
target_compile_options(rukh_chess_core PRIVATE
    $<$<CXX_COMPILER_ID:Clang>:-Wall;-Wextra;-Wpedantic;-Wcast-qual>)

if(BUILD_TESTING)
    add_executable(rukh_chess_core_tests tests/chess_core_tests.cpp)
    target_link_libraries(rukh_chess_core_tests PRIVATE RukhChessCore::RukhChessCore)
    add_test(NAME rukh-chess-core COMMAND rukh_chess_core_tests)
endif()

install(TARGETS rukh_chess_core EXPORT RukhChessCoreTargets
    ARCHIVE DESTINATION "${CMAKE_INSTALL_LIBDIR}"
    LIBRARY DESTINATION "${CMAKE_INSTALL_LIBDIR}"
    RUNTIME DESTINATION "${CMAKE_INSTALL_BINDIR}"
    INCLUDES DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}")
install(DIRECTORY include/ DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}")
install(EXPORT RukhChessCoreTargets NAMESPACE RukhChessCore::
    FILE RukhChessCoreTargets.cmake
    DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/rukh_chess_core")

configure_package_config_file(cmake/RukhChessCoreConfig.cmake.in
    "${CMAKE_CURRENT_BINARY_DIR}/rukh_chess_coreConfig.cmake"
    INSTALL_DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/rukh_chess_core")
write_basic_package_version_file("${CMAKE_CURRENT_BINARY_DIR}/rukh_chess_coreConfigVersion.cmake"
    VERSION "${PROJECT_VERSION}" COMPATIBILITY SameMajorVersion)
install(FILES
    "${CMAKE_CURRENT_BINARY_DIR}/rukh_chess_coreConfig.cmake"
    "${CMAKE_CURRENT_BINARY_DIR}/rukh_chess_coreConfigVersion.cmake"
    DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/rukh_chess_core")
