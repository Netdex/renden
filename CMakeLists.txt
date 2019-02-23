cmake_minimum_required(VERSION 3.1)
project(renden)

set (CMAKE_CXX_STANDARD 14)

option(GLFW_BUILD_DOCS OFF)
option(GLFW_BUILD_EXAMPLES OFF)
option(GLFW_BUILD_TESTS OFF)
add_subdirectory(vendor/glfw)

option(ASSIMP_BUILD_ASSIMP_TOOLS OFF)
option(ASSIMP_BUILD_SAMPLES OFF)
option(ASSIMP_BUILD_TESTS OFF)
add_subdirectory(vendor/assimp)

option(BUILD_BULLET2_DEMOS OFF)
option(BUILD_CPU_DEMOS OFF)
option(BUILD_EXTRAS OFF)
option(BUILD_OPENGL3_DEMOS OFF)
option(BUILD_UNIT_TESTS OFF)
add_subdirectory(vendor/bullet)

set(JSON_BuildTests OFF CACHE INTERNAL "")
add_subdirectory(vendor/json)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

if(MSVC)
    add_compile_options("/W4" "$<$<CONFIG:RELEASE>:/O2>")
#    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W4")
#    set(CMAKE_CXX_FLAGS_RELEASE "") MSVC hates this
else()
    add_compile_options("-Wall" "-Wextra" "$<$<CONFIG:RELEASE>:-O3>" "$<$<CONFIG:DEBUG>:-g>")
    if(NOT WIN32)
        set(GLAD_LIBRARIES dl)
    endif()
endif()


include_directories(renden/inc/
                    vendor/assimp/include/
                    vendor/bullet/src/
                    vendor/glad/include/
                    vendor/glfw/include/
                    vendor/glm/
                    vendor/stb/)

file(GLOB VENDORS_SOURCES vendor/glad/src/glad.c)
file(GLOB_RECURSE PROJECT_HEADERS renden/inc/*.hpp)
file(GLOB_RECURSE PROJECT_SOURCES renden/src/*.cpp)
file(GLOB_RECURSE PROJECT_SHADERS renden/shaders/*.comp
                          renden/shaders/*.frag
                          renden/shaders/*.geom
                          renden/shaders/*.vert)

file(GLOB PROJECT_CONFIGS CMakeLists.txt
                          Readme.md
                         .gitattributes
                         .gitignore
                         .gitmodules)

source_group("inc" FILES ${PROJECT_HEADERS})
source_group("shaders" FILES ${PROJECT_SHADERS})
source_group("src" FILES ${PROJECT_SOURCES})
source_group("vendors" FILES ${VENDORS_SOURCES})

add_definitions(-DGLFW_INCLUDE_NONE
                -DPROJECT_SOURCE_DIR=\"${PROJECT_SOURCE_DIR}\")
add_executable(${PROJECT_NAME} ${PROJECT_SOURCES} ${PROJECT_HEADERS}
                               ${PROJECT_SHADERS} ${PROJECT_CONFIGS}
                               ${VENDORS_SOURCES})
target_link_libraries(${PROJECT_NAME} PUBLIC assimp glfw
                      ${GLFW_LIBRARIES} ${GLAD_LIBRARIES}
                      BulletDynamics BulletCollision LinearMath)
target_link_libraries(${PROJECT_NAME} PRIVATE nlohmann_json::nlohmann_json)

set_target_properties(${PROJECT_NAME} PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/${PROJECT_NAME})
