//
// Created by Nazarii on 12/3/2024.
//

#include "Serializer.h"
#include <yaml-cpp/yaml.h>
#include <Utils.h>
#include <filesystem>

namespace {

std::shared_ptr<Scene> CreateDefaultScene()
{
    auto scene = std::make_shared<Scene>("Default Scene");
    const glm::vec3 defaultCameraPosition(0.0f, 2.0f, 5.0f);
    const glm::vec3 defaultCameraRotation(0.0f);
    const glm::vec3 defaultCameraScale(1.0f);

    entt::entity cameraEntity = scene->CreateEntity("DefaultCamera");
    scene->AddComponent<Transform>(cameraEntity, defaultCameraPosition, defaultCameraRotation, defaultCameraScale);
    scene->AddComponent<Camera>(cameraEntity, true);
    return scene;
}

} // namespace



glm::vec3 DeserializeVec3(YAML::Node node, std::string key)
{
    glm::vec3 vector;
    if (node[key].IsSequence())
    {
        vector.x = node[key][0].as<float>();
        vector.y = node[key][1].as<float>();
        vector.z = node[key][2].as<float>();
    }

    return vector;
}

void Serializer::Serialize(Scene* scene, const std::string& filename)
{
    YAML::Emitter emitter;
    emitter << YAML::BeginMap;
    emitter << YAML::Key << "Scene";
    emitter << YAML::BeginSeq;


    for (entt::entity entity : scene->GetEntitiesWithComponent<Name>())
    {
        emitter << YAML::BeginMap;
        emitter << YAML::Key << "Name" << YAML::Value << scene->GetComponent<Name>(entity).name;

        if (scene->HasComponent<Transform>(entity))
        {
            Transform& transform = scene->GetComponent<Transform>(entity);

            emitter << YAML::Key << "Transform" << YAML::Value << YAML::BeginMap;

            emitter << YAML::Key << "Position" << YAML::Value;
            emitter << YAML::Flow << YAML::BeginSeq;
            emitter << transform.position.x;
            emitter << transform.position.y;
            emitter << transform.position.z;
            emitter << YAML::EndSeq;

            emitter << YAML::Key << "Rotation" << YAML::Value;
            emitter << YAML::Flow << YAML::BeginSeq;
            emitter << transform.rotation.x;
            emitter << transform.rotation.y;
            emitter << transform.rotation.z;
            emitter << YAML::EndSeq;

            emitter << YAML::Key << "Scale" << YAML::Value;
            emitter << YAML::Flow << YAML::BeginSeq;
            emitter << transform.scale.x;
            emitter << transform.scale.y;
            emitter << transform.scale.z;
            emitter << YAML::EndSeq;

            emitter << YAML::EndMap;

        }

        if (scene->HasComponent<Model>(entity))
        {
            Model& model = scene->GetComponent<Model>(entity);
            emitter << YAML::Key << "Model" << YAML::Value << model.GetFileName();
        }

        if (scene->HasComponent<Material>(entity))
        {
            Material& material = scene->GetComponent<Material>(entity);
            emitter << YAML::Key << "Material" << YAML::Value << YAML::BeginMap;
            emitter << YAML::Key << "Albedo" << YAML::Value << material.Albedo.GetFilePath();
            emitter << YAML::Key << "Shader" << YAML::Value << material.Shader.GetName();
            emitter << YAML::EndMap;
        }

        if (scene->HasComponent<Camera>(entity))
        {
            Camera& camera = scene->GetComponent<Camera>(entity);
            emitter << YAML::Key << "Camera" << YAML::Value << YAML::BeginMap;
            emitter << YAML::Key << "Primary" << YAML::Value << camera.IsPrimary();
            emitter << YAML::EndMap;
        }


        emitter << YAML::EndMap;
    }

    emitter << YAML::EndSeq;
    emitter << YAML::EndMap;

    File::writeFile(filename, emitter.c_str());
}

std::shared_ptr<Scene> Serializer::Deserialize(const std::string& filename)
{
    auto fallbackScene = [](const std::string& reason) {
        std::cerr << "[Serializer] " << reason << " Falling back to default scene." << std::endl;
        return CreateDefaultScene();
    };

    if (filename.empty())
        return fallbackScene("Empty scene filename provided.");

    std::filesystem::path scenePath(filename);
    if (!scenePath.is_absolute())
    {
        std::filesystem::path searchRoot = std::filesystem::current_path();
        bool resolved = false;
        while (true)
        {
            std::filesystem::path candidate = searchRoot / scenePath;
            if (std::filesystem::exists(candidate))
            {
                scenePath = candidate;
                resolved = true;
                break;
            }
            if (!searchRoot.has_parent_path() || searchRoot.parent_path() == searchRoot)
                break;
            searchRoot = searchRoot.parent_path();
        }

        if (!resolved)
        {
            // Combine with current path just for logging clarity
            scenePath = std::filesystem::current_path() / scenePath;
        }
    }

    YAML::Node root;
    try
    {
        if (!std::filesystem::exists(scenePath))
            return fallbackScene("Scene file not found: " + scenePath.string());

        root = YAML::LoadFile(scenePath.string());
    }
    catch (const YAML::Exception& e)
    {
        return fallbackScene(std::string("Failed to load scene '") + scenePath.string() + "': " + e.what());
    }

    if (!root["Scene"])
        return fallbackScene("Scene file does not contain a 'Scene' node: " + scenePath.string());

    std::shared_ptr<Scene> scene = std::make_shared<Scene>(filename);

    for (auto node : root["Scene"])
    {
        auto entity = scene->CreateEntity(node["Name"].as<std::string>());
        if (node["Transform"])
        {
            glm::vec3 position = DeserializeVec3(node["Transform"], "Position");
            glm::vec3 rotation = DeserializeVec3(node["Transform"], "Rotation");
            glm::vec3 scale = DeserializeVec3(node["Transform"], "Scale");

            std::cout << position.x << std::endl;

            scene->AddComponent<Transform>(entity, position, rotation, scale);
        }

        if (node["Model"])
        {
            std::string filename = node["Model"].as<std::string>();
            scene->AddComponent<Model>(entity, filename);
        }

        if (node["Material"])
        {
            std::string albedo = node["Material"]["Albedo"].as<std::string>();
            std::string shader = node["Material"]["Shader"].as<std::string>();

            scene->AddComponent<Material>(entity, Texture(albedo), Shader::DefaultShader, "x");
        }

        if (node["Camera"])
        {
            bool primary = node["Camera"]["Primary"].as<bool>();
            scene->AddComponent<Camera>(entity, primary);
        }
    }

    return scene;
}
