#pragma once

#include <glm/glm.hpp>

#include "Amber/Renderer/Camera.h"
#include "Amber/Renderer/RenderPass.h"
#include "Amber/Renderer/Texture.h"

#include "Amber/Scene/Entity.h"
#include "Amber/Scene/Scene.h"

namespace Amber
{

struct SceneRendererOptions
{
    bool ShowGrid = true;
    float GridResolution = 0.025f;
    float GridScale = 16.025f;
    float GridSize = 16.025f;
};

struct SceneRendererCamera
{
    Amber::Camera Camera;
    glm::mat4 ViewMatrix;
};

class SceneRenderer
{
public:
    static void Init();
    static void SetViewportSize(uint32_t width, uint32_t height);

    static void BeginScene(Scene* scene, const SceneRendererCamera& camera);
    static void EndScene();

    static void SubmitMesh(const Ref<Mesh> mesh, const glm::mat4& transform = glm::mat4(1.0f), const Ref<MaterialInstance> = nullptr);

    static std::pair<Ref<TextureCube>, Ref<TextureCube>> CreateEnvironmentMap(const std::string& filepath);

    static Ref<RenderPass> GetFinalRenderPass();
    static Ref<Texture2D> GetFinalColorBuffer();

    static SceneRendererOptions& GetOptions();

private:
    static void GeometryPass();
    static void CompositePass();
    static void FlushDrawList();
};

}
