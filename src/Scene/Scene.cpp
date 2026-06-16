#include "Scene/Scene.h"
#include "Renderer/Renderer.h"
#include <glm/gtc/matrix_transform.hpp>
#include "Systems/RenderSystem.h"
#include <fstream>

Scene::Scene() {
    m_Entities.reserve(10000);
    m_Transforms.reserve(10000);
    m_Renderables.reserve(10000);
    m_Textures.reserve(10000);
    Entity left = CreateEntity();
    Entity middle = CreateEntity();
    Entity right = CreateEntity();
    

    GetTransform(left).Position = {-0.7f, 0.0f, 0.0f};
    GetTransform(right).Position = {0.7f, 0.0f, 0.0f};
}

void Scene::Clear() {
    m_Entities.clear();
    m_Transforms.clear();
    m_Renderables.clear();
    m_Textures.clear();

    while (!m_FreeEntities.empty())
        m_FreeEntities.pop();
}

void Scene::Save(const std::string& path) {
    std::ofstream out(path);

    for (size_t i = 0; i < m_Entities.size(); i++) {
        if (!m_Renderables[i].Visible)
            continue;

        auto& t = m_Transforms[i];

        out << t.Position.x << " "
            << t.Position.y << " "
            << t.Rotation << " "
            << t.Scale.x << " "
            << t.Scale.y << "\n";
    }

    out.close();
}

void Scene::Load(const std::string& path) {
    Clear();

    std::ifstream in(path);

    float px, py, rot, sx, sy;

    while (in >> px >> py >> rot >> sx >> sy) {
        Entity e = CreateEntity();

        auto& t = GetTransform(e);
        t.Position = {px, py, 0.0f};
        t.Rotation = rot;
        t.Scale = {sx, sy, 1.0f};

        GetRender(e).Visible = true;
    }

    in.close();
}


void Scene::OnUpdate(float deltaTime) {
    // TransformSystem::Update(deltaTime, m_Entities, m_Transforms);
}

void Scene::OnRender() {
    // RenderSystem::Render(m_Entities, m_Transforms, m_Renderables);
}


Entity Scene::CreateEntity() {

    if (!m_FreeEntities.empty()) {
        Entity e = m_FreeEntities.front();
        m_FreeEntities.pop();
        return e;
    }

    Entity e = (Entity)m_Entities.size();
    m_Entities.push_back(e);

    m_Transforms.emplace_back();
    m_Renderables.emplace_back();
    m_Textures.emplace_back();

    return e;
}

Entity Scene::SpawnEntity(const glm::vec3& position) {
    Entity e = CreateEntity();
    GetTransform(e).Position = position;
    GetRender(e).Visible = true;
    return e;
}

TransformComponent& Scene::GetTransform(Entity e) {
    return m_Transforms[e];
}

RenderComponent& Scene::GetRender(Entity e) {
    return m_Renderables[e];
}

void Scene::DestroyEntity(Entity e) {
    if (e >= m_Entities.size())
        return;

    m_Renderables[e].Visible = false;
    m_Textures[e].Path.clear();
    m_FreeEntities.push(e);
}

TextureComponent& Scene::GetTexture(Entity entity)
{
    return m_Textures[entity];
}

void Scene::RemoveFromFreeList(Entity e) {
    std::queue<Entity> temp;
    while (!m_FreeEntities.empty()) {
        Entity front = m_FreeEntities.front();
        m_FreeEntities.pop();
        if (front != e) {
            temp.push(front);
        }
    }
    m_FreeEntities = temp;
}

Scene::Scene(const Scene& other)
{
    m_Entities     = other.m_Entities;
    m_Transforms   = other.m_Transforms;
    m_Renderables  = other.m_Renderables;
    m_Textures     = other.m_Textures;
    m_FreeEntities = other.m_FreeEntities;
}
void Scene::SavePrefab(
    int entity,
    const std::string& path)
{
    std::ofstream out(path);

    if (!out.is_open())
        return;

    auto& transform =
        m_Transforms[entity];

    auto& render =
        m_Renderables[entity];

    auto& texture =
        m_Textures[entity];

    out <<
        transform.Position.x << " "
        << transform.Position.y << " "
        << transform.Position.z << "\n";

    out <<
        transform.Rotation << "\n";

    out <<
        texture.Path << "\n";

    out.close();
}

void Scene::LoadPrefab(
    const std::string& path)
{
    std::ifstream in(path);

    if (!in.is_open())
        return;

    glm::vec3 pos;
    float rotation;

    std::string texturePath;

    in >>
        pos.x >>
        pos.y >>
        pos.z;

    in >> rotation;

    in.ignore();

    std::getline(in, texturePath);

    Entity entity =
        SpawnEntity(pos);

    int index =
        (int)m_Entities.size() - 1;
    m_Transforms[index]
        .Rotation = rotation;

    m_Textures[index]
        .Path = texturePath;

    in.close();
}