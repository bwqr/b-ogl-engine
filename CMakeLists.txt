cmake_minimum_required(VERSION 3.15)
project(b-ogl-engine)

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/modules")

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O0")

file(GLOB BASE_SRC "base/game/*.h" "base/game/*.cpp" "base/gl/*.h" "base/gl/*.cpp" "base/struct/*.h" "base/window/*.h" "base/window/*.cpp" "base/window/glfw/*.h" "base/window/glfw/*.cpp"
        "*.h" "*.cpp" "objects/*.cpp" "objects/*.h" "objects/model/*.h" "objects/model/*.cpp" "base/overlay/*.h" "base/overlay/*.cpp"
        )

file(GLOB THIRDPARTY_SRC "thirdparty/stb/stb_image.h" "thirdparty/imgui/*.h" "thirdparty/imgui/*.cpp" "thirdparty/glad/*.h" "thirdparty/glad/*.c")

find_package(OpenGL)
find_package(glfw)
find_package(assimp)

find_library(dl NAMES libdl.so libdl PATHS /usr/lib)

include_directories(thirdparty)

add_executable(b-ogl-engine ${BASE_SRC} ${THIRDPARTY_SRC} thirdparty/stb/stb_image.h)
target_link_libraries(b-ogl-engine OpenGL)
target_link_libraries(b-ogl-engine glfw)
target_link_libraries(b-ogl-engine assimp)
target_link_libraries(b-ogl-engine ${glad})
target_link_libraries(b-ogl-engine ${dl})
