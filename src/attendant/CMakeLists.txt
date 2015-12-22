set(TARGET attendant)
project(${TARGET})

cmake_minimum_required(VERSION 2.6)
set(PROJECT_ROOT ${CMAKE_CURRENT_SOURCE_DIR}/../..)
include(${PROJECT_ROOT}/CMakeScripts/ProjectCommon.cmake)

find_package(Qt4 REQUIRED QtCore)
include(${PROJECT_ROOT}/CMakeScripts/QtCommonActions.cmake)

set(MOC_HEADERS
)

set(HEADERS
	${MOC_HEADERS}

	at_assert.h
	at_crash_handler.h
	at_log.h
	qgzip.h
)

set(ALL_HEADERS
	${MOC_HEADERS}
	${HEADERS}
)

set(SOURCES
	at_crash_handler.cpp
	at_log.cpp
	qgzip.cpp
)

attendant_project_add_library(${TARGET}
    SHARED
    ${ALL_HEADERS}
    ${SOURCES}
)

target_link_libraries(${TARGET})

attendant_project_install(${TARGET}
    common
    HEADERS
)
