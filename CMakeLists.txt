cmake_minimum_required (VERSION 2.8)

project(lbadta)
set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/CMake)

option(CREATE_PYTHON_WRAPPER "shall wrapper code be created and compiled?" ON)
option(COMPILE_C_TESTS "compile tests for the C code" ON)

include_directories(${CHECK_INCLUDE_DIR})

file(GLOB LBADTA_SRC *.c)

set(CMAKE_VERBOSE_MAKEFILE ON)

add_library(lbadta SHARED ${LBADTA_SRC})
set_target_properties(lbadta PROPERTIES PREFIX "")

#add_definitions()
set_target_properties(lbadta PROPERTIES COMPILE_FLAGS "-Wall -pedantic -Werror -O2 -std=c99 -fPIC")
#set_target_properties(CADImport PROPERTIES LINK_FLAGS "-Wl,-z,origin")
target_link_libraries(lbadta)

install(TARGETS lbadta DESTINATION .)

#Tests
enable_testing()

if(CREATE_PYTHON_WRAPPER)
endif()

if(COMPILE_C_TESTS)
    find_package(Check REQUIRED)
    include_directories(${CMAKE_CURRENT_SOURCE_DIR})
    set(TEST_LBADTA_SRC tests/test_lbadta.c)
    add_executable(test_lbadta ${TEST_LBADTA_SRC})
    target_link_libraries(test_lbadta ${CHECK_LIBRARIES} lbadta)
    add_test(test_lbadta test_lbadta)
endif()

if(CREATE_PYTHON_WRAPPER)	
    set(Python_ADDITIONAL_VERSIONS 3)
    find_package(PythonLibs REQUIRED)
    find_package(PythonInterp REQUIRED)
    find_package(SWIG REQUIRED)
    set(CMAKE_SWIG_OUTDIR ${CMAKE_CURRENT_BINARY_DIR}/swig)
	include_directories(/usr/include/python3.2)
	include_directories(${CMAKE_CURRENT_SOURCE_DIR})
	include(${SWIG_USE_FILE})
	set(CMAKE_C_FLAGS "-Wall -pedantic -O2 -std=c99 -fPIC")        
	swig_add_module(pylbadta python swig/lbadta.i)
    #set_target_properties(pylbadta PROPERTIES LIBRARY_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/python)
	swig_link_libraries(pylbadta ${PYTHON_LIBRARIES})
endif()
