#include "abpch.h"
#include "OpenGLShader.h"

#include <fstream>

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Amber/Renderer/RenderCommand.h"

namespace Amber
{

uint32_t OpenGLShader::ShaderTypeFromString(const std::string& type)
{
    if (type == "vertex")
        return GL_VERTEX_SHADER;
    if (type == "fragment" || type == "pixel")
        return GL_FRAGMENT_SHADER;
    if (type == "compute")
        return GL_COMPUTE_SHADER;

    AB_CORE_ASSERT(false, "Invalid shader type!");
    return 0;
}

OpenGLShader::OpenGLShader(const std::string& filepath)
    : m_AssetPath(filepath)
{
    AB_PROFILE_FUNCTION();

    auto loc = filepath.find_first_of("/\\");
    m_Name = loc != std::string::npos ? filepath.substr(loc + 1) : filepath;
    loc = m_Name.rfind(".");
    m_Name = loc != std::string::npos ? m_Name.substr(0, loc) : m_Name;

    Reload();
}

OpenGLShader::OpenGLShader(const std::string& name, const std::string& source)
    : m_Name(name)
{
    AB_PROFILE_FUNCTION();

    Load(source);
}

OpenGLShader::~OpenGLShader()
{
    AB_PROFILE_FUNCTION();

    glDeleteProgram(m_RendererID);
}

void OpenGLShader::Reload()
{
    std::string source = ReadShaderFromFile();
    Load(source);
}

void OpenGLShader::Load(const std::string& source)
{
    PreProcess(source);
    if (!m_IsCompute)
        Parse();

    RenderCommand::Submit([=]()
    {
        if (m_RendererID)
            glDeleteProgram(m_RendererID);

        CompileAndUploadShader();

        if (!m_IsCompute)
            ResolveUniforms();

        glUseProgram(m_RendererID);
    });
}

std::string OpenGLShader::ReadShaderFromFile() const
{
    AB_PROFILE_FUNCTION();

    std::string result;
    std::ifstream in(m_AssetPath, std::ios::in | std::ios::binary);
    if (in)
    {
        in.seekg(0, std::ios::end);
        size_t size = in.tellg();
        if (size != -1)
        {
            result.resize(size);
            in.seekg(0, std::ios::beg);
            in.read(&result[0], size);
        }
        else
        {
            AB_CORE_ASSERT(false, "Shader file could not be read!");
        }
        in.close();
    }
    else
    {
        AB_CORE_ASSERT(false, "Shader file could not be opened!");
    }

    return result;
}

void OpenGLShader::PreProcess(const std::string& source)
{
    AB_PROFILE_FUNCTION();

    const char* typeToken = "#type";
    size_t tokenLength = strlen(typeToken);
    size_t pos = source.find(typeToken, 0);
    while (pos != std::string::npos)
    {
        size_t eol = source.find_first_of("\r\n", pos);
        AB_CORE_ASSERT(eol != std::string::npos, "Shader syntax error!");
        size_t begin = source.find_first_not_of(" ", pos + tokenLength);
        AB_CORE_ASSERT(begin != std::string::npos, "Shader syntax error!");
        std::string type = source.substr(begin, eol - begin);
        
        size_t nextLinePos = source.find_first_not_of("\r\n", eol);
        AB_CORE_ASSERT(nextLinePos != std::string::npos, "Shader syntax error!");
        pos = source.find(typeToken, nextLinePos);

        auto shaderType = ShaderTypeFromString(type);
        m_ShaderSource[shaderType] = pos == std::string::npos ?
            source.substr(nextLinePos) :
            source.substr(nextLinePos, pos - nextLinePos);

        if (shaderType == GL_COMPUTE_SHADER)
        {
            m_IsCompute = true;
            break;
        }
    }
}

static const char* FindToken(const char* str, const std::string& token)
{
    const char* t = str;
    while (t = strstr(t, token.c_str()))
    {
        bool left = str == t || isspace(t[-1]);
        bool right = !t[token.size()] || isspace(t[token.size()]);
        if (left && right)
            return t;

        t += token.size();
    }

    return nullptr;
}

static const char* FindToken(const std::string& str, const std::string& token)
{
    FindToken(str.c_str(), token);
}

static std::vector<std::string> SplitString(const std::string& str, const std::string& delimiters)
{
    size_t start = 0;
    size_t end = str.find_first_of(delimiters);

    std::vector<std::string> result;

    while (true)
    {
        auto token = str.substr(start, end - start);
        if (!token.empty())
            result.push_back(token);

        if (end == std::string::npos)
            break;

        start = end + 1;
        end = str.find_first_of(delimiters, start);
    }

    return result;
}

static std::vector<std::string> SplitString(const std::string& str, const char delimiters)
{
    return SplitString(str, std::string(1, delimiters));
}

static std::vector<std::string> Tokenize(const std::string& str)
{
    return SplitString(str, " \t\n");
}

static std::vector<std::string> GetLines(const std::string& str)
{
    return SplitString(str, "\n");
}

static std::string GetBlock(const char* str, const char** outPosition)
{
    const char* end = strstr(str, "}");
    if (!end)
        return str;

    if (outPosition)
        *outPosition = end;

    return std::string(str, end - str + 1);
}

static std::string GetStatement(const char* str, const char** outPosition)
{
    const char* end = strstr(str, ";");
    if (!end)
        return str;

    if (outPosition)
        *outPosition = end;

    return std::string(str, end - str);
}

static bool StartsWith(const std::string& str, const std::string& start)
{
    return str.substr(0, start.size()) == start;
}

static bool IsTypeStringResource(const std::string& type)
{
    if (type == "sampler2D") return true;
    return false;
}

void OpenGLShader::Parse()
{
    const char* token;
    const char* vstr;
    const char* fstr;

    m_Resources.clear();
    m_Structs.clear();
    m_VSMaterialUniformBuffer.reset();
    m_PSMaterialUniformBuffer.reset();

    auto& vertexSource = m_ShaderSource[GL_VERTEX_SHADER];
    auto& fragmentSource = m_ShaderSource[GL_FRAGMENT_SHADER];

    // Vertex Shader
    vstr = vertexSource.c_str();
    while (token = FindToken(vstr, "struct"))
        ParseUniformStruct(GetBlock(token, &vstr), ShaderDomain::Vertex);

    vstr = vertexSource.c_str();
    while (token = FindToken(vstr, "uniform"))
        ParseUniformStruct(GetStatement(token, &vstr), ShaderDomain::Vertex);

    // Fragment Shader
    fstr = fragmentSource.c_str();
    while (token = FindToken(fstr, "struct"))
        ParseUniformStruct(GetBlock(token, &fstr), ShaderDomain::Vertex);

    fstr = fragmentSource.c_str();
    while (token = FindToken(fstr, "uniform"))
        ParseUniformStruct(GetStatement(token, &fstr), ShaderDomain::Vertex);
}

void OpenGLShader::ParseUniformStruct(const std::string& block, ShaderDomain domain)
{
    auto tokens = Tokenize(block);
    uint32_t index = 0;

    index++;    // struct
    std::string name = tokens[index++];
    index++;    // {

    ShaderUniformStruct* uniformStruct = new ShaderUniformStruct(name);
    while (index < tokens.size())
    {
        std::string typeString = tokens[index++];
        std::string fieldName = tokens[index++];

        if (auto semi = fieldName.find(";"))
            fieldName = fieldName.substr(0, semi);

        uint32_t count = 1;
        if (auto open = fieldName.find("["))
        {
            auto close = fieldName.find("]");
            count = (uint32_t)atoi(fieldName.substr(open + 1, close - open).c_str());
            fieldName = fieldName.substr(0, open);
        }

        ShaderUniform* uniform = new OpenGLShaderUniform(domain, OpenGLShaderUniform::StringToType(typeString), fieldName, count);
        uniformStruct->AddField(uniform);
    }
    m_Structs.push_back(uniformStruct);
}

void OpenGLShader::ParseUniform(const std::string& statement, ShaderDomain domain)
{
    auto tokens = Tokenize(statement);
    uint32_t index = 0;

    index++;    // uniform
    std::string typeString = tokens[index++];
    std::string name = tokens[index++];

    uint32_t count = 1;
    if (auto open = name.find("["))
    {
        auto close = name.find("]");
        count = (uint32_t)atoi(name.substr(open + 1, close - open).c_str());
        name = name.substr(0, open);
    }

    if (IsTypeStringResource(typeString))
    {
        ShaderResource* resource = new OpenGLShaderResource(OpenGLShaderResource::StringToType(typeString), name, count);
        m_Resources.push_back(resource);
    }
    else
    {
        auto type = OpenGLShaderUniform::StringToType(typeString);
        OpenGLShaderUniform* uniform = nullptr;

        if (type == OpenGLShaderUniform::Type::None)
        {
            auto uniformStruct = FindStruct(typeString);
            uniform = new OpenGLShaderUniform(domain, uniformStruct, name, count);
        }
        else
        {
            uniform = new OpenGLShaderUniform(domain, type, name, count);
        }

        if (domain == ShaderDomain::Vertex)
        {
            if (!m_VSMaterialUniformBuffer)
                m_VSMaterialUniformBuffer = CreateScope<OpenGLShaderUniformBuffer>(domain, m_Name + "_vertex");

            m_VSMaterialUniformBuffer->PushUniform(uniform);
        }
        else if (domain == ShaderDomain::Pixel)
        {
            if (!m_PSMaterialUniformBuffer)
                m_PSMaterialUniformBuffer = CreateScope<OpenGLShaderUniformBuffer>(domain, m_Name + "_pixel");

            m_PSMaterialUniformBuffer->PushUniform(uniform);
        }
    }
}

void OpenGLShader::CompileAndUploadShader()
{
    AB_PROFILE_FUNCTION();

    std::vector<uint32_t> shaderIDs;

    uint32_t program = glCreateProgram();
    for (auto& [type, source] : m_ShaderSource)
    {
        const char* src = source.c_str();
        uint32_t shader = glCreateShader(type);
        glShaderSource(shader, 1, &src, nullptr);

        glCompileShader(shader);
        int32_t status;
        glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
        if (status == GL_FALSE)
        {
            int32_t length;
            glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &length);

            std::vector<char> log(length);
            glGetShaderInfoLog(shader, length, &length, &log[0]);


            glDeleteShader(shader);

            AB_CORE_ERROR("Shader failed to compile:\n{0}", log.data());
            AB_CORE_ASSERT(false);
        }

        glAttachShader(program, shader);
        shaderIDs.push_back(shader);
    }

