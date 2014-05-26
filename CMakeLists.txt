cmake_minimum_required(VERSION 2.6)

set(PROJECT_ROOT ${CMAKE_CURRENT_SOURCE_DIR}/)
include(${PROJECT_ROOT}/CMakeScripts/ProjectCommon.cmake)

add_subdirectory(src)

kmpx_project_image()
