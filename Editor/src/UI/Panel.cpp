//
// Created by Nazarii on 11/29/2024.
//

#include "Panel.h"
#include "UI.h"

#include "../Application.h"
#include "imgui.h"
#include "Rendering/Scene.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include <entt/entt.hpp>
#include <iostream>
#include <sstream>
#include <tinyfiledialogs.h>
#include <typeindex>
#include <type_traits>
#include <glm/gtc/type_ptr.hpp>
#include "UI.h"
#include "ImGuizmo.h"
#include "glm/glm.hpp"







namespace UI
{
    void DrawSceneViewPanel(FrameBuffer* frameBuffer, Camera& viewCamera, entt::entity selected, Scene* scene)
    {
        ImGui::Begin("Scene View");
        static ImVec2 prevSize(0,0);


        ImVec2 currentPanelSize = ImGui::GetContentRegionAvail();
        //Updating camera and framebuffer every frame
        viewCamera.Reset(currentPanelSize.x, currentPanelSize.y);
        frameBuffer->Update(currentPanelSize.x, currentPanelSize.y);

        // Check if the texture ID is valid
        if (!(frameBuffer && frameBuffer->GetTextureId() != 0))
        {
            ImGui::Text("Invalid framebuffer texture");
            ImGui::End();
        }

        // Render the framebuffer texture
        ImGui::Image(frameBuffer->GetTextureId(),
                     ImVec2(frameBuffer->GetWidth(), frameBuffer->GetHeight()),
                     ImVec2(0, 1), ImVec2(1, 0)); // Flip texture coordinates (bottom-left to top-left)

        ImGuizmo::SetDrawlist();
        ImGuizmo::SetOrthographic(false);
        ImGuizmo::SetRect(ImGui::GetWindowPos().x, ImGui::GetWindowPos().y, currentPanelSize.x, currentPanelSize.y);


        // Drawing Gizmo for selected entity
        if (scene->HasComponent<Model>(selected))
        {
            Transform& transform = scene->GetComponent<Transform>(selected);

            // Set up Gizmo
            glm::mat4 model = transform.GetModel();

            // Manipulate Gizmo
            ImGuizmo::Manipulate(glm::value_ptr(viewCamera.GetView()),
                                 glm::value_ptr(viewCamera.GetProjection()),
                                 ImGuizmo::OPERATION::TRANSLATE,
                                 ImGuizmo::WORLD,
                                 glm::value_ptr(model));

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

        ImGui::End();
    }



    void DrawSettingsPanel(FrameStats frameStats) 
    {
        ImGui::Begin("Render Stats");

        ImGui::Text("%s", std::string("Delta Time : " + std::to_string(frameStats.DeltaTime)).c_str());
        ImGui::Text("%s", std::string("FPS : " + std::to_string(1/frameStats.DeltaTime)).c_str());

        ImGui::End();
    }

    void DrawMainMenuBar()
    {
        if (ImGui::BeginMainMenuBar())
        {
            // File menu
            if (ImGui::BeginMenu("File"))
            {
                if (ImGui::MenuItem("New", "Ctrl+N")) { /* Handle New */ }
                if (ImGui::MenuItem("Open", "Ctrl+O"))
                {
                    const char* filters[] = {"*.*", "*.yaml", "*.txt"};
                    const char* scenePath = tinyfd_openFileDialog(
                        "Open Scene", "", 3, filters, "YAML Files", 0
                    );

                    if (scenePath)
                    {
                        std::shared_ptr<Scene> scene = Serializer::Deserialize(scenePath);
                        Application::GetInstance()->SetScene(scene);
                    }

                }
                if (ImGui::MenuItem("Save", "Ctrl+S"))
                {
                    Scene* scene = Application::GetInstance()->GetScene();

                    const char* savePath = tinyfd_saveFileDialog(
                        "Save File", std::string(scene->GetName() + ".yaml").c_str(), 0, nullptr, nullptr
                    );

                    if (savePath) Serializer::Serialize(Application::GetInstance()->GetScene(), savePath);
                }
                ImGui::Separator();
                if (ImGui::MenuItem("Exit", "Alt+F4")) { /* Handle Exit */ }
                ImGui::EndMenu();
            }

            // Edit menu
            if (ImGui::BeginMenu("Edit"))
            {
                if (ImGui::MenuItem("Undo", "Ctrl+Z")) { /* Handle Undo */ }
                if (ImGui::MenuItem("Redo", "Ctrl+Y")) { /* Handle Redo */ }
                ImGui::Separator();
                if (ImGui::MenuItem("Cut", "Ctrl+X")) { /* Handle Cut */ }
                if (ImGui::MenuItem("Copy", "Ctrl+C")) { /* Handle Copy */ }
                if (ImGui::MenuItem("Paste", "Ctrl+V")) { /* Handle Paste */ }
                ImGui::EndMenu();
            }

            // View menu
            if (ImGui::BeginMenu("View"))
            {
                if (ImGui::MenuItem("Show Toolbar", nullptr, true)) { /* Handle Toolbar Toggle */ }
                if (ImGui::MenuItem("Show Sidebar", nullptr, true)) { /* Handle Sidebar Toggle */ }
                ImGui::EndMenu();
            }

            ImGui::EndMainMenuBar();
        }
    }



    void DrawPropertiesPanel(Scene* scene, entt::entity& selected)
    {
        ImGui::Begin("Properties");

        if (scene->HasComponent<Name>(selected))
        {
            auto& name = scene->GetComponent<Name>(selected).name;

            static char buffer[256];
            memset(buffer, 0, 256);
            strcpy(buffer, name.c_str());

            if (ImGui::InputText("Name", buffer, 256)) {
                name = buffer;
            }
        }

        if (scene->HasComponent<Transform>(selected))
        {
            Transform& transform = scene->GetComponent<Transform>(selected);

            if (ImGui::TreeNodeEx("Transform"))
            {
                ImGui::DragFloat3("Position", &transform.position.x, 0.1f);
                ImGui::DragFloat3("Rotation", &transform.rotation.x, 1.0f);
                ImGui::DragFloat3("Scale", &transform.scale.x, 0.1f);

                if (scene->HasComponent<Camera>(selected))
                {
                    Camera& camera = scene->GetComponent<Camera>(selected);
                    camera.UpdateView(transform);
                }

                if (ImGui::Button("Remove"))
                {
                    scene->RemoveComponent<Transform>(selected);
                }

                ImGui::TreePop();
            }
        }
        

        if (scene->HasComponent<Model>(selected))
        {
            Model& model = scene->GetComponent<Model>(selected);

            if (ImGui::TreeNodeEx("Mesh"))
            {
                ImGui::Text("%s", std::string("Mesh "  + model.GetName()).c_str());

                if (ImGui::Button("Load"))
                {
                    const char* filters[] = {"*.obj", "*.fbx", "*.gltf"};

                    const char* filePath = tinyfd_openFileDialog(
                        "Select a File", "", 1, filters,  "All Files", 0);

                    if (filePath)
                    {
                        model = Model(filePath);
                    }
                }

                if (ImGui::Button("Remove"))
                {
                    scene->RemoveComponent<Model>(selected);
                }

                ImGui::TreePop();
            }
        }
        

        if (scene->HasComponent<Material>(selected))
        {
            Material& material = scene->GetComponent<Material>(selected);

            if (ImGui::TreeNodeEx("Material"))
            {
                ImGui::Text("%s", material.Name.c_str());

                ImGui::Text("Albedo");
                ImGui::Image(material.Albedo.GetId(), ImVec2(100, 100));


                if (ImGui::Button("Load"))
                {
                    const char* filters[] = {"*.png", "*.jpg", "*.jpeg"};

                    const char* filePath = tinyfd_openFileDialog(
                        "Select a File", "", 1, filters,  "All Files", 0);

                    if (filePath)
                    {
                        material.Albedo = Texture(filePath);
                    }
                }


                if (ImGui::Button("Remove"))
                {
                    scene->RemoveComponent<Material>(selected);
                }

                ImGui::TreePop();
            }
        }

        if (scene->HasComponent<Camera>(selected))
        {
            Camera& camera = scene->GetComponent<Camera>(selected);

            if (ImGui::TreeNodeEx("Camera"))
            {
                ImGui::Text("%s", ("Is Primary : " + std::string(camera.IsPrimary()? "Yes" : "No")).c_str());
                if (ImGui::Button("Remove"))
                {
                    scene->RemoveComponent<Camera>(selected);
                }

                ImGui::TreePop();
            }
        }
       
        
        if (ImGui::BeginPopup("ComponentContext"))
        {
            if (ImGui::MenuItem("Transform"))
            {
                scene->AddComponent<Transform>(selected);
            }

            if (ImGui::MenuItem("Material"))
            {
                scene->AddComponent<Material>(selected, Material{Texture::DefaultTexture, Shader::DefaultShader, "default"});
            }

            if (ImGui::MenuItem("Model"))
            {
                scene->AddComponent<Model>(selected);
            }

            ImGui::EndPopup();
        }

        if (ImGui::Button("Add Component"))
        {
            ImGui::OpenPopup("ComponentContext");
        }

        ImGui::End();
    }





    void DrawHierarchyPanel(Scene* scene, entt::entity& selected)
    {

        ImGui::Begin("Hierarchy");

        static entt::entity hovered = entt::null;

        auto view = scene->GetRegistry().view<Name>();

        for (auto entity : view) {
            auto& nameComponent = view.get<Name>(entity);

            if (ImGui::Selectable(nameComponent.name.c_str(), selected == entity)) {
                selected = entity;
            }

            if (ImGui::IsMouseDown(1) && ImGui::IsItemHovered())
            {
                hovered = entity;
                ImGui::OpenPopup("RightClickPopup");
            }
        }



        if (ImGui::BeginPopup("RightClickPopup")) // Check if the popup is open
        {
            if (ImGui::MenuItem("Delete")) // Add a "Delete" menu item
            {
                if (scene->GetRegistry().valid(hovered) && hovered != entt::null)  // Check if the entity is valid before deletion
                {
                    std::cout << "Deleting entity " << scene->GetComponent<Name>(hovered).name << std::endl;
                    scene->GetRegistry().destroy(hovered);
                    hovered = entt::null;
                }
            }

            if (ImGui::MenuItem("Deactivate")) // Add a "Deactivate" menu item
            {
                hovered = entt::null;
            }
            


            ImGui::EndPopup(); // End the popup
        }


        static int emptyId = 0;
        //Not finished (Simple solution for now)
        if (ImGui::Button("Create Empty Entity"))
        {
            if (emptyId == 0)
                scene->CreateEntity("Empty");
            else 
                scene->CreateEntity("Empty (" + std::to_string(emptyId) + ")");
            emptyId++;
        }


        ImGui::End();

        if (selected != entt::null) 
        {
            DrawPropertiesPanel(scene, selected);
        }
    }
}
