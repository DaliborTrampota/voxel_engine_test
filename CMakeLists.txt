cmake_minimum_required(VERSION 3.10)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if("${CMAKE_SOURCE_DIR}" STREQUAL "${CMAKE_BINARY_DIR}")
    message(FATAL_ERROR "In-source builds are not allowed. Please create a separate build directory.")
endif()

project(engine_test VERSION 1.0)

# Enable testing
option(BUILD_TESTING "Build the testing tree" ON)

add_subdirectory(voxel_engine)


set(PROJECT_RUNTIME_OUTPUT_DIR "${CMAKE_BINARY_DIR}/engine_test/$<CONFIG>")


file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS src/*.cpp src/*.h)
source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}/src" PREFIX "Source Files" FILES ${SOURCES})

# Game resources
file(GLOB_RECURSE SHADER_FILES CONFIGURE_DEPENDS shaders/*.glsl)
file(GLOB_RECURSE RESOURCE_FILES CONFIGURE_DEPENDS resources/*)

source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}/shaders" PREFIX "Shaders" FILES ${SHADER_FILES})
source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}/resources" PREFIX "Resources" FILES ${RESOURCE_FILES})

add_executable(engine_test WIN32 ${SOURCES}
        src/level/ChunkTracker.cpp
        src/level/ChunkTracker.h)
if(WIN32)
    target_link_options(engine_test PRIVATE "/SUBSYSTEM:CONSOLE")
endif()

target_link_libraries(engine_test PRIVATE VoxelEngine)
target_link_libraries(engine_test PUBLIC glfw)

target_include_directories(engine_test SYSTEM PUBLIC "${CMAKE_SOURCE_DIR}/voxel_engine/engine/src")

target_sources(engine_test PRIVATE ${RESOURCE_FILES})
target_sources(engine_test PRIVATE ${SHADER_FILES})

setup_voxelengine_resource_copy(engine_test)

if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/resources")
    add_custom_command(TARGET engine_test POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
            ${CMAKE_CURRENT_SOURCE_DIR}/resources $<TARGET_FILE_DIR:engine_test>/resources
    )
endif()

# Set debugger working directory
set_target_properties(engine_test PROPERTIES
    VS_DEBUGGER_WORKING_DIRECTORY "$<TARGET_FILE_DIR:engine_test>"
)