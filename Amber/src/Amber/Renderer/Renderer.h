#pragma once

#include <glm/glm.hpp>

#include "Amber/Renderer/OrthographicCamera.h"
#include "Amber/Renderer/RenderCommand.h"
#include "Amber/Renderer/Shader.h"
#include "Amber/Renderer/VertexArray.h"

namespace Amber 
{

class Renderer 
{
public:
    static void Init();
    static void Shutdown();

    static void BeginScene(OrthographicCamera& camera);
    static void EndScene();

    template<typename FuncT>
    static void Submit(FuncT&& func) 
    { 
        RenderCommand::Submit(std::forward<FuncT>(func));
    }

    static void WaitAndRender();

    static RendererAPI::API GetAPI() { return RendererAPI::GetAPI(); }

private:
    struct SceneData
    {
        glm::mat4 ViewProjectionMatrix;
    };

    static Scope<SceneData> s_SceneData;
};

}