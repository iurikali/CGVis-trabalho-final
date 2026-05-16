#include "camera.hpp"
#include "game_object.hpp"
#include <iostream>
#include <cmath>
const double M_PI = std::acos(-1.0); // Ou 2 * acos(0.0)

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
