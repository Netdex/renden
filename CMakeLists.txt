cmake_minimum_required(VERSION 3.16)
project(renden)

set(CMAKE_CXX_STANDARD 17)

option(GLFW_BUILD_DOCS OFF)
option(GLFW_BUILD_EXAMPLES OFF)
option(GLFW_BUILD_TESTS OFF)
add_subdirectory(vendor/glfw)

#option(ASSIMP_BUILD_ASSIMP_TOOLS OFF)
#option(ASSIMP_BUILD_SAMPLES OFF)
#option(ASSIMP_BUILD_TESTS OFF)
#add_subdirectory(vendor/assimp)

#option(BUILD_BULLET2_DEMOS OFF)
#option(BUILD_CPU_DEMOS OFF)
#option(BUILD_EXTRAS OFF)
#option(BUILD_OPENGL3_DEMOS OFF)
#option(BUILD_UNIT_TESTS OFF)
#add_subdirectory(vendor/bullet)

option(BUILD_TESTING OFF)
add_subdirectory(vendor/json)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

include_directories(
    renden/src/
)

include_directories(SYSTEM
	vendor/assimp/include/
	vendor/bullet/src/
	vendor/glad/include/
	vendor/glfw/include/
	vendor/glm/
	vendor/stb/
	vendor/spdlog/include/
	vendor/span-lite/include/
	vendor/imgui/
	vendor/imgui/examples/
    vendor/tomlplusplus/
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

file(GLOB_RECURSE PROJECT_HEADERS renden/src/*.hpp)
file(GLOB_RECURSE PROJECT_SOURCES renden/src/*.cpp)
file(GLOB_RECURSE PROJECT_SHADERS 
    renden/shaders/*.comp
	renden/shaders/*.frag
	renden/shaders/*.geom
	renden/shaders/*.vert
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

add_definitions(
	-DGLFW_INCLUDE_NONE
    -DIMGUI_IMPL_OPENGL_LOADER_GLAD
	-DPROJECT_SOURCE_DIR=\"${PROJECT_SOURCE_DIR}\"
)
add_executable(${PROJECT_NAME} ${PROJECT_SOURCES} ${PROJECT_HEADERS}
                               ${PROJECT_SHADERS} ${PROJECT_CONFIGS}
                               ${VENDORS_SOURCES})
target_link_libraries(${PROJECT_NAME} PUBLIC glfw ${GLFW_LIBRARIES} ${GLAD_LIBRARIES})
target_link_libraries(${PROJECT_NAME} PRIVATE nlohmann_json::nlohmann_json)
#target_link_libraries(${PROJECT_NAME} PUBLIC assimp BulletDynamics BulletCollision LinearMath)

if(MSVC)
	target_compile_options(${PROJECT_NAME} PRIVATE "/experimental:external" "/external:W0" "/external:anglebrackets")
	set_source_files_properties(${PROJECT_HEADERS} ${PROJECT_SOURCES} PROPERTIES COMPILE_FLAGS "/W4 /WX")
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
