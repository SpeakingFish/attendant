# Possible customized cmake variables
#     CMAKE_BUILD_TYPE=Debug|Release - type of build (default 'Release')
#     INSTALL_TYPE=Files|Symlinks - installs symlinks or copy of files (default 'Files')

if(NOT CMAKE_BUILD_TYPE)
    message("!!! WARNING !!! -> CMAKE_BUILD_TYPE is not set. Default value is 'Release'")
    message("!!!             -> Use -D CMAKE_BUILD_TYPE=Debug|Release for explicit definition.")
    set(CMAKE_BUILD_TYPE "Release")
endif()

if (NOT INSTALL_TYPE)
    message("!!! WARNING !!! -> INSTALL_TYPE is not set. Default value is 'Files'")
    message("!!!             -> Use -D INSTALL_TYPE=Files|Symlinks for explicit definition.")
    set(INSTALL_TYPE "Files")
endif()

if (INSTALL_TYPE STREQUAL "Symlinks")
    set(INSTALL_SYMLINKS True)
endif()

set(CMAKE_MODULE_PATH ${PROJECT_ROOT}/CMakeScripts/ ${CMAKE_MODULE_PATH})
set(PROJECT_SRC ${PROJECT_ROOT}/src/attendant)
set(KMPX_PROJECT_INSTALL_ROOT "/tmp/kmpx/usr")
set(KMPX_PROJECT_TMP_ROOT "/tmp")
set(KMPX_PROJECT_NAME "kmpx")

set(DEBUG_SUFFIX "")
if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    set(DEBUG_SUFFIX "")
endif()

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Werror -Wextra -Wunused -fPIC")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -Wall -Werror -Woverloaded-virtual -Wextra -Wunused -fPIC")

# boost and threads
set(Boost_USE_STATIC_LIBS    ON)
set(Boost_USE_STATIC_RUNTIME ON)
set(Boost_USE_MULTITHREADED  ON)
find_package(Boost)
include_directories(${Boost_INCLUDE_DIRS})

include(Macros)
include_directories(${PROJECT_ROOT}/src)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
include_directories(${CMAKE_BINARY_DIR})
link_directories(${CMAKE_BINARY_DIR})

include(${PROJECT_ROOT}/CMakeScripts/MSWindowsSpecific.cmake)

set_if_not(LIBRARY_OUTPUT_PATH ${CMAKE_CURRENT_BINARY_DIR}/lib)
set_if_not(EXECUTABLE_OUTPUT_PATH ${CMAKE_CURRENT_BINARY_DIR}/bin)

