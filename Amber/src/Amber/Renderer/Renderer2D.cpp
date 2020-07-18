#include "abpch.h"
#include "Renderer2D.h"

#include "Amber/Renderer/Material.h"
#include "Amber/Renderer/RenderCommand.h"
#include "Amber/Renderer/Shader.h"
#include "Amber/Renderer/VertexArray.h"

namespace Amber
{

struct QuadVertex
{
    glm::vec3 Position;
    glm::vec4 Color;
    glm::vec2 TexCoord;
    float TexIndex;
    float TilingFactor;
};

struct LineVertex
{
    glm::vec3 Position;
    glm::vec4 Color;
};

struct Renderer2DData
{
    // General
    Scope<ShaderLibrary> ShaderLibrary;
    Renderer2D::Statistics Stats;
    bool ActiveScene = false;

    // Quads
    static const uint32_t MaxQuads = 20000;
    static const uint32_t MaxQuadVertices = MaxQuads * 4;
    static const uint32_t MaxQuadIndices = MaxQuads * 6;

    static const uint32_t MaxTextureSlots = 32;

    Ref<VertexArray> QuadVertexArray;
    Ref<VertexBuffer> QuadVertexBuffer;
    Ref<Texture2D> WhiteTexture;

    std::array<Ref<Texture2D>, MaxTextureSlots> TextureSlots;
    uint32_t TextureSlotIndex = 1;

    uint32_t QuadIndexCount = 0;
    QuadVertex* QuadVertexBufferBase = nullptr;
    QuadVertex* QuadVertexBufferPtr = nullptr;

    Ref<VertexArray> FullscreenQuadVertexArray;

    glm::mat4 QuadVertexPositions;

    Ref<Material> QuadBaseMaterial;
    Ref<MaterialInstance> QuadMaterial;

    // Lines
    static const uint32_t MaxLines = 10000;
    static const uint32_t MaxLineVertices = MaxLines * 2;
    static const uint32_t MaxLineIndices = MaxLines * 2;

    Ref<VertexArray> LineVertexArray;
    Ref<VertexBuffer> LineVertexBuffer;

    uint32_t LineIndexCount = 0;
    LineVertex* LineVertexBufferBase = nullptr;
    LineVertex* LineVertexBufferPtr = nullptr;

