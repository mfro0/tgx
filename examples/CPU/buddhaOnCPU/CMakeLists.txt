###############################################################################
###############################################################################
#
#                    CMakeList for project: buddhaOnCPU 
# 
# Building the example (Windows/Linux/MacOS).
# For Windows/Linuc/MacOS.
#
# Building the example:
#
# 1. Install CMake (version 3.10 later). 
#
# 2. Open a shell/terminal inside the directory that contains this file. 
#
# 3. Install CImg (only for Linux/MacOS)
#    - If on Linux/Debian, run: "sudo apt install cimg-dev"
#    - If on MacOS/homebrew, run: "brew install cimg"
#
# 4. run the following commands:
#       mkdir build
#       cd build
#       cmake ..
#
# 4. This will create the project files into the /build directory which can now 
#    be used to build the example. .
#
#    -> On Windows. Open the Visual Studio solution file "buddhaOnCPU.sln" and 
#       build the example from within the IDE. 
# 
#    -> On Linux/MacOS. Run "make" to build the example. 
###############################################################################
###############################################################################
cmake_minimum_required(VERSION 3.10)

project(buddhaOnCPU)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -mcpu=68040")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -mcpu=68040")
set(CMAKE_LD_FLAGS "${CMAKE_LD_FLAGS} -mcpu=68040")
#set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -mcpu=547x")
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -mcpu=547x")
#set(CMAKE_LD_FLAGS "${CMAKE_LD_FLAGS} -mcpu=547x")

# release is the default build type
if (NOT CMAKE_BUILD_TYPE)
	set(CMAKE_BUILD_TYPE Release)
endif ()

# add TGX
include_directories("../../../src/")
file(GLOB tgx_SRC "../../../src/*.cpp") 

# add the project files.
file(GLOB project_SRC "buddha.h" "buddhaOnCPU.cpp")

# build an executable
add_executable(${PROJECT_NAME} ${project_SRC} ${tgx_SRC})

target_compile_features(${PROJECT_NAME} PUBLIC cxx_std_17)

# override default behaviour and use R/G/B order to match CImg's color channels ordering. 
add_compile_definitions(TGX_RGB32_ORDER_BGR=1)
add_compile_definitions(TGX_RGBf_ORDER_BGR=1)

# set the project as the default startup project in visual studio.
set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT "${PROJECT_NAME}")

# move CMake specific project inside filter "CMakePredefinedTargets".
set_property(GLOBAL PROPERTY USE_FOLDERS ON)
set(PREDEFINED_TARGETS_FOLDER "CustomTargets")

target_link_libraries(${PROJECT_NAME} -lgem)

# add Atari cross compiler include directories
include_directories("/usr/m68k-atari-mintelf/include"
)
###############################################################################
#end of file
###############################################################################


