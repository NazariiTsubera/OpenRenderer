//
// Created by Nazarii on 11/25/2024.
//

#include "UI.h"

#include <iostream>
#include <algorithm>

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "GLFW/glfw3.h"
#include "System/Window.h"
#include <tinyfiledialogs.h>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/string_cast.hpp>
#include <ImGuizmo.h>

#include "imgui_internal.h"
#include "Rendering/Scene.h"

namespace UI
{

    void Init()
    {
        //ImGui initialization
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();

        ImGuiIO& io = ImGui::GetIO();
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
        io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;
        io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;

        constexpr float baseFontSize = 18.0f;
        float xscale = 1.0f;
        float yscale = 1.0f;
        glfwGetWindowContentScale(Window::currentWindow->GetHandle(), &xscale, &yscale);
        const float dpiScale = std::max(1.0f, 0.5f * (xscale + yscale));

        io.Fonts->AddFontFromFileTTF("res/fonts/TitilliumWeb-Bold.ttf", baseFontSize * dpiScale);
        io.FontDefault = io.Fonts->AddFontFromFileTTF("res/fonts/TitilliumWeb-Regular.ttf", baseFontSize * dpiScale);
        ImGuiStyle& style = ImGui::GetStyle();
        style.ScaleAllSizes(dpiScale);
        io.FontGlobalScale = 1.0f / dpiScale;

        // Setup Dear ImGui style
        ImGui::StyleColorsDark();
        SetDarkTheme();
        ImGui_ImplOpenGL3_Init();
        ImGui_ImplGlfw_InitForOpenGL(Window::currentWindow->GetHandle(), true);
    }

    void Begin()
    {
        glClear(GL_COLOR_BUFFER_BIT);

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
        ImGuizmo::BeginFrame();
        ImGui::DockSpaceOverViewport();
    }

    void End()
    {
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());


        ImGuiIO& io = ImGui::GetIO();

        if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
        {
            ImGui::UpdatePlatformWindows();
            ImGui::RenderPlatformWindowsDefault();
            

            // Restore the OpenGL rendering context to the main window DC, since platform windows might have changed it.
            glfwMakeContextCurrent(Window::currentWindow->GetHandle());
        }
    }

    void SetDarkTheme()
    {
        ImVec4* colors = ImGui::GetStyle().Colors;
        colors[ImGuiCol_FrameBg]                = ImVec4(0.27f, 0.27f, 0.28f, 0.54f);
        colors[ImGuiCol_WindowBg]               = ImVec4(0.11f, 0.11f, 0.11f, 0.84f);

        colors[ImGuiCol_TitleBg] = ImVec4{ 0.15f, 0.1505f, 0.151f, 1.0f };
        colors[ImGuiCol_TitleBgActive] = ImVec4{ 0.15f, 0.1505f, 0.151f, 1.0f };
        colors[ImGuiCol_TitleBgCollapsed] = ImVec4{ 0.15f, 0.1505f, 0.151f, 1.0f };
    }

    void DrawGuizmo(glm::mat4 view, glm::mat4 projection, Transform& transform, float scale)
    {
        // Set orthogonal projection
        ImGuizmo::BeginFrame();
        ImGuizmo::SetDrawlist();
        ImGuizmo::SetOrthographic(false);
        ImGuizmo::SetRect(ImGui::GetWindowPos().x, ImGui::GetWindowPos().y, ImGui::GetWindowSize().x, ImGui::GetWindowSize().y);

        glm::mat4 model = transform.GetModel();

        ImGuizmo::Manipulate(glm::value_ptr(view),
                                 glm::value_ptr(projection),
                                 ImGuizmo::OPERATION::TRANSLATE,
                                 ImGuizmo::WORLD,
                                 glm::value_ptr(model));

        ImGui::DebugDrawItemRect();

        std::cout << "guizmo view : "<< glm::to_string(view) << std::endl;

        if (ImGuizmo::IsUsing())
        {
            glm::vec3 newPosition, newRotation, newScale;
            ImGuizmo::DecomposeMatrixToComponents(glm::value_ptr(model),
                                                  glm::value_ptr(newPosition),
                                                  glm::value_ptr(newRotation),
                                                  glm::value_ptr(newScale));
            transform.position = newPosition;
            transform.rotation = newRotation;
            transform.scale = newScale;
        }
    }

}
