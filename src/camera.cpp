#include "camera.hpp"
#include "game_object.hpp"
#include "matrices.h"
#include <iostream>
#include <cmath>
//const double M_PI = std::acos(-1.0); // Ou 2 * acos(0.0)

extern bool g_first_person;


BaseCamera::BaseCamera():
    position(0.0f, 0.0f, 0.0f),
    view(0.0f, 0.0f, -1.0f),
    up(0.0f, 1.0f, 0.0f)
    {}
    void BaseCamera::set_position(glm::vec3 position) {this->position = position;}
    void BaseCamera::set_view(glm::vec3 view) {this->view = view;}
    void BaseCamera::set_up(glm::vec3 up) {this->up = up;}
    void BaseCamera::set_look_at(glm::vec3 look_at)
    {
        if (look_at - position != glm::vec3(0.0f, 0.0f, 0.0f))
            view = look_at - position;
        else
            std::cout << "look_at deve ser diferente de position!" << std::endl;
    }
    
    glm::vec3 BaseCamera::get_position() {return position;}
    glm::vec3 BaseCamera::get_view() {return view;}
    glm::vec3 BaseCamera::get_up() {return up;}

FollowCamera::FollowCamera():
    target(0.0f, 0.0f, 0.0f),
    offset(0.0f, 1.0f, -1.0f),
    BaseCamera(){
        set_target(target);
        set_offset(offset);
    }
    
    void FollowCamera::set_target(glm::vec3 target)
    {
        this->target = target;
        this->set_position(target + offset);
        this->set_look_at(target);
    }
    void FollowCamera::set_offset(glm::vec3 offset)
    {
        this->offset = offset;
        if (offset != glm::vec3(0.0f, 0.0f, 0.0f))
            this->set_position(target + offset);
        else
            std::cout << "offset deve ser diferente de zero!" << std::endl;
    }


FirstPersonCamera::FirstPersonCamera():
    mouseSensitivity(0.01f),
    pitch(0.0f),
    yaw(M_PI),
    BaseCamera(){}
    
    void FirstPersonCamera::Rotate(float dx, float dy)
    {
        yaw   -= dx * mouseSensitivity;
        pitch -= dy * mouseSensitivity;

        float limit = glm::radians(75.0f);
        pitch = glm::clamp(pitch, -limit, limit);

        // Reconstrói o view vetor a partir dos ângulos
        glm::vec3 view;
        view.x = cos(pitch) * sin(yaw);
        view.y = sin(pitch);
        view.z = cos(pitch) * cos(yaw);

        set_view(glm::normalize(view));
    } 

    float FirstPersonCamera::get_yaw()
    {
        return yaw;
    }

    void FirstPersonCamera::RotateSpin(float speed_degrees_per_second, float dt)
    {
        if (is_spinning)
        {
            float speed_rad = glm::radians(speed_degrees_per_second);
            float delta_yaw = speed_rad * dt;
            float max_rotation = glm::radians(360.0f); 
            
            if (accumulated_rotation + delta_yaw >= max_rotation)
            {
                delta_yaw = max_rotation - accumulated_rotation;
                
                accumulated_rotation = 0.0f; 
                is_spinning = false; 
            }
            else
            {
                accumulated_rotation += delta_yaw;
            }

            yaw += delta_yaw; 

            if (yaw >= max_rotation) yaw -= max_rotation;
            if (yaw < 0.0f)          yaw += max_rotation;

            glm::vec3 view;
            view.x = cos(pitch) * sin(yaw);
            view.y = sin(pitch);
            view.z = cos(pitch) * cos(yaw);

            set_view(glm::normalize(view));
        }
    }


    Camera::Camera(): 
    theta(0.0f),
    phi(0.0f),
    distance(3.5f),
    look_at(0.0f, 0.0f, 0.0f)
{
    update_position();
}


void Camera::Rotate(float dx, float dy)
{
    // Atualizamos parâmetros da câmera com os deslocamentos
    theta -= 0.01f*dx;
    phi   += 0.01f*dy;
    
    // Em coordenadas esféricas, o ângulo phi deve ficar entre -pi/2 e +pi/2.
    float phimax = 3.141592f/2;
    float phimin = -phimax;

    if (g_first_person)
    {
        phimax = 3.141592f/4;
        phimin = -phimax;
    }
    
    if (phi > phimax)
    phi = phimax;
    
    if (phi < phimin)
    phi = phimin;
    update_position();
}

void Camera::Zoom(float yoffset)
{
    // Atualizamos a distância da câmera para a origem utilizando a
    // movimentação da "rodinha", simulando um ZOOM.
    distance -= 0.1f*yoffset;
    
    // Uma câmera look-at nunca pode estar exatamente "em cima" do ponto para
    // onde ela está olhando, pois isto gera problemas de divisão por zero na
    // definição do sistema de coordenadas da câmera. Isto é, a variável abaixo
    // nunca pode ser zero. Versões anteriores deste código possuíam este bug,
    // o qual foi detectado pelo aluno Vinicius Fraga (2017/2).
    const float verysmallnumber = std::numeric_limits<float>::epsilon();
    if (distance < verysmallnumber)
    distance = verysmallnumber;
    update_position();
}

void Camera::update_position()
{
    float y = distance*sin(phi);
    float z = distance*cos(phi)*cos(theta);
    float x = distance*cos(phi)*sin(theta);

    // Abaixo definimos as varáveis que efetivamente definem a câmera virtual.
    // Veja slides 195-227 e 229-234 do documento Aula_08_Sistemas_de_Coordenadas.pdf.
    position  = glm::vec3(x, y, z) + look_at; // Ponto "c", centro da câmera
    view = look_at - position; // Vetor "view", sentido para onde a câmera está virada
    up = glm::vec3(0.0f, 1.0f, 0.0f); // Vetor "up" fixado para apontar para o "céu" (eito Y global)
}

float Camera::get_distance()
{
    return distance;
}

void Camera::set_look_at(glm::vec3 look_at)
{
    this->look_at = look_at;
    update_position();
}