    glLinkProgram(program);
    int32_t status;
    glGetProgramiv(program, GL_LINK_STATUS, &status);
    if (status == GL_FALSE)
    {
        int32_t length;
        glGetProgramiv(program, GL_INFO_LOG_LENGTH, &length);

        std::vector<char> log(length);
        glGetProgramInfoLog(program, length, &length, &log[0]);

        AB_CORE_ERROR("{0}", log.data());
        AB_CORE_ASSERT(false, "OpenGLShader program failed to link!");

        glDeleteProgram(program);
        for (auto id : shaderIDs)
        {
            glDetachShader(program, id);
            glDeleteShader(id);
        }
    }
    else
    {
        for (auto id : shaderIDs)
            glDetachShader(program, id);

        m_RendererID = program;
    }
}

void OpenGLShader::ResolveUniforms()
{
    if (m_VSMaterialUniformBuffer)
    {
        const auto& uniforms = m_VSMaterialUniformBuffer->GetUniforms();
        for (uint32_t i = 0; i < uniforms.size(); i++)
        {
            auto uniform = dynamic_cast<OpenGLShaderUniform*>(uniforms[i]);
            if (uniform->GetType() == OpenGLShaderUniform::Type::Struct)
            {
                const auto& uniformStruct = uniform->GetShaderUniformStruct();
                const auto& fields = uniformStruct.GetFields();
                for (uint32_t j = 0; j < fields.size(); j++)
                {
                    auto field = dynamic_cast<OpenGLShaderUniform*>(fields[j]);
                    field->SetLocation(GetUniformLocation(uniform->GetName() + "." + field->GetName()));
                }
            }
            else
            {
                uniform->SetLocation(GetUniformLocation(uniform->GetName()));
            }
        }
    }

    if (m_PSMaterialUniformBuffer)
    {
        const auto& uniforms = m_PSMaterialUniformBuffer->GetUniforms();
        for (uint32_t i = 0; i < uniforms.size(); i++)
        {
            auto uniform = dynamic_cast<OpenGLShaderUniform*>(uniforms[i]);
            if (uniform->GetType() == OpenGLShaderUniform::Type::Struct)
            {
                const auto& uniformStruct = uniform->GetShaderUniformStruct();
                const auto& fields = uniformStruct.GetFields();
                for (uint32_t j = 0; j < fields.size(); j++)
                {
                    auto field = dynamic_cast<OpenGLShaderUniform*>(fields[j]);
                    field->SetLocation(GetUniformLocation(uniform->GetName() + "." + field->GetName()));
                }
            }
            else
            {
                uniform->SetLocation(GetUniformLocation(uniform->GetName()));
            }
        }
    }

    uint32_t sampler = 0;
    for (uint32_t i = 0; i < m_Resources.size(); i++)
    {
        auto resource = dynamic_cast<OpenGLShaderResource*>(m_Resources[i]);
        int32_t location = GetUniformLocation(resource->GetName());

        if (resource->GetCount() == 1)
        {
            resource->SetRegister(sampler);
            if (location != -1)
                UploadUniformInt(location, sampler);

            sampler++;
        }
        else
        {
            resource->SetRegister(0);

            uint32_t count = resource->GetCount();
            int32_t* samplers = new int32_t[count];
            for (uint32_t j = 0; j < count; j++)
                samplers[j] = j;

            UploadUniformIntArray(location, samplers, count);
            delete[] samplers;
        }
    }
}

