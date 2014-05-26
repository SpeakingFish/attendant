set(TARGET crash_tests)

cmake_minimum_required(VERSION 2.6)
set(PROJECT_ROOT ${CMAKE_CURRENT_SOURCE_DIR}/../..)
include(${PROJECT_ROOT}/CMakeScripts/ProjectCommon.cmake)

find_package(Qt4 REQUIRED QtCore)
include(${PROJECT_ROOT}/CMakeScripts/QtCommonActions.cmake)

project_module(attendant)

set(HEADERS
)

set(SOURCES
	crash_tests.cpp
)

kmpx_project_add_executable(${TARGET}
	${HEADERS}
	${SOURCES}
)

target_link_libraries(${TARGET}
	attendant
	${QT_LIBRARIES}
)

kmpx_project_install(${TARGET})
