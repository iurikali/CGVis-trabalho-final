#pragma once
#include <glm/mat4x4.hpp>
#include <glm/vec3.hpp>
#include <glad/glad.h>

// Lógica geral de testes colisão está nesse arquivo. 
// Para lógica específica, como a física do player, olhar a sua implementação.

//Classe feita com base no site 
//https://medium.com/@andrebluntindie/3d-aabb-collision-detection-and-resolution-for-voxel-games-5fcbfdb8cdb4
class AABB
{
    public:
    glm::vec3 box_min_original;
    glm::vec3 box_max_original;

    glm::vec3    box_min; // Axis-Aligned Bounding Box do objeto
    glm::vec3    box_max;

    bool disabled = false;

    AABB(glm::vec3 min, glm::vec3 max);

    void Update(glm::vec3 position, glm::vec3 scale);

    bool IntersectsX(AABB against);

    bool IntersectsY(AABB against);

    bool IntersectsZ(AABB against);

    bool Intersects(AABB against);

    float GetClipX(AABB against, float deltaX);

    float GetClipY(AABB against, float deltaY);

    float GetClipZ(AABB against, float deltaZ);

    void DrawDebug();
};