void OpenGLShader::ResolveAndSetUniforms(const Scope<OpenGLShaderUniformBuffer>& uniformBuffer, Buffer buffer)
{
    const auto& uniforms = uniformBuffer->GetUniforms();
    for (uint32_t i = 0; i < uniforms.size(); i++)
    {
        auto uniform = dynamic_cast<OpenGLShaderUniform*>(uniforms[i]);
        if (uniform->IsArray())
            ResolveAndSetUniformArray(uniform, buffer);
        else
            ResolveAndSetUniform(uniform, buffer);
    }
}

void OpenGLShader::ResolveAndSetUniform(OpenGLShaderUniform* uniform, Buffer buffer)
{
    if (uniform->GetLocation() == -1)
    {
        AB_CORE_ERROR("Uniform {0} has invalid location", uniform->GetName());
        return;
    }

    uint32_t offset = uniform->GetOffset();
    switch (uniform->GetType())
    {
        case OpenGLShaderUniform::Type::Int32:
            UploadUniformInt(uniform->GetLocation(), *(int32_t*)buffer.Data[offset]);
            break;
        case OpenGLShaderUniform::Type::Float32:
            UploadUniformFloat(uniform->GetLocation(), *(float*)buffer.Data[offset]);
            break;
        case OpenGLShaderUniform::Type::Vec2:
            UploadUniformFloat2(uniform->GetLocation(), *(glm::vec2*)buffer.Data[offset]);
            break;
        case OpenGLShaderUniform::Type::Vec3:
            UploadUniformFloat3(uniform->GetLocation(), *(glm::vec3*)buffer.Data[offset]);
            break;
        case OpenGLShaderUniform::Type::Vec4:
            UploadUniformFloat4(uniform->GetLocation(), *(glm::vec4*)buffer.Data[offset]);
            break;
        case OpenGLShaderUniform::Type::Mat3:
            UploadUniformMat3(uniform->GetLocation(), *(glm::mat3*)buffer.Data[offset]);
            break;
        case OpenGLShaderUniform::Type::Mat4:
            UploadUniformMat4(uniform->GetLocation(), *(glm::mat4*)buffer.Data[offset]);
            break;
        case OpenGLShaderUniform::Type::Struct:
            UploadUniformStruct(uniform, buffer.Data, offset);
            break;
        default:
            AB_CORE_ASSERT(false, "Unknown uniform type!");
    }
}

