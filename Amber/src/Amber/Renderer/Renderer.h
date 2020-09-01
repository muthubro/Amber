#pragma once

#include <glm/glm.hpp>

#include "Amber/Math/Frustum.h"

#include "Amber/Renderer/Mesh.h"
#include "Amber/Renderer/RenderCommand.h"
#include "Amber/Renderer/RenderPass.h"
#include "Amber/Renderer/Shader.h"

namespace Amber 
{

class Renderer 
{
public:
    static void Init();
    static void Shutdown();

    template<typename FuncT>
    static void Submit(FuncT&& func) 
    { 
        RenderCommand::Submit(std::forward<FuncT>(func));
    }

    static void WaitAndRender();

    static void BeginRenderPass(const Ref<RenderPass>& renderpass, bool clear = true);
    static void EndRenderPass();

    static void DrawFullscreenQuad(const Ref<MaterialInstance>& material);
    static void DrawMesh(Ref<Mesh> mesh, const glm::mat4& transform, Ref<MaterialInstance> overrideMaterial = nullptr);

    static void DrawAABB(const Math::AABB& aabb, const glm::mat4& transform, const glm::vec4& color = glm::vec4(1.0f));
    static void DrawAABB(Ref<Mesh> mesh, const glm::mat4& transform, const glm::vec4& color = glm::vec4(1.0f));

    static void DrawFrustum(const Math::Frustum& frustum, const glm::vec4& color = glm::vec4(1.0f));

    static const Scope<ShaderLibrary>& GetShaderLibrary();
    static RendererAPI::API GetAPI() { return RendererAPI::GetAPI(); }
};

}