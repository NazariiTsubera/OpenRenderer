//
// Created by Nazarii on 11/28/2024.
//

#include "Application.h"
#include "System/Input.h"
#include <iostream>
#include <filesystem>
#include <glm/glm.hpp>
#include <Rendering/Serializer.h>
#include "UI/UI.h"
#include "UI/Panel.h"

Application* Application::instance;

namespace {
std::filesystem::path ResolveScenePath(const std::filesystem::path& relativePath)
{
    if (relativePath.is_absolute())
        return relativePath;

    std::filesystem::path searchRoot = std::filesystem::current_path();
    while (true)
    {
        std::filesystem::path candidate = searchRoot / relativePath;
        if (std::filesystem::exists(candidate))
            return candidate;

        if (!searchRoot.has_parent_path() || searchRoot.parent_path() == searchRoot)
            break;
        searchRoot = searchRoot.parent_path();
    }

    return relativePath;
}
}

Application::Application()
    :
    window({1920, 1080, "OpenRenderer"}),
    frameStats({0, 0}), selected(entt::null)
{

    window.SetEventCallback(Application::EventCallback);
    Window::currentWindow = &window;

    const std::filesystem::path defaultSceneRelative = std::filesystem::path("Editor") / "scenes" / "Scene1.yaml";
    const std::filesystem::path defaultScenePath = ResolveScenePath(defaultSceneRelative);
    scene = Serializer::Deserialize(defaultScenePath.string());
    cameraEntity = scene->GetPrimaryCamera();

    if (cameraEntity == entt::null)
    {
        cameraEntity = scene->CreateEntity("DefaultCamera");
    }

    const glm::vec3 defaultCameraPosition(0.0f, 2.0f, 5.0f);
    const glm::vec3 defaultCameraRotation(0.0f);
    const glm::vec3 defaultCameraScale(1.0f);

    if (!scene->HasComponent<Transform>(cameraEntity))
    {
        scene->AddComponent<Transform>(cameraEntity, defaultCameraPosition, defaultCameraRotation, defaultCameraScale);
    }

    if (!scene->HasComponent<Camera>(cameraEntity))
    {
        scene->AddComponent<Camera>(cameraEntity, true);
    }

    frameBuffer = std::make_shared<FrameBuffer>(window.GetWidth(), window.GetHeight());

    Camera& camera = scene->GetComponent<Camera>(cameraEntity);
    Transform& transform = scene->GetComponent<Transform>(cameraEntity);
    camera.UpdateView(transform);

    cameraController = std::make_shared<CameraController>(camera, transform, 0.2);

    renderer = std::make_unique<Renderer>();
    renderer->AddLight(PointLight{glm::vec3(0), glm::vec3(1), 1});

    UI::Init();

}


Application::~Application()
{

}

void Application::Run()
{

    while (true)
    {
        frameStats.Begin();
        Camera& camera = scene->GetComponent<Camera>(cameraEntity);

        frameBuffer->Bind();
        renderer->BeginScene(camera);
        renderer->DrawScene(*scene.get());
        frameBuffer->Unbind();

        //Drawing UI
        UI::Begin();
        UI::DrawMainMenuBar();

        UI::DrawHierarchyPanel(scene.get(), selected);
        UI::DrawSceneViewPanel(frameBuffer.get(), camera, selected, scene.get());
        UI::DrawSettingsPanel(frameStats);

        UI::End();

        Input::update();
        cameraController->Update();
        // Update window
        window.Update();

        frameStats.End();
    }
}






void Application::EventCallback(Events::Event& e)
{

    if (e.GetType() == Events::Event::EventType::WindowResizeEvent)
    {
        Events::WindowResizeEvent& resizeEvent = (Events::WindowResizeEvent&)e;
    }

    if (e.GetType() == Events::Event::EventType::WindowCloseEvent)
    {
        std::exit(0);
    }
}
