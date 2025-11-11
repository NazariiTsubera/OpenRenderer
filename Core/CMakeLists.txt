cmake_minimum_required(VERSION 3.20)

set(SRC_DIR "${CMAKE_CURRENT_SOURCE_DIR}/src")

file(GLOB_RECURSE SOURCES "${SRC_DIR}/*.cpp")
file(GLOB_RECURSE HEADERS "${SRC_DIR}/*.h")

add_library(Core STATIC ${SOURCES} ${HEADERS})

target_include_directories(Core
    PUBLIC
        ${SRC_DIR}
)

target_link_libraries(Core
    PUBLIC
        glfw
        glm::glm
        EnTT::EnTT
    PRIVATE
        glad
        assimp
        yaml-cpp
)

if(APPLE)
    target_compile_definitions(Core PRIVATE GL_SILENCE_DEPRECATION)
    target_link_libraries(Core
        PUBLIC
            "-framework OpenGL"
            "-framework Cocoa"
            "-framework IOKit"
            "-framework QuartzCore"
    )
endif()
