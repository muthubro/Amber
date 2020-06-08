#pragma once

#include "Amber/Renderer/Texture.h"
#include "Amber/Renderer/RenderPass.h"

#include "Amber/Scene/Entity.h"
#include "Amber/Scene/Scene.h"

namespace Amber
{

class SceneRenderer
{
public:
    static void Init();
    static void SetViewportSize(uint32_t width, uint32_t height);

    static void BeginScene(Scene* scene);
    static void EndScene();

    static void SubmitEntity(Entity* entity);

    static std::pair<Ref<TextureCube>, Ref<TextureCube>> CreateEnvironmentMap(const std::string& filepath);

    static Ref<RenderPass> GetFinalRenderPass();
    static Ref<Texture2D> GetFinalColorBuffer();

private:
    static void GeometryPass();
    static void CompositePass();
    static void FlushDrawList();
};

}