void OpenGLShader::ResolveAndSetUniformArray(OpenGLShaderUniform* uniform, Buffer buffer)
{
    if (uniform->GetLocation() == -1)
    {
        AB_CORE_ERROR("Uniform {0} has invalid location", uniform->GetName());
        return;
    }

    uint32_t offset = uniform->GetOffset();
    switch (uniform->GetType())
    {
        case OpenGLShaderUniform::Type::Int32:
            UploadUniformIntArray(uniform->GetLocation(), (int32_t*)buffer.Data[offset], uniform->GetCount());
            break;
        case OpenGLShaderUniform::Type::Mat4:
            UploadUniformMat4Array(uniform->GetLocation(), *(glm::mat4*)buffer.Data[offset], uniform->GetCount());
            break;
        default:
            AB_CORE_ASSERT(false, "Unknown uniform type!");
    }
}

void OpenGLShader::ResolveAndSetUniformField(const OpenGLShaderUniform& field, byte* data, uint32_t offset)
{
    if (field.GetLocation() == -1)
    {
        AB_CORE_ERROR("Uniform {0} has invalid location", field.GetName());
        return;
    }

    switch (field.GetType())
    {
        case OpenGLShaderUniform::Type::Int32:
            UploadUniformInt(field.GetLocation(), *(int32_t*)data[offset]);
            break;
        case OpenGLShaderUniform::Type::Float32:
            UploadUniformFloat(field.GetLocation(), *(float*)data[offset]);
            break;
        case OpenGLShaderUniform::Type::Vec2:
            UploadUniformFloat2(field.GetLocation(), *(glm::vec2*)data[offset]);
            break;
        case OpenGLShaderUniform::Type::Vec3:
            UploadUniformFloat3(field.GetLocation(), *(glm::vec3*)data[offset]);
            break;
        case OpenGLShaderUniform::Type::Vec4:
            UploadUniformFloat4(field.GetLocation(), *(glm::vec4*)data[offset]);
            break;
        case OpenGLShaderUniform::Type::Mat3:
            UploadUniformMat3(field.GetLocation(), *(glm::mat3*)data[offset]);
            break;
        case OpenGLShaderUniform::Type::Mat4:
            UploadUniformMat4(field.GetLocation(), *(glm::mat4*)data[offset]);
            break;
        default:
            AB_CORE_ASSERT(false, "Unknown uniform type!");
    }
}

