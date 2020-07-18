#include "abpch.h"
#include "SceneRenderer.h"

#include <glad/glad.h>

#include "Amber/Renderer/Camera.h"
#include "Amber/Renderer/Framebuffer.h"
#include "Amber/Renderer/RenderCommand.h"
#include "Amber/Renderer/Renderer.h"
#include "Amber/Renderer/Renderer2D.h"
#include "Amber/Renderer/Shader.h"

namespace Amber
{

struct SceneRendererData
{
    Scope<ShaderLibrary> ShaderLibrary;

    Scene* ActiveScene = nullptr;
    struct SceneInfo
    {
        SceneRendererCamera SceneCamera;
        Entity CameraEntity;

        Ref<MaterialInstance> SkyboxMaterial;
        Environment SceneEnvironment;
        Light ActiveLight;
    } SceneData;

    Ref<RenderPass> GeometryPass;
    Ref<RenderPass> CompositePass;

    Ref<Texture2D> BRDFLUT;

    struct MeshDrawCommand
    {
        Ref<Mesh> Mesh;
        Ref<MaterialInstance> Material;
        glm::mat4 Transform;
    };
    std::vector<MeshDrawCommand> DrawList;
    std::vector<MeshDrawCommand> SelectedDrawList;

    struct CameraDrawCommand
    {
        SceneCamera Camera;
        glm::mat4 Transform;
    };
    std::vector<CameraDrawCommand> CameraDrawList;

    Ref<Material> CompositeBaseMaterial;
    Ref<MaterialInstance> GridMaterial;
    Ref<MaterialInstance> OutlineMaterial;
    Ref<MaterialInstance> OutlineAnimatedMaterial;

