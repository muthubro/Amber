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

    const Scene* ActiveScene = nullptr;
    struct SceneInfo
    {
        Camera SceneCamera;

        Ref<MaterialInstance> SkyboxMaterial;
        Environment SceneEnvironment;
        Light ActiveLight;
    } SceneData;

    Ref<RenderPass> GeometryPass;
    Ref<RenderPass> CompositePass;

    Ref<Material> CompositeBaseMaterial;

    Ref<Texture2D> BRDFLUT;

    struct DrawCommand
    {
        Ref<Mesh> Mesh;
        Ref<MaterialInstance> Material;
        glm::mat4 Transform;
    };
    std::vector<DrawCommand> DrawList;

    Ref<MaterialInstance> GridMaterial;

    SceneRendererOptions Options;
};

static SceneRendererData s_Data;

void SceneRenderer::Init()
{
    s_Data.ShaderLibrary = CreateScope<ShaderLibrary>();

    s_Data.ShaderLibrary->Load("assets/shaders/EnvironmentIrradiance.glsl");
    s_Data.ShaderLibrary->Load("assets/shaders/EnvironmentMipFilter.glsl");
    s_Data.ShaderLibrary->Load("assets/shaders/EquirectangularToCubemap.glsl");
    s_Data.ShaderLibrary->Load("assets/shaders/SceneComposite.glsl");
    s_Data.ShaderLibrary->Load("assets/shaders/Grid.glsl");

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

    s_Data.CompositeBaseMaterial = Ref<Material>::Create(s_Data.ShaderLibrary->Get("SceneComposite"));

    s_Data.BRDFLUT = Texture2D::Create("assets/textures/BRDF_LUT.tga");

    s_Data.GridMaterial = Ref<MaterialInstance>::Create(Ref<Material>::Create(s_Data.ShaderLibrary->Get("Grid")));
}

void SceneRenderer::SetViewportSize(uint32_t width, uint32_t height)
{
    s_Data.GeometryPass->GetSpecification().TargetFramebuffer->Resize(width, height);
    s_Data.CompositePass->GetSpecification().TargetFramebuffer->Resize(width, height);
}

void SceneRenderer::BeginScene(Scene* scene, const Camera& camera)
{
    AB_CORE_ASSERT(!s_Data.ActiveScene, "Another scene is still active!");

    s_Data.ActiveScene = scene;

    s_Data.SceneData.SceneCamera = camera;
    s_Data.SceneData.SkyboxMaterial = scene->GetSkyboxMaterial();
    s_Data.SceneData.SceneEnvironment = scene->GetEnvironment();
    s_Data.SceneData.ActiveLight = scene->GetLight();
}

void SceneRenderer::EndScene()
{
    AB_CORE_ASSERT(s_Data.ActiveScene, "No active scene!");

    s_Data.ActiveScene = nullptr;

    FlushDrawList();
}

void SceneRenderer::SubmitMesh(const Ref<Mesh> mesh, const glm::mat4& transform, const Ref<MaterialInstance> overrideMaterial)
{
    s_Data.DrawList.push_back({ mesh, overrideMaterial, transform });
}

void SceneRenderer::GeometryPass()
{
    Renderer::BeginRenderPass(s_Data.GeometryPass);

    auto& viewProj = s_Data.SceneData.SceneCamera.GetViewProjection();
    
    // Skybox
    s_Data.SceneData.SkyboxMaterial->Set("u_InverseVP", glm::inverse(viewProj));
    Renderer::DrawFullscreenQuad(s_Data.SceneData.SkyboxMaterial);

    // Render entities
    for (auto& drawCommand : s_Data.DrawList)
    {
        auto baseMaterial = drawCommand.Mesh->GetMaterial();

        baseMaterial->Set("u_ViewProjection", viewProj);
        baseMaterial->Set("u_ViewPosition", s_Data.SceneData.SceneCamera.GetPosition());

        baseMaterial->Set("u_IrradianceTexture", s_Data.SceneData.SceneEnvironment.IrradianceMap);
        baseMaterial->Set("u_RadianceTexture", s_Data.SceneData.SceneEnvironment.RadianceMap);
        baseMaterial->Set("u_BRDFLUT", s_Data.BRDFLUT);

        struct LightUniform
        {
            glm::vec3 Radiance;
            float Multiplier;
        };
        LightUniform light{ s_Data.SceneData.ActiveLight.Radiance, s_Data.SceneData.ActiveLight.Multiplier };

        baseMaterial->Set("u_LightDirection", s_Data.SceneData.ActiveLight.Direction);
        baseMaterial->Set("u_Light", light);

        Renderer::DrawMesh(drawCommand.Mesh, drawCommand.Transform, drawCommand.Material);
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

    Renderer::EndRenderPass();
}

void SceneRenderer::CompositePass()
{
    Renderer::BeginRenderPass(s_Data.CompositePass);

    auto material = Ref<MaterialInstance>::Create(s_Data.CompositeBaseMaterial);
    material->Set("u_Exposure", s_Data.SceneData.SceneCamera.GetExposure());
    material->Set("u_Texture", s_Data.GeometryPass->GetSpecification().TargetFramebuffer->GetColorAttachments()[0]);
    material->Set("u_TextureSamples", s_Data.GeometryPass->GetSpecification().TargetFramebuffer->GetSpecification().Samples);

    Renderer::DrawFullscreenQuad(material);

    Renderer::EndRenderPass();

    // For Sandbox3D
    //auto fbo = s_Data.CompositePass->GetSpecification().TargetFramebuffer;
    //Amber::RenderCommand::Submit([fbo] {
    //    glBindFramebuffer(GL_READ_FRAMEBUFFER, fbo->GetRendererID());
    //    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
    //    glBlitFramebuffer(
    //        0, 0, fbo->GetSpecification().Width, fbo->GetSpecification().Height,
    //        0, 0, fbo->GetSpecification().Width, fbo->GetSpecification().Height,
    //        GL_COLOR_BUFFER_BIT, GL_NEAREST);
    //});
}

void SceneRenderer::FlushDrawList()
{
    GeometryPass();
    CompositePass();

    s_Data.DrawList.clear();
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

}