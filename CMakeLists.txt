cmake_minimum_required(VERSION 3.16)
project(renden)

set(CMAKE_CXX_STANDARD 17)

set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(GLFW_INSTALL OFF CACHE BOOL "" FORCE)
add_subdirectory(vendor/glfw)

include_directories(src/)

include_directories(SYSTEM
	vendor/glad/include/
	vendor/glfw/include/
	vendor/glm/
	vendor/stb/
	vendor/spdlog/include/
	vendor/span-lite/include/
	vendor/imgui/
	vendor/imgui/examples/
    vendor/toml11/
)

add_definitions(
	-DGLFW_INCLUDE_NONE
    -DIMGUI_IMPL_OPENGL_LOADER_GLAD
	-DPROJECT_SOURCE_DIR=\"${PROJECT_SOURCE_DIR}\"
)

file(GLOB VENDORS_SOURCES 
    vendor/glad/src/glad.c
    vendor/imgui/imgui.cpp
    vendor/imgui/imgui_demo.cpp
    vendor/imgui/imgui_draw.cpp
    vendor/imgui/imgui_widgets.cpp
    vendor/imgui/examples/imgui_impl_opengl3.cpp
    vendor/imgui/examples/imgui_impl_glfw.cpp
)

SET(PROJECT_MAIN src/main.cpp)
file(GLOB_RECURSE PROJECT_HEADERS src/*.hpp)
file(GLOB_RECURSE PROJECT_SOURCES src/*.cpp)
list(REMOVE_ITEM PROJECT_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/${PROJECT_MAIN})

file(GLOB_RECURSE PROJECT_SHADERS 
    shaders/*.comp
	shaders/*.frag
	shaders/*.geom
	shaders/*.vert
)

file(GLOB PROJECT_CONFIGS CMakeLists.txt
                          Readme.md
                         .gitattributes
                         .gitignore
                         .gitmodules)

source_group("inc" FILES ${PROJECT_HEADERS})
source_group("shaders" FILES ${PROJECT_SHADERS})
source_group("src" FILES ${PROJECT_SOURCES})
source_group("vendor" FILES ${VENDORS_SOURCES})

add_library(coverage_config INTERFACE)
option(CODE_COVERAGE "Enable coverage reporting" OFF)
if(CODE_COVERAGE AND CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
	target_compile_options(coverage_config INTERFACE -O0 -g --coverage)
	target_link_options(coverage_config INTERFACE --coverage)
endif()

option(PACKAGE_TESTS "Build tests for this project" OFF)
if(PACKAGE_TESTS)
    enable_testing()
    include(GoogleTest)
    add_subdirectory(tests)
endif()

add_library(${PROJECT_NAME}_lib STATIC ${PROJECT_SOURCES} ${PROJECT_HEADERS}
									   ${PROJECT_SHADERS} ${PROJECT_CONFIGS}
									   ${VENDORS_SOURCES})
add_executable(${PROJECT_NAME} ${PROJECT_MAIN})

target_link_libraries(${PROJECT_NAME}_lib PUBLIC glfw ${GLFW_LIBRARIES} ${GLAD_LIBRARIES} coverage_config)
target_link_libraries(${PROJECT_NAME} PUBLIC ${PROJECT_NAME}_lib coverage_config)

if(MSVC)
	set_source_files_properties(${PROJECT_SOURCES} PROPERTIES COMPILE_FLAGS "/experimental:external /external:W0 /external:anglebrackets /W4 /WX")
	set_source_files_properties(${VENDORS_SOURCES} PROPERTIES COMPILE_FLAGS "/w")
	target_link_options(${PROJECT_NAME} PRIVATE "$<$<CONFIG:RELWITHDEBINFO>:/INCREMENTAL:NO>" "$<$<CONFIG:RELWITHDEBINFO>:/PROFILE>")
else()
    target_compile_options(${PROJECT_NAME} PRIVATE "-Wall" "-Wextra" "-Werror" "$<$<CONFIG:RELEASE>:-O3>" "$<$<CONFIG:DEBUG>:-g>")
    set_source_files_properties(${VENDORS_SOURCES} PROPERTIES COMPILE_FLAGS "-w")
    if(NOT WIN32)
        set(GLAD_LIBRARIES dl)
    endif()
endif()

set_target_properties(${PROJECT_NAME} PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/${PROJECT_NAME})
