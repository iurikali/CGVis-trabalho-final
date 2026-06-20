#include "bezier.hpp"
#include <glm/vec3.hpp>
#include <glad/glad.h>
#include <vector>
#include <cmath>

glm::vec3 vec3Lerp(float t, glm::vec3 a, glm::vec3 b)
{
    return t*b + (1-t)*a;
}

glm::vec3 bezier(float t, std::vector<glm::vec3> pontos)
{ 
    std::vector<glm::vec3> temp = pontos;

    for (int size = pontos.size()-1; size > 0; size--){
        for (int i = 0; i < size; i++)
            temp[i] = vec3Lerp(t, pontos[i], pontos[i+1]);
        for (int i = 0; i < size; i++)
            pontos[i] = temp[i];
    }

    return pontos[0];
}  

// glm::vec3 CubicBezier(glm::vec3 p0, glm::vec3 p1, glm::vec3 p2, glm::vec3 p3, float t)
// {
//     float u = 1.0f - t;
//     return u*u*u*p0 + 3*u*u*t*p1 + 3*u*t*t*p2 + t*t*t*p3;
// }

