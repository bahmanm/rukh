cmake_minimum_required( VERSION 3.28 )

if( CMAKE_HOST_SYSTEM_NAME STREQUAL "Darwin" )
    set( CMAKE_OSX_DEPLOYMENT_TARGET "27.0" CACHE STRING "Minimum supported macOS version" FORCE )
    set( CMAKE_OSX_ARCHITECTURES "arm64" CACHE STRING "Minimum supported macOS architecture" FORCE )
endif()

project(
    rukh-wdl
    VERSION "1.0.0"
    LANGUAGES CXX )

include( "${CMAKE_CURRENT_SOURCE_DIR}/../etc/cmake/platform-check.cmake" )
include( CTest )
include( GNUInstallDirs )
include( CMakePackageConfigHelpers )

set( CMAKE_CXX_STANDARD 23 )
set( CMAKE_CXX_STANDARD_REQUIRED ON )
set( CMAKE_CXX_EXTENSIONS OFF )

add_library(
    rukh_wdl
    src/wdl.cpp )
add_library(
    RukhWdl::RukhWdl
    ALIAS rukh_wdl )
set_target_properties(
    rukh_wdl
    PROPERTIES EXPORT_NAME RukhWdl )

target_include_directories(
    rukh_wdl
    PUBLIC
        "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>"
        "$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>" )
target_compile_features(
    rukh_wdl
    PUBLIC cxx_std_23 )
target_compile_options(
    rukh_wdl
    PRIVATE
        $<$<CXX_COMPILER_ID:Clang>:-Wall;-Wextra;-Wpedantic;-Wcast-qual> )

install(
    TARGETS rukh_wdl
    EXPORT RukhWdlTargets
    ARCHIVE DESTINATION "${CMAKE_INSTALL_LIBDIR}"
    LIBRARY DESTINATION "${CMAKE_INSTALL_LIBDIR}"
    RUNTIME DESTINATION "${CMAKE_INSTALL_BINDIR}"
    INCLUDES DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}" )
install( DIRECTORY include/ DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}" )
install(
    EXPORT RukhWdlTargets
    NAMESPACE RukhWdl::
    FILE RukhWdlTargets.cmake
    DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/rukh_wdl" )

configure_package_config_file(
    cmake/RukhWdlConfig.cmake.in
    "${CMAKE_CURRENT_BINARY_DIR}/rukh_wdlConfig.cmake"
    INSTALL_DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/rukh_wdl" )
write_basic_package_version_file(
    "${CMAKE_CURRENT_BINARY_DIR}/rukh_wdlConfigVersion.cmake"
    VERSION "${PROJECT_VERSION}"
    COMPATIBILITY SameMajorVersion )
install(
    FILES
        "${CMAKE_CURRENT_BINARY_DIR}/rukh_wdlConfig.cmake"
        "${CMAKE_CURRENT_BINARY_DIR}/rukh_wdlConfigVersion.cmake"
    DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/rukh_wdl" )

if( BUILD_TESTING )
    add_executable(
        rukh_wdl_tests
        tests/wdl_tests.cpp )
    target_link_libraries(
        rukh_wdl_tests
        PRIVATE RukhWdl::RukhWdl )
    add_test(
        NAME rukh-wdl-numeric
        COMMAND rukh_wdl_tests )
endif()
