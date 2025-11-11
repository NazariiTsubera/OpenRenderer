# OpenRenderer

A cross-platform open source renderer.

## Build

All third-party dependencies (GLFW, ImGui, ImGuizmo, yaml-cpp, Assimp, GLM, EnTT, TinyFileDialogs, etc.) are fetched automatically with CMake's `FetchContent`, so no git submodules or manual downloads are required. The first configure step needs network access so that CMake can download the archives.

```bash
cmake -S . -B build
cmake --build build
```

This generates the `Core` library and the `Editor` executable in the `build` tree. Use standard CMake presets or generators as needed for your platform.
