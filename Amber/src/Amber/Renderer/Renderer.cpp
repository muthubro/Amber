#include "abpch.h"
#include "Renderer.h"

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

void Renderer::WaitAndRender()
{
    RenderCommand::GetCommandQueue().Execute();
}

const Scope<ShaderLibrary>& Renderer::GetShaderLibrary()
{
    return s_Data.ShaderLibrary;
}

}