int32_t OpenGLShader::GetUniformLocation(const std::string& name) const
{
    int32_t result = glGetUniformLocation(m_RendererID, name.c_str());
    if (result == -1)
        AB_CORE_WARN("Could not find uniform {0} in shader.", name);

    return result;
}

ShaderUniformStruct* OpenGLShader::FindStruct(const std::string& name)
{
    for (auto uniformStruct : m_Structs)
    {
        if (uniformStruct->GetName() == name)
            return uniformStruct;
    }
    return nullptr;
}

void OpenGLShader::UploadUniformInt(uint32_t location, int32_t value)
{
    glUniform1i(location, value);
}

void OpenGLShader::UploadUniformIntArray(uint32_t location, int32_t* values, uint32_t count)
{
    glUniform1iv(location, count, values);
}

void OpenGLShader::UploadUniformFloat(uint32_t location, float value)
{
    glUniform1f(location, value);
}

void OpenGLShader::UploadUniformFloat2(uint32_t location, const glm::vec2& value)
{
    glUniform2f(location, value.x, value.y);
}

void OpenGLShader::UploadUniformFloat3(uint32_t location, const glm::vec3& value)
{
    glUniform3f(location, value.x, value.y, value.z);
}

void OpenGLShader::UploadUniformFloat4(uint32_t location, const glm::vec4& value)
{
    glUniform4f(location, value.x, value.y, value.z, value.w);
}

