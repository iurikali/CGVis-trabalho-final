#pragma once
#include "game_object.hpp"


class Camera
{
private:
    float theta; // Ângulo no plano ZX em relação ao eixo Z
    float phi;   // Ângulo em relação ao eixo Y
    float distance; // Distância da câmera para a origem

public:
    glm::vec3 position;
    glm::vec3 view;
    glm::vec3 up;
    glm::vec3 look_at;
    Camera();

    void Rotate(float dx, float dy);

    void Zoom(float yoffset);

    void update_position();

    float get_distance();

    void set_look_at(glm::vec3 look_at);

};