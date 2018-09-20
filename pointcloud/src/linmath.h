#pragma once

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <glm/mat3x3.hpp>
#include <glm/mat4x4.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtc/constants.hpp>

constexpr float PI = 3.14159265359f;
constexpr glm::qualifier QUALIFIER = glm::qualifier::packed_lowp;

typedef glm::vec<2, float, QUALIFIER> vec2;
typedef glm::vec<3, float, QUALIFIER> vec3;
typedef glm::vec<4, float, QUALIFIER> vec4;

typedef glm::mat<3, 3, float, QUALIFIER> mat3;
typedef glm::mat<4, 4, float, QUALIFIER> mat4;
typedef glm::quat quat;
