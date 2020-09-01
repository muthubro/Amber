#pragma once

#include <glm/glm.hpp>

#include "Amber/Renderer/Camera.h"
#include "Amber/Renderer/RenderPass.h"
#include "Amber/Renderer/Texture.h"

#include "Amber/Scene/Entity.h"
#include "Amber/Scene/Scene.h"
#include <Amber.h>

namespace Amber
{

struct SceneRendererOptions
{
    bool ShowGrid = true;
    float GridResolution = 0.025f;
    float GridScale = 16.025f;
    float GridSize = 16.025f;
    bool ShowBoundingBoxes = false;
    bool ShowCamera = false;
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

    static void SubmitCamera(const SceneCamera& camera, const glm::mat4& transform);
    static void SubmitMesh(const Ref<Mesh> mesh, const glm::mat4& transform = glm::mat4(1.0f), const Ref<MaterialInstance> = nullptr);
    static void SubmitSelectedMesh(const Ref<Mesh> mesh, const glm::mat4& transform = glm::mat4(1.0f));
    static void SubmitSprite(const Renderer2D::QuadData& quadData);

    static std::pair<Ref<TextureCube>, Ref<TextureCube>> CreateEnvironmentMap(const std::string& filepath);

    static Ref<RenderPass> GetFinalRenderPass();
    static Ref<Texture2D> GetFinalColorBuffer();

    static SceneRendererOptions& GetOptions();
    static Scope<ShaderLibrary>& GetShaderLibrary();

private:
    static void GeometryPass();
    static void CompositePass();
    static void FlushDrawList();
};

}