void OpenGLShader::UploadUniformMat3(uint32_t location, const glm::mat3& value)
{
    glUniformMatrix3fv(location, 1, GL_FALSE, glm::value_ptr(value));
}

void OpenGLShader::UploadUniformMat4(uint32_t location, const glm::mat4& value)
{
    glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(value));
}

void OpenGLShader::UploadUniformMat4Array(uint32_t location, const glm::mat4& value, uint32_t count)
{
    glUniformMatrix4fv(location, count, GL_FALSE, glm::value_ptr(value));
}

void OpenGLShader::UploadUniformStruct(OpenGLShaderUniform* uniform, byte* buffer, uint32_t offset)
{
    const auto& uniformStruct = uniform->GetShaderUniformStruct();
    const auto& fields = uniformStruct.GetFields();
    for (uint32_t i = 0; i < fields.size(); i++)
    {
        auto field = dynamic_cast<OpenGLShaderUniform*>(fields[i]);
        ResolveAndSetUniformField(*field, buffer, offset);
        offset += field->GetSize();
    }
}

void OpenGLShader::Bind() const
{
    AB_PROFILE_FUNCTION();

    RenderCommand::Submit([=]()
    {
        glUseProgram(m_RendererID);
    });
}

void OpenGLShader::Unbind() const
{
    AB_PROFILE_FUNCTION();

    RenderCommand::Submit([=]()
    {
        glUseProgram(0);
    });
}

void OpenGLShader::SetVSMaterialUniformBuffer(Buffer buffer)
{
    RenderCommand::Submit([this, buffer]
        {
            glUseProgram(m_RendererID);
            ResolveAndSetUniforms(m_VSMaterialUniformBuffer, buffer);
        });
}

void OpenGLShader::SetPSMaterialUniformBuffer(Buffer buffer)
{
    RenderCommand::Submit([this, buffer]
        {
            glUseProgram(m_RendererID);
            ResolveAndSetUniforms(m_PSMaterialUniformBuffer, buffer);
        });
}













void OpenGLShader::SetInt(const std::string& name, int32_t value)
{
    AB_PROFILE_FUNCTION();

    UploadUniformInt(name, value);
}

void OpenGLShader::SetIntArray(const std::string& name, int32_t* values, uint32_t count)
{
    AB_PROFILE_FUNCTION();

    UploadUniformIntArray(name, values, count);
}

void OpenGLShader::SetFloat(const std::string& name, float value)
{
    AB_PROFILE_FUNCTION();

    UploadUniformFloat(name, value);
}

void OpenGLShader::SetFloat3(const std::string& name, const glm::vec3& value)
{
    AB_PROFILE_FUNCTION();

    UploadUniformFloat3(name, value);
}

void OpenGLShader::SetFloat4(const std::string& name, const glm::vec4& value)
{
    AB_PROFILE_FUNCTION();

    UploadUniformFloat4(name, value);
}

void OpenGLShader::SetMat4(const std::string& name, const glm::mat4& value)
{
    AB_PROFILE_FUNCTION();

    UploadUniformMat4(name, value);
}

void OpenGLShader::UploadUniformInt(const std::string& name, int32_t value)
{
    RenderCommand::Submit([=]()
        {
            if (m_LocationMap.find(name) == m_LocationMap.end())
                m_LocationMap[name] = glGetUniformLocation(m_RendererID, name.c_str());

            int32_t location = m_LocationMap[name];
            if (location == -1)
                AB_CORE_ERROR("Uniform {0} not found!", name);
            else
                glUniform1i(location, value);
        });
}

