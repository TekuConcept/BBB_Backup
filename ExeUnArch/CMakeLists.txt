cmake_minimum_required(VERSION 3.0.2)
project(DemoLZMA)

set(CMAKE_FIND_LIBRARY_SUFFIXES ${CMAKE_STATIC_LIBRARY_SUFFIX})
# git clone http://git.tukaani.org/xz.git
#find_package(LibLZMA)
find_library(LIBLZMA_LIBRARIES
	lzma
	PATHS "C:/Users/na/Documents/GitHub/XZ/windows/Release"
	PATH_SUFFIXES ${CMAKE_STATIC_LIBRARY_SUFFIX}
)

message(STATUS ${LIBLZMA_FOUND})
message(STATUS ${LIBLZMA_LIBRARIES})

add_definitions(-DLZMA_API_STATIC)

set(LIBLZMA_INCLUDES "UNKNOWN" CACHE PATH "")
include_directories(
    Include
    ${LIBLZMA_INCLUDES}
)
file(GLOB_RECURSE SOURCE_FILES "Source/*.cpp")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

add_executable(DemoLZMA ${SOURCE_FILES} main.cpp)
target_link_libraries(DemoLZMA ${LIBLZMA_LIBRARIES})

#add_executable(Hello hello.cpp)