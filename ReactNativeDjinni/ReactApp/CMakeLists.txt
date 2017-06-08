cmake_minimum_required(VERSION 3.4.0)
project(NativeExtensions)

set(CMAKE_CXX_STANDARD 11)

include_directories(
    "../ThirdParty/Djinni/support-lib"
    "../ThirdParty/Djinni/support-lib/jni"
	"../djinni-src/cpp"
	"../djinni-src/jni"
	"../src/cpp"
)

set(SRC_FILES
    "../ThirdParty/Djinni/support-lib/jni/djinni_support.cpp"
    "../ThirdParty/Djinni/support-lib/jni/djinni_main.cpp"
    "../djinni-src/jni/NativeHelloWorld.cpp"
    "../src/cpp/hello_world_impl.cpp"
)

add_library(native-lib SHARED ${SRC_FILES})

find_library(log-lib log )

target_link_libraries(native-lib ${log-lib})
