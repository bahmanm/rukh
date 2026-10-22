cmake_minimum_required( VERSION 3.28 )

if( CMAKE_HOST_SYSTEM_NAME STREQUAL "Darwin" )
    set( CMAKE_OSX_DEPLOYMENT_TARGET "27.0" CACHE STRING "Minimum supported macOS version" FORCE )
    set( CMAKE_OSX_ARCHITECTURES "arm64" CACHE STRING "Minimum supported macOS architecture" FORCE )
endif()

project(
    rukh-support
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
    rukh_support
    src/memory.cpp )
add_library(
    RukhSupport::RukhSupport
    ALIAS rukh_support )
set_target_properties(
    rukh_support
    PROPERTIES EXPORT_NAME RukhSupport )

target_include_directories(
    rukh_support
    PUBLIC
        "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>"
        "$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>" )
target_compile_features(
    rukh_support
    PUBLIC cxx_std_23 )
target_compile_options(
    rukh_support
    PRIVATE
        $<$<CXX_COMPILER_ID:Clang>:-Wall;-Wextra;-Wpedantic;-Wcast-qual> )

install(
    TARGETS rukh_support
    EXPORT rukh_supportTargets
    ARCHIVE DESTINATION "${CMAKE_INSTALL_LIBDIR}"
    LIBRARY DESTINATION "${CMAKE_INSTALL_LIBDIR}"
    RUNTIME DESTINATION "${CMAKE_INSTALL_BINDIR}"
    INCLUDES DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}" )
install( DIRECTORY include/ DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}" )
install(
    EXPORT rukh_supportTargets
    FILE rukh_supportTargets.cmake
    NAMESPACE RukhSupport::
    DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/rukh_support" )

configure_package_config_file(
    cmake/rukh_supportConfig.cmake.in
    "${CMAKE_CURRENT_BINARY_DIR}/rukh_supportConfig.cmake"
    INSTALL_DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/rukh_support" )
write_basic_package_version_file(
    "${CMAKE_CURRENT_BINARY_DIR}/rukh_supportConfigVersion.cmake"
    VERSION "${PROJECT_VERSION}"
    COMPATIBILITY SameMajorVersion )
install(
    FILES
        "${CMAKE_CURRENT_BINARY_DIR}/rukh_supportConfig.cmake"
        "${CMAKE_CURRENT_BINARY_DIR}/rukh_supportConfigVersion.cmake"
    DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/rukh_support" )

if( BUILD_TESTING )
    add_executable(
        rukh_support_memory_tests
        tests/memory_tests.cpp )
    target_link_libraries(
        rukh_support_memory_tests
        PRIVATE RukhSupport::RukhSupport )
    add_test(
        NAME rukh-support-memory
        COMMAND rukh_support_memory_tests )
endif()