    Ref<Material> LineBaseMaterial;
    Ref<MaterialInstance> LineMaterial;
};

static Renderer2DData s_Data;

void Renderer2D::Init()
{
    AB_PROFILE_FUNCTION();

    s_Data.ShaderLibrary = CreateScope<ShaderLibrary>();

    s_Data.ShaderLibrary->Load("assets/shaders/Renderer2D.glsl");
    s_Data.ShaderLibrary->Load("assets/shaders/Line.glsl");

    // Quads
    s_Data.QuadVertexArray = VertexArray::Create();

    s_Data.QuadVertexBuffer = VertexBuffer::Create(s_Data.MaxQuadVertices * sizeof(QuadVertex));
    s_Data.QuadVertexBuffer->SetLayout({
        { ShaderDataType::Float3, "a_Position" },
        { ShaderDataType::Float4, "a_Color" },
        { ShaderDataType::Float2, "a_TexCoord" },
        { ShaderDataType::Float, "a_TexIndex" },
        { ShaderDataType::Float, "a_TilingFactor" }
    });
    s_Data.QuadVertexArray->AddVertexBuffer(s_Data.QuadVertexBuffer);

    uint32_t* quadIndices = new uint32_t[Renderer2DData::MaxQuadIndices];
    for (uint32_t i = 0, offset = 0; i < Renderer2DData::MaxQuadIndices; i += 6, offset += 4)
    {
        quadIndices[i + 0] = offset + 0;
        quadIndices[i + 1] = offset + 1;
        quadIndices[i + 2] = offset + 2;

        quadIndices[i + 3] = offset + 2;
        quadIndices[i + 4] = offset + 3;
        quadIndices[i + 5] = offset + 0;
    }
    Ref<IndexBuffer> quadIndexBuffer = IndexBuffer::Create(quadIndices, Renderer2DData::MaxQuadIndices * sizeof(uint32_t));
    s_Data.QuadVertexArray->SetIndexBuffer(quadIndexBuffer);
    delete[] quadIndices;

    s_Data.WhiteTexture = Texture2D::Create(TextureFormat::RGBA, 1, 1);
    uint32_t whiteTextureData = 0xffffffff;
    s_Data.WhiteTexture->Lock();
    s_Data.WhiteTexture->GetWritableBuffer().Write(&whiteTextureData, sizeof(uint32_t));
    s_Data.WhiteTexture->Unlock();

    s_Data.TextureSlots[0] = s_Data.WhiteTexture;

    s_Data.QuadVertexBufferBase = new QuadVertex[Renderer2DData::MaxQuadVertices];

    s_Data.QuadVertexPositions[0] = { -0.5f, -0.5f, 0.0f, 1.0f };
    s_Data.QuadVertexPositions[1] = { 0.5f, -0.5f, 0.0f, 1.0f };
    s_Data.QuadVertexPositions[2] = { 0.5f,  0.5f, 0.0f, 1.0f };
    s_Data.QuadVertexPositions[3] = { -0.5f,  0.5f, 0.0f, 1.0f };

    s_Data.FullscreenQuadVertexArray = VertexArray::Create();

    float fsQuadData[] = {
        -1.0f, -1.0f, 0.0f, 0.0f,
         1.0f, -1.0f, 1.0f, 0.0f,
         1.0f,  1.0f, 1.0f, 1.0f,
        -1.0f,  1.0f, 0.0f, 1.0f
    };
    Ref<VertexBuffer> fullscreenVBO = VertexBuffer::Create(fsQuadData, sizeof(fsQuadData));
    fullscreenVBO->SetLayout({
        { ShaderDataType::Float2, "a_Position" },
        { ShaderDataType::Float2, "a_TexCoords" }
    });
    s_Data.FullscreenQuadVertexArray->AddVertexBuffer(fullscreenVBO);

    uint32_t fsQuadIndices[] = { 0, 1, 2, 2, 3, 0 };
    Ref<IndexBuffer> fullscreenIBO = IndexBuffer::Create(fsQuadIndices, sizeof(fsQuadIndices));
    s_Data.FullscreenQuadVertexArray->SetIndexBuffer(fullscreenIBO);

    s_Data.QuadBaseMaterial = Ref<Material>::Create(s_Data.ShaderLibrary->Get("Renderer2D"));

    // Lines
    s_Data.LineVertexArray = VertexArray::Create();

    s_Data.LineVertexBuffer = VertexBuffer::Create(Renderer2DData::MaxLineVertices * sizeof(LineVertex));
    s_Data.LineVertexBuffer->SetLayout({
        { ShaderDataType::Float3, "a_Position" },
        { ShaderDataType::Float4, "a_Color" }
    });
    s_Data.LineVertexArray->AddVertexBuffer(s_Data.LineVertexBuffer);

    uint32_t* lineIndices = new uint32_t[Renderer2DData::MaxLineIndices];
    for (uint32_t i = 0; i < Renderer2DData::MaxLineIndices; i++)
        lineIndices[i] = i;

    Ref<IndexBuffer> lineIndexBuffer = IndexBuffer::Create(lineIndices, Renderer2DData::MaxLineIndices * sizeof(uint32_t));
    s_Data.LineVertexArray->SetIndexBuffer(lineIndexBuffer);
    delete[] lineIndices;

    s_Data.LineVertexBufferBase = new LineVertex[Renderer2DData::MaxLineVertices];

    s_Data.LineBaseMaterial = Ref<Material>::Create(s_Data.ShaderLibrary->Get("Line"));
}

void Renderer2D::Shutdown()
{
    AB_PROFILE_FUNCTION();

    delete[] s_Data.QuadVertexBufferBase;
    delete[] s_Data.LineVertexBufferBase;
}

void Renderer2D::BeginScene(const glm::mat4& viewProjection)
{
    AB_PROFILE_FUNCTION();

    AB_CORE_ASSERT(!s_Data.ActiveScene, "A scene is already active!");

    s_Data.ActiveScene = true;

    // Quad
    s_Data.QuadMaterial = Ref<MaterialInstance>::Create(s_Data.QuadBaseMaterial);
    s_Data.QuadMaterial->Set("u_ViewProjection", viewProjection);

    s_Data.QuadIndexCount = 0;
    s_Data.QuadVertexBufferPtr = s_Data.QuadVertexBufferBase;

    s_Data.TextureSlotIndex = 1;

    // Line
    s_Data.LineMaterial = Ref<MaterialInstance>::Create(s_Data.LineBaseMaterial);
    s_Data.LineMaterial->Set("u_ViewProjection", viewProjection);

    s_Data.LineIndexCount = 0;
    s_Data.LineVertexBufferPtr = s_Data.LineVertexBufferBase;
}

void Renderer2D::EndScene()
{
    AB_PROFILE_FUNCTION();

    AB_CORE_ASSERT(s_Data.ActiveScene, "No active scene!");

    FlushQuads();
    FlushLines();

    s_Data.ActiveScene = false;
}

void Renderer2D::FlushQuads()
{
    AB_PROFILE_FUNCTION();

    if (s_Data.QuadIndexCount == 0)
        return;

    s_Data.QuadMaterial->Bind();
    s_Data.QuadVertexArray->Bind();

    uint32_t dataSize = (uint32_t)((uint8_t*)s_Data.QuadVertexBufferPtr - (uint8_t*)s_Data.QuadVertexBufferBase);
    s_Data.QuadVertexBuffer->SetData(s_Data.QuadVertexBufferBase, dataSize);

    for (uint32_t i = 0; i < s_Data.TextureSlotIndex; i++)
        s_Data.TextureSlots[i]->Bind(i);

    RenderCommand::DrawIndexed(s_Data.QuadIndexCount, PrimitiveType::Triangles);
    s_Data.Stats.DrawCalls++;

    s_Data.QuadIndexCount = 0;
    s_Data.QuadVertexBufferPtr = s_Data.QuadVertexBufferBase;
    s_Data.TextureSlotIndex = 1;
}

void Renderer2D::FlushLines()
{
    AB_PROFILE_FUNCTION();

    if (s_Data.LineIndexCount == 0)
        return;

    s_Data.LineMaterial->Bind();
    s_Data.LineVertexArray->Bind();

    uint32_t dataSize = (uint32_t)((uint8_t*)s_Data.LineVertexBufferPtr - (uint8_t*)s_Data.LineVertexBufferBase);
    s_Data.LineVertexBuffer->SetData(s_Data.LineVertexBufferBase, dataSize);

    RenderCommand::DrawIndexed(s_Data.LineIndexCount, PrimitiveType::Lines, false);
    s_Data.Stats.DrawCalls++;

    s_Data.LineIndexCount = 0;
    s_Data.LineVertexBufferPtr = s_Data.LineVertexBufferBase;
}

void Renderer2D::SetQuadVertexData(const glm::vec3& position, const glm::vec4& color, const glm::vec2& texCoord, float textureIndex, float tilingFactor)
{
    AB_PROFILE_FUNCTION();

    s_Data.QuadVertexBufferPtr->Position = position;
    s_Data.QuadVertexBufferPtr->Color = color;
    s_Data.QuadVertexBufferPtr->TexCoord = texCoord;
    s_Data.QuadVertexBufferPtr->TexIndex = textureIndex;
    s_Data.QuadVertexBufferPtr->TilingFactor = tilingFactor;
}

float Renderer2D::GetTextureSlot(const Ref<Texture2D>& texture)
{
    AB_PROFILE_FUNCTION();

    if (!texture) return 0.0f;

    float textureIndex = -1.0f;
    for (uint32_t i = 0; i < s_Data.TextureSlotIndex; i++)
    {
        if (*s_Data.TextureSlots[i] == *texture)
        {
            textureIndex = (float)i;
            break;
        }
    }
    if (textureIndex == -1.0f)
    {
        if (s_Data.TextureSlotIndex >= Renderer2DData::MaxTextureSlots)
            FlushQuads();

        textureIndex = (float)s_Data.TextureSlotIndex;
        s_Data.TextureSlots[s_Data.TextureSlotIndex] = texture;
        s_Data.TextureSlotIndex++;
    }

    return textureIndex;
}

void Renderer2D::DrawQuad(const QuadData& data)
{
    AB_PROFILE_FUNCTION();

    AB_CORE_ASSERT(s_Data.ActiveScene, "No active scene!");

    if (s_Data.QuadIndexCount >= Renderer2DData::MaxQuadIndices)
        FlushQuads();

    float textureIndex = Renderer2D::GetTextureSlot(data.Texture);
    glm::mat4 actualPosition = data.Transform * s_Data.QuadVertexPositions;

    const uint32_t quadVertexCount = 4;
    for (uint8_t i = 0; i < quadVertexCount; i++)
    {
        Renderer2D::SetQuadVertexData(actualPosition[i], data.Color[i], data.TexCoords[i], textureIndex, data.TilingFactor);
        s_Data.QuadVertexBufferPtr++;
    }

    s_Data.QuadIndexCount += 6;
    s_Data.Stats.QuadCount++;
}

void Renderer2D::DrawLine(const glm::vec3& p0, const glm::vec3& p1, const glm::vec4& color)
{
    AB_PROFILE_FUNCTION();

    AB_CORE_ASSERT(s_Data.ActiveScene, "No active scene!");

    if (s_Data.LineIndexCount >= s_Data.MaxLineIndices)
        FlushLines();

    s_Data.LineVertexBufferPtr->Position = p0;
    s_Data.LineVertexBufferPtr->Color = color;
    s_Data.LineVertexBufferPtr++;

    s_Data.LineVertexBufferPtr->Position = p1;
    s_Data.LineVertexBufferPtr->Color = color;
    s_Data.LineVertexBufferPtr++;

    s_Data.LineIndexCount += 2;
    s_Data.Stats.LineCount++;
}

void Renderer2D::DrawQuad(Ref<MaterialInstance> material, const glm::mat4& transform)
{
    bool depthTest = false;
    if (material)
    {
        material->Bind();
        depthTest = material->GetFlag(MaterialFlag::DepthTest);

        material->Set("u_Transform", transform);
    }

    s_Data.FullscreenQuadVertexArray->Bind();
    RenderCommand::DrawIndexed(6, PrimitiveType::Triangles, depthTest);
}

void Renderer2D::DrawFullscreenQuad(Ref<MaterialInstance> material)
{
    bool depthTest = false;
    bool stencilTest = false;
    if (material)
    {
        material->Bind();
        depthTest = material->GetFlag(MaterialFlag::DepthTest);
        stencilTest = material->GetFlag(MaterialFlag::StencilTest);
    }

    s_Data.FullscreenQuadVertexArray->Bind();
    RenderCommand::DrawIndexed(6, PrimitiveType::Triangles, depthTest, stencilTest);
}

void Renderer2D::ResetStats()
{
    memset(&s_Data.Stats, 0, sizeof(Statistics));
}

Renderer2D::Statistics Renderer2D::GetStats()
{
    return s_Data.Stats;
}

// TODO: Do this in a better way (a texture factory of some kind)
Ref<Texture2D> Renderer2D::WhiteTexture()
{
    return s_Data.WhiteTexture;
}

}