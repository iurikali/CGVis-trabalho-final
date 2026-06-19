#pragma once
#include "game_object.hpp"
#define M_PI 3.14159265358979323846


class Camera
{
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

    float distance; // Distância da câmera para a origem
    float theta; // Ângulo no plano ZX em relação ao eixo Z
    float phi;   // Ângulo em relação ao eixo Y
}; 


class BaseCamera
{
private:
    glm::vec3 position;
    glm::vec3 view;
    glm::vec3 up;
public:
    BaseCamera();

    void set_position(glm::vec3 position);
    void set_view(glm::vec3 view);
    void set_look_at(glm::vec3 look_at);
    void set_up(glm::vec3 up);
    
    glm::vec3 get_position();
    glm::vec3 get_view();
    glm::vec3 get_up();

}; 

class FollowCamera : public BaseCamera
{
private: 
    glm::vec3 target;
    glm::vec3 offset;
public:
    FollowCamera();

    void set_target(glm::vec3 target);
    void set_offset(glm::vec3 offset);
};

class FirstPersonCamera : public BaseCamera
{
private:
    float mouseSensitivity;
    float pitch;
    float yaw;
public:
    FirstPersonCamera();

    void Rotate(float dx, float dy);
    float get_yaw();
};