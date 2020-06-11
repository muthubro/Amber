#include "abpch.h"
#include "Renderer.h"

#include <assimp/Importer.hpp>
#include <glad/glad.h>

#include "Amber/Renderer/Renderer2D.h"
#include "Amber/Renderer/SceneRenderer.h"
#include "Amber/Renderer/Shader.h"

namespace Amber 
{

struct RendererData
{
    Scope<ShaderLibrary> ShaderLibrary;
    Ref<RenderPass> ActiveRenderPass;
};

static RendererData s_Data;

void Renderer::Init()
{
    AB_PROFILE_FUNCTION();

    s_Data.ShaderLibrary = CreateScope<ShaderLibrary>();

    s_Data.ShaderLibrary->Load("assets/shaders/MeshShader.glsl");
    s_Data.ShaderLibrary->Load("Static_Lighting", "assets/shaders/AmberPBR_Static.glsl");

    RenderCommand::Init();
    Renderer2D::Init();
    SceneRenderer::Init();

}

void Renderer::Shutdown()
{
    Renderer2D::Shutdown();
}

void Renderer::WaitAndRender()
{
    RenderCommand::GetCommandQueue().Execute();
}

void Renderer::BeginRenderPass(const Ref<RenderPass>& renderpass, bool clear)
{
    AB_CORE_ASSERT(renderpass, "Render pass is null!");

    s_Data.ActiveRenderPass = renderpass;
    renderpass->GetSpecification().TargetFramebuffer->Bind();

    if (clear)
    {
        glm::vec4 clearColor = renderpass->GetSpecification().TargetFramebuffer->GetSpecification().ClearColor;
        RenderCommand::SetClearColor(clearColor);
        RenderCommand::Clear();
    }
}

void Renderer::EndRenderPass()
{
    AB_CORE_ASSERT(s_Data.ActiveRenderPass, "No active render pass!");

    s_Data.ActiveRenderPass->GetSpecification().TargetFramebuffer->Unbind();
    s_Data.ActiveRenderPass = nullptr;
}

void Renderer::SubmitFullscreenQuad(const Ref<MaterialInstance>& material)
{
    Renderer2D::SubmitFullscreenQuad(material);
}

void Renderer::SubmitMesh(Ref<Mesh> mesh, const glm::mat4& transform, const Ref<MaterialInstance>& overrideMaterial)
{
    mesh->Bind();

    auto materials = mesh->GetMaterials();
    for (Submesh& submesh : mesh->GetSubmeshes())
    {
        auto material = materials[submesh.MaterialIndex];
        glm::mat4 transformMatrix = transform * submesh.Transform;
        glm::mat3 normalTransform = glm::transpose(glm::inverse(glm::mat3(transformMatrix)));
        material->Set("u_Transform", transformMatrix);
        material->Set("u_NormalTransform", normalTransform);

        material->Bind();
        RenderCommand::DrawIndexedOffset(
            submesh.IndexCount, PrimitiveType::Triangles, (void*)(sizeof(uint32_t) * submesh.BaseIndex), 
            submesh.BaseVertex, material->GetFlag(MaterialFlag::DepthTest));
    }
}

const Scope<ShaderLibrary>& Renderer::GetShaderLibrary()
{
    return s_Data.ShaderLibrary;
}

}