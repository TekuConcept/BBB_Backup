cmake_minimum_required (VERSION 3.0.2)
project(SocketsLibrary)

# - - - - - - - - - - - - - - - - - - - - - - - - - - - - #
#                 COMPILER CONFIGURATION                  #
# - - - - - - - - - - - - - - - - - - - - - - - - - - - - #

# allow native builds along side windows mingw cross-compiler
if(MINGW)
	# setup windows cross-compiler tool chain
	set(CMAKE_SYSTEM_NAME Linux)
	set(CMAKE_C_COMPILER arm-linux-gnueabihf-gcc)
	set(CMAKE_CXX_COMPILER arm-linux-gnueabihf-g++)
endif(MINGW)

if(CMAKE_COMPILER_IS_GNUCC OR CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -Wall -Werror -Wextra")
elseif(MSVC)
	if(CMAKE_CXX_FLAGS MATCHES "/W[0-4]")
    	string(REGEX REPLACE "/W[0-4]" "/W4" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
	else(CMAKE_CXX_FLAGS MATCHES "/W[0-4]")
    	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W4")
	endif(CMAKE_CXX_FLAGS MATCHES "/W[0-4]")
    set(CompilerFlags
        CMAKE_CXX_FLAGS
        CMAKE_CXX_FLAGS_DEBUG
        CMAKE_CXX_FLAGS_RELEASE
        CMAKE_C_FLAGS
        CMAKE_C_FLAGS_DEBUG
        CMAKE_C_FLAGS_RELEASE
    )
    foreach(CompilerFlag ${CompilerFlags})
        string(REPLACE "/MD" "/MT" ${CompilerFlag} "${${CompilerFlag}}")
    endforeach()
endif()

SET(CMAKE_INSTALL_PREFIX "${CMAKE_CURRENT_SOURCE_DIR}/Install" CACHE PATH "Prefix
prepended to install directories" FORCE)

# - - - - - - - - - - - - - - - - - - - - - - - - - - - - #
#                     PROJECT MODULES                     #
# - - - - - - - - - - - - - - - - - - - - - - - - - - - - #

include_directories("Include")
file(GLOB_RECURSE SOURCE_FILES "Source/*.cpp")
option(BUILD_SHARED "Build shared libraries instead of static ones." OFF)
if(${BUILD_SHARED})
    add_library(Sockets SHARED ${SOURCE_FILES})
    target_compile_definitions(Sockets PUBLIC -DDLL_EXPORTS)
else()
    add_library(Sockets STATIC ${SOURCE_FILES})
    target_compile_definitions(Sockets PUBLIC -DSTATIC)
endif()

add_executable(DemoClient "Demo/mainclient.cpp")
target_link_libraries(DemoClient Sockets)

add_executable(DemoServer "Demo/mainserver.cpp")
target_link_libraries(DemoServer Sockets)

install(
    TARGETS Sockets DemoClient DemoServer
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
)
INSTALL (
    DIRECTORY ${CMAKE_SOURCE_DIR}/Include/
    DESTINATION include
    FILES_MATCHING PATTERN "*.h*"
)
