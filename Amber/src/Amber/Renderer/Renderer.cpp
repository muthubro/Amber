#include "abpch.h"
#include "Renderer.h"

#include <assimp/Importer.hpp>
#include <glad/glad.h>

#include "Amber/Renderer/Renderer2D.h"
#include "Amber/Renderer/Shader.h"

namespace Amber 
{

struct RendererData
{
    Scope<ShaderLibrary> ShaderLibrary;
};

static RendererData s_Data;

void Renderer::Init()
{
    AB_PROFILE_FUNCTION();

    s_Data.ShaderLibrary = CreateScope<ShaderLibrary>();

    s_Data.ShaderLibrary->Load("assets/shaders/MeshShader.glsl");

    RenderCommand::Init();
    Renderer2D::Init();
}

void Renderer::Shutdown()
{
    Renderer2D::Shutdown();
}

// TODO: Improve this API and move the OpenGL part out
void Renderer::DrawMesh(const std::string& filename, const glm::mat4& MVP)
{
    static auto mesh = CreateRef<Mesh>(filename);
    mesh->Bind();

    for (auto& submesh : mesh->GetSubmeshes())
    {
        auto baseMaterial = mesh->GetMaterial();
        baseMaterial->Set("u_MVP", MVP);

        auto material = mesh->GetMaterials()[submesh.MaterialIndex];
        material->Bind();

        Submit([=]()
        {
            if (material->GetFlag(MaterialFlag::DepthTest))
                glEnable(GL_DEPTH_TEST);
            else
                glDisable(GL_DEPTH_TEST);

            glDrawElementsBaseVertex(GL_TRIANGLES, submesh.IndexCount, GL_UNSIGNED_INT, (void*)(sizeof(uint32_t) * submesh.BaseIndex), submesh.BaseVertex);
        });
    }
}

void Renderer::WaitAndRender()
{
    RenderCommand::GetCommandQueue().Execute();
}

const Scope<ShaderLibrary>& Renderer::GetShaderLibrary()
{
    return s_Data.ShaderLibrary;
}

}