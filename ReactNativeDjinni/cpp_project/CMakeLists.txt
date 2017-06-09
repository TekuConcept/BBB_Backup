cmake_minimum_required(VERSION 3.1.0)
project(TestDjinni)

#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
set(CMAKE_CXX_STANDARD 11)

include_directories(
	"../djinni-src/cpp"
	"../src/cpp"
)

set(SRC_FILES
	"../src/cpp/hello_world_impl.cpp"
	"main.cpp"
)

add_executable(run ${SRC_FILES})