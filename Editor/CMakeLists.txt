cmake_minimum_required(VERSION 3.20)

set(SRC_DIR "${CMAKE_CURRENT_SOURCE_DIR}/src")

file(GLOB_RECURSE SOURCES "${SRC_DIR}/*.cpp")
file(GLOB_RECURSE HEADERS "${SRC_DIR}/*.h")

add_executable(Editor ${SOURCES} ${HEADERS})

target_include_directories(Editor PRIVATE ${SRC_DIR})

target_link_libraries(Editor
    PRIVATE
        Core
        imgui
        imguizmo
        tfd
)

if(APPLE)
    target_compile_definitions(Editor PRIVATE GL_SILENCE_DEPRECATION)
endif()

add_custom_target(CopyRes
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_SOURCE_DIR}/Editor/res/
        ${CMAKE_BINARY_DIR}/Editor/res
        COMMENT "Copying resource files to ${CMAKE_BINARY_DIR}/Core/res/"
)

add_dependencies(Editor CopyRes)
