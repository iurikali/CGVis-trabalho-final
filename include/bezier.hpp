#pragma once
#include <glm/mat4x4.hpp>
#include <glm/vec3.hpp>
#include <glad/glad.h>
#include <vector>

glm::vec3 vec3Lerp(float t, glm::vec3 a, glm::vec3 b);

glm::vec3 bezier(float t, std::vector<glm::vec3> pontos);