void OpenGLShader::UploadUniformIntArray(const std::string& name, int32_t* values, uint32_t count)
{
    RenderCommand::Submit([=]()
        {
            int32_t samplers[32];
            for (uint32_t i = 0; i < 32; i++)
                samplers[i] = i;

            if (m_LocationMap.find(name) == m_LocationMap.end())
                m_LocationMap[name] = glGetUniformLocation(m_RendererID, name.c_str());

            int32_t location = m_LocationMap[name];
            if (location == -1)
                AB_CORE_ERROR("Uniform {0} not found!", name);
            else
                glUniform1iv(location, count, samplers);
        });
}

void OpenGLShader::UploadUniformFloat(const std::string& name, float value)
{
    RenderCommand::Submit([=]()
        {
            if (m_LocationMap.find(name) == m_LocationMap.end())
                m_LocationMap[name] = glGetUniformLocation(m_RendererID, name.c_str());

            int32_t location = m_LocationMap[name];
            if (location == -1)
                AB_CORE_ERROR("Uniform {0} not found!", name);
            else
                glUniform1f(location, value);
        });
}

void OpenGLShader::UploadUniformFloat2(const std::string& name, const glm::vec2& values)
{
    RenderCommand::Submit([=]()
        {
            if (m_LocationMap.find(name) == m_LocationMap.end())
                m_LocationMap[name] = glGetUniformLocation(m_RendererID, name.c_str());

            int32_t location = m_LocationMap[name];
            if (location == -1)
                AB_CORE_ERROR("Uniform {0} not found!", name);
            else
                glUniform2f(location, values.x, values.y);
        });
}

void OpenGLShader::UploadUniformFloat3(const std::string& name, const glm::vec3& values)
{
    RenderCommand::Submit([=]()
        {
            if (m_LocationMap.find(name) == m_LocationMap.end())
                m_LocationMap[name] = glGetUniformLocation(m_RendererID, name.c_str());

            int32_t location = m_LocationMap[name];
            if (location == -1)
                AB_CORE_ERROR("Uniform {0} not found!", name);
            else
                glUniform3f(location, values.x, values.y, values.z);
        });
}

void OpenGLShader::UploadUniformFloat4(const std::string& name, const glm::vec4& values)
{
    RenderCommand::Submit([=]()
        {
            if (m_LocationMap.find(name) == m_LocationMap.end())
                m_LocationMap[name] = glGetUniformLocation(m_RendererID, name.c_str());

            int32_t location = m_LocationMap[name];
            if (location == -1)
                AB_CORE_ERROR("Uniform {0} not found!", name);
            else
                glUniform4f(location, values.x, values.y, values.z, values.w);
        });
}

void OpenGLShader::UploadUniformMat3(const std::string& name, const glm::mat3& matrix)
{
    RenderCommand::Submit([=]()
        {
            if (m_LocationMap.find(name) == m_LocationMap.end())
                m_LocationMap[name] = glGetUniformLocation(m_RendererID, name.c_str());

            int32_t location = m_LocationMap[name];
            if (location == -1)
                AB_CORE_ERROR("Uniform {0} not found!", name);
            else
                glUniformMatrix3fv(location, 1, GL_FALSE, glm::value_ptr(matrix));
        });
}

void OpenGLShader::UploadUniformMat4(const std::string& name, const glm::mat4& matrix)
{
    RenderCommand::Submit([=]()
        {
            if (m_LocationMap.find(name) == m_LocationMap.end())
                m_LocationMap[name] = glGetUniformLocation(m_RendererID, name.c_str());

            int32_t location = m_LocationMap[name];
            if (location == -1)
                AB_CORE_ERROR("Uniform {0} not found!", name);
            else
                glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(matrix));
        });
}

}