cmake_minimum_required(VERSION 3.12)
project(CGT VERSION 2.2)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_MODULE_PATH "${CMAKE_MODULE_PATH};${PROJECT_SOURCE_DIR}/cmake")
set(CMAKE_DEBUG_POSTFIX d)

include_directories("include")

if(MSVC)
	add_definitions(-DNOMINMAX)
	set(EXECUTABLE_OUTPUT_PATH  "${PROJECT_SOURCE_DIR}/binary/windows")
	set(LIBRARY_OUTPUT_PATH     "${PROJECT_SOURCE_DIR}/binary/windows")
else()
	set(EXECUTABLE_OUTPUT_PATH  "${PROJECT_SOURCE_DIR}/binary/linux")
	set(LIBRARY_OUTPUT_PATH     "${PROJECT_SOURCE_DIR}/binary/linux")
endif()

if(CMAKE_BUILD_TYPE MATCHES Debug)
	set(EXECUTABLE_OUTPUT_PATH  "${EXECUTABLE_OUTPUT_PATH}/debug")
	set(LIBRARY_OUTPUT_PATH     "${LIBRARY_OUTPUT_PATH}/debug")
else()
	set(EXECUTABLE_OUTPUT_PATH  "${EXECUTABLE_OUTPUT_PATH}/release")
	set(LIBRARY_OUTPUT_PATH     "${LIBRARY_OUTPUT_PATH}/release")
endif()

find_package(OpenSceneGraph
    REQUIRED COMPONENTS
	osgDB
	osgAnimation
	osgUtil)

include_directories(${OPENSCENEGRAPH_INCLUDE_DIRS})

find_package(GDAL REQUIRED)
include_directories(${GDAL_INCLUDE_DIRS})
add_subdirectory(cgt)