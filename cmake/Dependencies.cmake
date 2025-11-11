include(FetchContent)

if(POLICY CMP0169)
    cmake_policy(SET CMP0169 OLD)
endif()

set(FETCHCONTENT_UPDATES_DISCONNECTED ON)

# Glad is already part of the repository; expose it once for the whole project.
if(NOT TARGET glad)
    add_subdirectory(${CMAKE_SOURCE_DIR}/Core/lib/Glad ${CMAKE_BINARY_DIR}/thirdparty/glad)
endif()

# GLFW
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_INSTALL OFF CACHE BOOL "" FORCE)
FetchContent_Declare(
    glfw
    GIT_REPOSITORY https://github.com/glfw/glfw.git
    GIT_TAG 3.4
)
FetchContent_MakeAvailable(glfw)

# GLM
FetchContent_Declare(
    glm
    GIT_REPOSITORY https://github.com/g-truc/glm.git
    GIT_TAG 0.9.9.8
)
FetchContent_GetProperties(glm)
if(NOT glm_POPULATED)
    FetchContent_Populate(glm)
    add_library(glm INTERFACE)
    target_include_directories(glm INTERFACE ${glm_SOURCE_DIR})
    add_library(glm::glm ALIAS glm)
endif()

# EnTT
FetchContent_Declare(
    entt
    GIT_REPOSITORY https://github.com/skypjack/entt.git
    GIT_TAG v3.13.2
)
FetchContent_MakeAvailable(entt)

# yaml-cpp
set(YAML_CPP_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(YAML_BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)
set(YAML_CPP_BUILD_TOOLS OFF CACHE BOOL "" FORCE)
FetchContent_Declare(
    yaml-cpp
    GIT_REPOSITORY https://github.com/jbeder/yaml-cpp.git
    GIT_TAG yaml-cpp-0.7.0
)
FetchContent_GetProperties(yaml-cpp)
if(NOT yaml-cpp_POPULATED)
    FetchContent_Populate(yaml-cpp)
    set(_yaml_cpp_cmakelists "${yaml-cpp_SOURCE_DIR}/CMakeLists.txt")
    if(EXISTS "${_yaml_cpp_cmakelists}")
        file(READ "${_yaml_cpp_cmakelists}" _yaml_cpp_cmake_contents)
        if(NOT _yaml_cpp_cmake_contents MATCHES "cmake_minimum_required\\(VERSION 3\\.5")
            string(REGEX REPLACE "cmake_minimum_required\\(VERSION [^)]+\\)"
                "cmake_minimum_required(VERSION 3.5)"
                _yaml_cpp_cmake_contents
                "${_yaml_cpp_cmake_contents}"
            )
            file(WRITE "${_yaml_cpp_cmakelists}" "${_yaml_cpp_cmake_contents}")
        endif()
    endif()
    add_subdirectory(${yaml-cpp_SOURCE_DIR} ${yaml-cpp_BINARY_DIR})
endif()

# Assimp
set(ASSIMP_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(ASSIMP_BUILD_ZLIB ON CACHE BOOL "" FORCE)
set(ASSIMP_BUILD_ASSIMP_TOOLS OFF CACHE BOOL "" FORCE)
set(ASSIMP_INSTALL OFF CACHE BOOL "" FORCE)
FetchContent_Declare(
    assimp
    GIT_REPOSITORY https://github.com/assimp/assimp.git
    GIT_TAG v5.3.1
)
FetchContent_MakeAvailable(assimp)

# ImGui
FetchContent_Declare(
    imgui
    GIT_REPOSITORY https://github.com/ocornut/imgui.git
    GIT_TAG docking
)
FetchContent_GetProperties(imgui)
if(NOT imgui_POPULATED)
    FetchContent_Populate(imgui)
    file(GLOB IMGUI_CORE_SRC
        ${imgui_SOURCE_DIR}/imgui*.cpp
        ${imgui_SOURCE_DIR}/misc/cpp/imgui_stdlib.cpp
        ${imgui_SOURCE_DIR}/backends/imgui_impl_glfw.cpp
        ${imgui_SOURCE_DIR}/backends/imgui_impl_opengl3.cpp
    )
    add_library(imgui STATIC ${IMGUI_CORE_SRC})
    target_include_directories(imgui PUBLIC
        ${imgui_SOURCE_DIR}
        ${imgui_SOURCE_DIR}/backends
        ${imgui_SOURCE_DIR}/misc/cpp
    )
    target_link_libraries(imgui PUBLIC glfw glad)
    target_compile_definitions(imgui PUBLIC IMGUI_IMPL_OPENGL_LOADER_GLAD)
endif()

# TinyFileDialogs
FetchContent_Declare(
    tinyfiledialogs
    GIT_REPOSITORY https://github.com/native-toolkit/tinyfiledialogs.git
    GIT_TAG master
)
FetchContent_GetProperties(tinyfiledialogs)
if(NOT tinyfiledialogs_POPULATED)
    FetchContent_Populate(tinyfiledialogs)
    add_library(tfd STATIC
        ${tinyfiledialogs_SOURCE_DIR}/tinyfiledialogs.c
    )
    target_include_directories(tfd PUBLIC ${tinyfiledialogs_SOURCE_DIR})
    if(WIN32)
        target_link_libraries(tfd PUBLIC ole32 uuid comdlg32)
        target_compile_definitions(tfd PRIVATE _CRT_NONSTDC_NO_DEPRECATE _CRT_SECURE_NO_WARNINGS)
    endif()
endif()

# ImGuizmo
FetchContent_Declare(
    imguizmo
    GIT_REPOSITORY https://github.com/CedricGuillemet/ImGuizmo.git
    GIT_TAG master
)
FetchContent_GetProperties(imguizmo)
if(NOT imguizmo_POPULATED)
    FetchContent_Populate(imguizmo)
    add_library(imguizmo STATIC
        ${imguizmo_SOURCE_DIR}/ImGuizmo.cpp
    )
    target_include_directories(imguizmo PUBLIC ${imguizmo_SOURCE_DIR})
    target_link_libraries(imguizmo PUBLIC imgui glm::glm)
endif()
