//
// Created by Nazarii on 11/28/2024.
//

#ifndef SCENE_H
#define SCENE_H
#include "Model.h"
#include <glm/glm.hpp>
#include <entt/entt.hpp>
#include <stdexcept>
#include <string>
#define GLM_ENABLE_EXPERIMENTAL
#include <iostream>
#include <glm/ext/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include "Camera.h"
#include "System/Input.h"
#include "Components.h"



class Scene
{
private:
    entt::registry registry;
    std::string name;
public:
    Scene() : name("Unnamed scene") {}
    Scene(std::string  name) : name(std::move(name)) {}
    ~Scene() {}

    Scene(const Scene&) = delete;
    Scene& operator=(const Scene&) = delete;
    Scene(Scene&&) noexcept = default;
    Scene& operator=(Scene&&) noexcept = default;
public:
    entt::entity CreateEntity(const std::string& name)
    {
        auto entity = registry.create();
        for (auto e : registry.view<Name>())
        {
            if (GetComponent<Name>(e).name == name)
            {
                std::cerr << "Entity " << name << " already exists!" << std::endl;
                return entt::null;
            }
        }
        registry.emplace<Name>(entity, name);
        registry.emplace<bool>(entity, true);
        return entity;
    }

    entt::entity GetEntity(const std::string& name) const
    {
        for (entt::entity entity : registry.view<Name>())
        {
            if (registry.get<Name>(entity).name == name)
                return entity;
        }
        return entt::null;
    }

    entt::entity GetPrimaryCamera()
    {

        for (auto entity : registry.view<Camera>())
        {
            Camera camera = registry.get<Camera>(entity);
            if (camera.IsPrimary())
                return entity;
        }
        return entt::null;
    }


    bool& Active(entt::entity entity)
    {
        return registry.get<bool>(entity);
    }


    entt::registry& GetRegistry() { return registry; }

    template<typename T,typename... Args> T& AddComponent(entt::entity entity,Args&&... args)
    {
        return registry.get_or_emplace<T>(entity, std::forward<Args>(args)...);
    }

    template<typename T>
    void RemoveComponent(entt::entity entity)
    {
        registry.remove<T>(entity);
    }

    void DeleteEntity(entt::entity entity)
    {
        registry.destroy(entity);
    }

    template<typename T>
    T& GetComponent(entt::entity entity)
    {
        if (registry.all_of<T>(entity))
            return registry.get<T>(entity);

        throw std::runtime_error("Entity does not have the requested component.");
    }

    template<typename T>
    bool HasComponent(entt::entity entity)
    {
        if (registry.valid(entity))
        {
            if (registry.all_of<T>(entity)) return true;
        }

        return false;
    }

    template<typename T>
    std::vector<entt::entity> GetEntitiesWithComponent() const
    {
        std::vector<entt::entity> result;
        for (entt::entity entity : registry.view<T>())
            result.push_back(entity);
        return result;
    }

    std::string GetName() { return name; }
};



#endif //SCENE_H
