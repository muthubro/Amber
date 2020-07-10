#include "abpch.h"
#include "Transforms.h"

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/matrix_decompose.hpp>

namespace Amber
{
namespace Math
{

std::tuple<glm::vec3, glm::quat, glm::vec3> DecomposeTransform(const glm::mat4& transform)
{
    glm::vec3 translationVector, skew, scaleVector;
    glm::vec4 perspective;
    glm::quat rotationQuaternion;

    glm::decompose(transform, scaleVector, rotationQuaternion, translationVector, skew, perspective);

    return { translationVector, rotationQuaternion, scaleVector };
}

} // Math
} // Amber