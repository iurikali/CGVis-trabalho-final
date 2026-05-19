#include "player.hpp"
#include "game_object.hpp"
#include <iostream>
#include <cmath>

#define M_PI 3.14159265358979323846

float lerp (float a, float b, float t)
{
    return a + t * (b - a);
}

float lerp_angle(float from, float to, float weight) 
{
    // Diferença entre os dois ângulos
    float difference = to - from;
    
    // Normaliza a diferença para o intervalo [-PI, PI]
    while (difference < -M_PI) difference += 2.0f * M_PI;
    while (difference >  M_PI) difference -= 2.0f * M_PI;
    
    // Faz o lerp padrão usando a diferença corrigida
    return from + difference * weight;
}


Player::Player(std::string n, int o_id, int t_id, float speed): 
    AnimatedObject(n, o_id, t_id),
    is_w_pressed(false),
    is_a_pressed(false),
    is_s_pressed(false),
    is_d_pressed(false),
    vel_x(0.0f),
    vel_y(0.0f),
    vel_z(0.0f),
    speed(speed),
    index_angle(0.0f),
    angle_looking(0.0f)

{
    std::cout << "PLAYER CRIADO" << std::endl;
}


void Player::Update(float delta_time)
{
    vel_x = (float) (is_d_pressed - is_a_pressed) * speed;
    vel_y += (float) (is_space_pressed) * speed * delta_time * 30;
    vel_z = (float) (is_s_pressed - is_w_pressed) * speed;

    if (vel_y > speed) vel_y = speed;

    
    //Calculando a rotação
    if(vel_x != 0.0 || vel_z != 0.0)
        angle_looking = std::atan2(vel_x, vel_z);

    
    index_angle = lerp_angle(index_angle, angle_looking, 16.0 * delta_time);
    rotation.y = index_angle;
    


    position.x += vel_x * delta_time;
    position.y += vel_y * delta_time;
    position.z += vel_z * delta_time;

    
    if (position.y > 0) // Não está no chão
        vel_y -= 3 * speed * delta_time;
    if (position.y <= 0){ // está no chão (ou abaixo)
        position.y = 0;
        vel_y = 0;
    }


    AnimatedObject::Update(delta_time);
}

void Player::set_w_pressed(bool b)
{
    is_w_pressed = b;
}
void Player::set_s_pressed(bool b)
{
    is_s_pressed = b;
}
void Player::set_a_pressed(bool b)
{
    is_a_pressed = b;
}
void Player::set_d_pressed(bool b)
{
    is_d_pressed = b;
}
void Player::set_space_pressed(bool b)
{
    is_space_pressed = b;
}