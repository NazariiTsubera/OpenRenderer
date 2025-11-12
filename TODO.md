# Engine Refactor Plan

## Goals
- Centralize window/context/input lifecycle in the Core Engine so apps avoid boilerplate.
- Make subsystems (ImGui, ECS/scene, renderer, serializer, etc.) optional modules that can be mixed as needed.
- Keep rendering agnostic: allow drawing directly to the swapchain or to offscreen targets that can be embedded in custom ImGui windows.

## Near-Term Tasks
1. **Engine Runtime Skeleton**
   - Define `EngineConfig` (window, renderer backend, vsync, optional ImGui).
   - Implement `Engine::Init/Run/Shutdown` plus per-frame callbacks (`OnUpdate`, `OnRender`, `OnUI`).
   - Move current window/camera/render loop setup into the runtime.

2. **Module System**
   - Create `IEngineModule` interface with `OnAttach`, `OnDetach`, `OnUpdate`, `OnRender`.
   - Convert ImGui bootstrap into a module; register it optionally based on config.
   - Wrap scene/ECS/serializer as another module so apps can opt out.

3. **Rendering Abstractions**
   - Introduce `RenderTarget`/`FrameBuffer` handles managed by the engine.
   - Provide APIs to create/destroy targets and fetch the main swapchain target.
   - Expose helpers to bind targets inside custom render passes.

4. **ImGui Integration**
   - Keep core ImGui layer optional.
   - Expose a utility to convert engine framebuffers into `ImTextureID`.
   - Provide a reusable `ImguiRenderPanel` component for convenience, but let apps build their own windows by grabbing framebuffers directly.

5. **Sample Applications**
   - Minimal sample: render triangle directly to main buffer without ImGui/ECS.
   - Editor sample: enable full stack (window + renderer + ECS + ImGui + panels).
   - Ensure both samples share the same engine runtime to validate modularity.

## Stretch Ideas
- Configuration presets (e.g., `EngineConfig::Minimal()`, `EngineConfig::Editor()`).
- Event bus for cross-module communication (input, selection, undo systems).
- Scriptable module registration (e.g., JSON/TOML describing which subsystems to load).
- Hot-reloadable render passes for editor workflows.

## Notes
- Keep dependencies flowing one way: runtime core <- optional modules <- applications.
- Preserve current functionality while refactoring; migrate features incrementally.
- Document public engine APIs early so downstream apps (editor, future tools) can adapt smoothly.