    SceneRendererOptions Options;
};

static SceneRendererData s_Data;

void SceneRenderer::Init()
{
    s_Data.ShaderLibrary = CreateScope<ShaderLibrary>();

    s_Data.ShaderLibrary->Load("assets/shaders/EnvironmentIrradiance.glsl");
    s_Data.ShaderLibrary->Load("assets/shaders/EnvironmentMipFilter.glsl");
    s_Data.ShaderLibrary->Load("assets/shaders/EquirectangularToCubemap.glsl");
    s_Data.ShaderLibrary->Load("assets/shaders/Grid.glsl");
    s_Data.ShaderLibrary->Load("assets/shaders/Outline.glsl");
    s_Data.ShaderLibrary->Load("assets/shaders/Outline_Animated.glsl");
    s_Data.ShaderLibrary->Load("assets/shaders/SceneComposite.glsl");

    FramebufferSpecification geoFramebufferSpec;
    geoFramebufferSpec.Width = 1280;
    geoFramebufferSpec.Height = 720;
    geoFramebufferSpec.Format = FramebufferFormat::RGBA16F;
    geoFramebufferSpec.ClearColor = { 0.1f, 0.1f, 0.1f, 1.0f };
    geoFramebufferSpec.Samples = 8;

    RenderPassSpecification geoRenderPassSpec;
    geoRenderPassSpec.TargetFramebuffer = Framebuffer::Create(geoFramebufferSpec);
    s_Data.GeometryPass = RenderPass::Create(geoRenderPassSpec);

    FramebufferSpecification compFramebufferSpec;
    compFramebufferSpec.Width = 1280;
    compFramebufferSpec.Height = 720;
    compFramebufferSpec.Format = FramebufferFormat::RGBA16F;
    compFramebufferSpec.ClearColor = { 0.5f, 0.1f, 0.1f, 1.0f };

    RenderPassSpecification compRenderPassSpec;
    compRenderPassSpec.TargetFramebuffer = Framebuffer::Create(compFramebufferSpec);
    s_Data.CompositePass = RenderPass::Create(compRenderPassSpec);

    s_Data.BRDFLUT = Texture2D::Create("assets/textures/BRDF_LUT.tga");

    s_Data.CompositeBaseMaterial = Ref<Material>::Create(s_Data.ShaderLibrary->Get("SceneComposite"));
    s_Data.GridMaterial = Ref<MaterialInstance>::Create(Ref<Material>::Create(s_Data.ShaderLibrary->Get("Grid")));

    s_Data.OutlineMaterial = Ref<MaterialInstance>::Create(Ref<Material>::Create(s_Data.ShaderLibrary->Get("Outline")));
    s_Data.OutlineMaterial->SetFlag(MaterialFlag::DepthTest, false);

    s_Data.OutlineAnimatedMaterial = Ref<MaterialInstance>::Create(Ref<Material>::Create(s_Data.ShaderLibrary->Get("Outline_Animated")));
    s_Data.OutlineAnimatedMaterial->SetFlag(MaterialFlag::DepthTest, false);
}

void SceneRenderer::SetViewportSize(uint32_t width, uint32_t height)
{
    s_Data.GeometryPass->GetSpecification().TargetFramebuffer->Resize(width, height);
    s_Data.CompositePass->GetSpecification().TargetFramebuffer->Resize(width, height);
}

void SceneRenderer::BeginScene(Scene* scene, const SceneRendererCamera& camera)
{
    AB_CORE_ASSERT(!s_Data.ActiveScene, "Another scene is still active!");

    s_Data.ActiveScene = scene;

    s_Data.SceneData.CameraEntity = s_Data.ActiveScene->GetMainCameraEntity();
    s_Data.SceneData.SceneCamera = camera;
    s_Data.SceneData.SkyboxMaterial = scene->GetSkyboxMaterial();
    s_Data.SceneData.SceneEnvironment = scene->GetEnvironment();
    s_Data.SceneData.ActiveLight = scene->GetLight();
}

void SceneRenderer::EndScene()
{
    AB_CORE_ASSERT(s_Data.ActiveScene, "No active scene!");
 
    FlushDrawList();

    s_Data.ActiveScene = nullptr;
}

void SceneRenderer::SubmitCamera(const SceneCamera& camera, const glm::mat4& transform)
{
    s_Data.CameraDrawList.push_back({ camera, transform });
}

void SceneRenderer::SubmitMesh(const Ref<Mesh> mesh, const glm::mat4& transform, const Ref<MaterialInstance> overrideMaterial)
{
    s_Data.DrawList.push_back({ mesh, overrideMaterial, transform });
}

void SceneRenderer::SubmitSelectedMesh(const Ref<Mesh> mesh, const glm::mat4& transform)
{
    s_Data.SelectedDrawList.push_back({ mesh, nullptr, transform });
}

void SceneRenderer::GeometryPass()
{
    Renderer::BeginRenderPass(s_Data.GeometryPass);
    RenderCommand::SetStencilMask(0);

    auto viewProj = s_Data.SceneData.SceneCamera.Camera.GetProjectionMatrix() * s_Data.SceneData.SceneCamera.ViewMatrix;
    glm::vec3 cameraPosition = glm::inverse(s_Data.SceneData.SceneCamera.ViewMatrix)[3];
    
    // Skybox
    s_Data.SceneData.SkyboxMaterial->Set("u_InverseVP", glm::inverse(viewProj));
    s_Data.SceneData.SkyboxMaterial->Set("u_Rotation", s_Data.SceneData.SceneEnvironment.Rotation);
    Renderer::DrawFullscreenQuad(s_Data.SceneData.SkyboxMaterial);

    // Render entities
    for (auto& drawCommand : s_Data.DrawList)
    {
        auto baseMaterial = drawCommand.Mesh->GetMaterial();
        auto shaderType = baseMaterial->GetShader()->GetType();

        baseMaterial->Set("u_ViewProjection", viewProj);
        if (shaderType == ShaderType::StandardStatic || shaderType == ShaderType::StandardAnimated)
        {
            baseMaterial->Set("u_ViewPosition", cameraPosition);

            baseMaterial->Set("u_IrradianceTexture", s_Data.SceneData.SceneEnvironment.IrradianceMap);
            baseMaterial->Set("u_RadianceTexture", s_Data.SceneData.SceneEnvironment.RadianceMap);
            baseMaterial->Set("u_BRDFLUT", s_Data.BRDFLUT);
            baseMaterial->Set("u_EnvironmentRotation", s_Data.SceneData.SceneEnvironment.Rotation);

            struct LightUniform
            {
                glm::vec3 Radiance;
                float Multiplier;
            };
            LightUniform light{ s_Data.SceneData.ActiveLight.Radiance, s_Data.SceneData.ActiveLight.Multiplier };

            baseMaterial->Set("u_LightDirection", s_Data.SceneData.ActiveLight.Direction);
            baseMaterial->Set("u_Light", light);
        }

        Renderer::DrawMesh(drawCommand.Mesh, drawCommand.Transform, drawCommand.Material);
    }

    if (!s_Data.SelectedDrawList.empty())
    {
        RenderCommand::SetStencilFunction(ComparisonFunc::Always, 1, 0xff);
        RenderCommand::SetStencilMask(0xff);
        RenderCommand::SetStencilOperation(StencilOperation::Keep, StencilOperation::Keep, StencilOperation::Replace);

        for (auto& drawCommand : s_Data.SelectedDrawList)
        {
            auto baseMaterial = drawCommand.Mesh->GetMaterial();
            auto shaderType = baseMaterial->GetShader()->GetType();

            baseMaterial->Set("u_ViewProjection", viewProj);
            if (shaderType == ShaderType::StandardStatic || shaderType == ShaderType::StandardAnimated)
            {
                baseMaterial->Set("u_ViewPosition", cameraPosition);

                baseMaterial->Set("u_IrradianceTexture", s_Data.SceneData.SceneEnvironment.IrradianceMap);
                baseMaterial->Set("u_RadianceTexture", s_Data.SceneData.SceneEnvironment.RadianceMap);
                baseMaterial->Set("u_BRDFLUT", s_Data.BRDFLUT);
                baseMaterial->Set("u_EnvironmentRotation", s_Data.SceneData.SceneEnvironment.Rotation);

                struct LightUniform
                {
                    glm::vec3 Radiance;
                    float Multiplier;
                };
                LightUniform light{ s_Data.SceneData.ActiveLight.Radiance, s_Data.SceneData.ActiveLight.Multiplier };

                baseMaterial->Set("u_LightDirection", s_Data.SceneData.ActiveLight.Direction);
                baseMaterial->Set("u_Light", light);
            }

            Renderer::DrawMesh(drawCommand.Mesh, drawCommand.Transform, drawCommand.Material);
        }

        if (!s_Data.Options.ShowBoundingBoxes)
        {
            s_Data.OutlineMaterial->Set("u_ViewProjection", viewProj);
            s_Data.OutlineMaterial->Set("u_Color", glm::vec3(1.0f, 0.2f, 0.0f));
            s_Data.OutlineAnimatedMaterial->Set("u_ViewProjection", viewProj);
            s_Data.OutlineAnimatedMaterial->Set("u_Color", glm::vec3(1.0f, 0.2f, 0.0f));

            RenderCommand::SetStencilFunction(ComparisonFunc::NotEqual, 1, 0xff);
            RenderCommand::SetStencilMask(0);

            RenderCommand::SetLineThickness(7.0f);
            RenderCommand::SetRasterizationMode(RasterizationMode::Line);

            for (auto& drawCommand : s_Data.SelectedDrawList)
                Renderer::DrawMesh(drawCommand.Mesh, drawCommand.Transform, drawCommand.Mesh->IsAnimated() ? s_Data.OutlineAnimatedMaterial : s_Data.OutlineMaterial);

            RenderCommand::SetPointSize(7.0f);
            RenderCommand::SetRasterizationMode(RasterizationMode::Point);

            for (auto& drawCommand : s_Data.SelectedDrawList)
                Renderer::DrawMesh(drawCommand.Mesh, drawCommand.Transform, drawCommand.Mesh->IsAnimated() ? s_Data.OutlineAnimatedMaterial : s_Data.OutlineMaterial);

            RenderCommand::SetStencilFunction(ComparisonFunc::Always, 1, 0xff);
            RenderCommand::SetStencilMask(0xff);
            RenderCommand::SetRasterizationMode(RasterizationMode::Fill);
        }
    }

    Renderer2D::BeginScene(viewProj);

    if (s_Data.Options.ShowCamera)
    {
        RenderCommand::SetLineThickness(1.0f);

        for (auto& drawCommand : s_Data.CameraDrawList)
        {
            auto data = drawCommand.Camera.GetPerspectiveData();
            Math::Frustum cameraFrustum(drawCommand.Transform, data.FOV, data.Near, data.Far, data.Width / data.Height);
            Renderer::DrawFrustum(cameraFrustum);
        }
    }

    if (s_Data.Options.ShowBoundingBoxes)
    {
        RenderCommand::SetLineThickness(2.0f);

        for (auto& drawCommand : s_Data.DrawList)
            Renderer::DrawAABB(drawCommand.Mesh, drawCommand.Transform, glm::vec4(1.0f));

        for (auto& drawCommand : s_Data.SelectedDrawList)
            Renderer::DrawAABB(drawCommand.Mesh, drawCommand.Transform, glm::vec4(0.0f, 1.0f, 0.0f, 1.0f));
    }

    if (s_Data.Options.ShowGrid)
    {
        s_Data.GridMaterial->Set("u_ViewProjection", viewProj);
        s_Data.GridMaterial->Set("u_Resolution", s_Data.Options.GridResolution);
        s_Data.GridMaterial->Set("u_Scale", s_Data.Options.GridScale);

        glm::mat4 gridTransform(1.0f);
        gridTransform = glm::rotate(gridTransform, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
        gridTransform = glm::scale(gridTransform, glm::vec3(s_Data.Options.GridSize * 0.5f));   // DrawQuad uses fullscreen quad, which is 2x2
        Renderer2D::DrawQuad(s_Data.GridMaterial, gridTransform);
    }

    Renderer2D::EndScene();
    Renderer::EndRenderPass();
}

void SceneRenderer::CompositePass()
{
    Renderer::BeginRenderPass(s_Data.CompositePass);

    auto material = Ref<MaterialInstance>::Create(s_Data.CompositeBaseMaterial);
    material->Set("u_Exposure", s_Data.SceneData.SceneCamera.Camera.GetExposure());
    material->Set("u_Texture", s_Data.GeometryPass->GetSpecification().TargetFramebuffer->GetColorAttachments()[0]);
    material->Set("u_TextureSamples", s_Data.GeometryPass->GetSpecification().TargetFramebuffer->GetSpecification().Samples);

    Renderer::DrawFullscreenQuad(material);

    Renderer::EndRenderPass();
}

void SceneRenderer::FlushDrawList()
{
    GeometryPass();
    CompositePass();

    s_Data.DrawList.clear();
    s_Data.SelectedDrawList.clear();
    s_Data.CameraDrawList.clear();
    s_Data.SceneData = {};
}

std::pair<Ref<TextureCube>, Ref<TextureCube>> SceneRenderer::CreateEnvironmentMap(const std::string& filepath)
{
    const uint32_t cubemapSize = 2048;
    const uint32_t irradianceSize = 32;

    // Equirectangular to cubemap
    Ref<Texture2D> equiTexture = Texture2D::Create(filepath, false, false);
    AB_CORE_ASSERT(equiTexture->GetFormat() == TextureFormat::Float16, "Equirectangular texture is not HDR!");
    Ref<TextureCube> cubemapTexture = TextureCube::Create(TextureFormat::Float16, cubemapSize, cubemapSize);

    s_Data.ShaderLibrary->Get("EquirectangularToCubemap")->Bind();
    equiTexture->Bind();
    RenderCommand::Submit([equiTexture, cubemapTexture, cubemapSize]() {
        glBindImageTexture(0, cubemapTexture->GetRendererID(), 0, GL_TRUE, 0, GL_WRITE_ONLY, GL_RGBA16F);
        glDispatchCompute(cubemapSize / 32, cubemapSize / 32, 6);
        glGenerateTextureMipmap(cubemapTexture->GetRendererID());
    });

    // Irradiance texture
    Ref<TextureCube> irradianceTexture = TextureCube::Create(TextureFormat::Float16, irradianceSize, irradianceSize);

    s_Data.ShaderLibrary->Get("EnvironmentIrradiance")->Bind();
    cubemapTexture->Bind();
    RenderCommand::Submit([irradianceTexture, cubemapTexture, irradianceSize]() {
        glBindImageTexture(0, irradianceTexture->GetRendererID(), 0, GL_TRUE, 0, GL_WRITE_ONLY, GL_RGBA16F);
        glDispatchCompute(irradianceSize / 32, irradianceSize / 32, 6);
        glGenerateTextureMipmap(irradianceTexture->GetRendererID());
    });

    // Prefiltered environment (radiance) texture
    Ref<TextureCube> radianceTexture = TextureCube::Create(TextureFormat::Float16, cubemapSize, cubemapSize);
    RenderCommand::Submit([radianceTexture, cubemapTexture]() {
        glCopyImageSubData(
            cubemapTexture->GetRendererID(), GL_TEXTURE_CUBE_MAP, 0, 0, 0, 0,
            radianceTexture->GetRendererID(), GL_TEXTURE_CUBE_MAP, 0, 0, 0, 0,
            cubemapTexture->GetWidth(), cubemapTexture->GetHeight(), 6);
    });

    auto filteringShader = s_Data.ShaderLibrary->Get("EnvironmentMipFilter");
    filteringShader->Bind();
    cubemapTexture->Bind();
    RenderCommand::Submit([radianceTexture, cubemapTexture, cubemapSize, filteringShader]() {
        const uint32_t mipCount = radianceTexture->GetMipLevelCount();
        const float deltaRoughness = 1.0f / glm::max((float)mipCount - 1.0f, 1.0f);
        for (uint32_t level = 1, size = cubemapSize / 2; level < mipCount; level++, size /= 2)
        {
            uint32_t numGroups = glm::max((uint32_t)1, size / 32);

            glBindImageTexture(0, radianceTexture->GetRendererID(), level, GL_TRUE, 0, GL_WRITE_ONLY, GL_RGBA16F);
            glProgramUniform1f(filteringShader->GetRendererID(), 0, level * deltaRoughness);
            glDispatchCompute(numGroups, numGroups, 6);
        }
    });

    return { irradianceTexture, radianceTexture };
}

Ref<RenderPass> SceneRenderer::GetFinalRenderPass()
{
    return s_Data.CompositePass;
}

Ref<Texture2D> SceneRenderer::GetFinalColorBuffer()
{
    return s_Data.CompositePass->GetSpecification().TargetFramebuffer->GetColorAttachments()[0];
}

SceneRendererOptions& SceneRenderer::GetOptions()
{
    return s_Data.Options;
}

Scope<ShaderLibrary>& SceneRenderer::GetShaderLibrary()
{
    return s_Data.ShaderLibrary;
}

}