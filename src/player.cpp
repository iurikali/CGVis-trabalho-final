#include "player.hpp"
#include "game_object.hpp"
#include <iostream>

Player::Player(std::string n, int o_id, int t_id, float speed): 
    AnimatedObject(n, o_id, t_id),
    is_w_pressed(false),
    is_a_pressed(false),
    is_s_pressed(false),
    is_d_pressed(false),
    vel_x(0.0f),
    vel_z(0.0f),
    speed(speed)
{
    std::cout << "PLAYER CRIADO" << std::endl;
}


void Player::Update(float delta_time)
{
    vel_x = (float) (is_d_pressed - is_a_pressed) * speed;
    vel_z = (float) (is_s_pressed - is_w_pressed) * speed;

    // std::cout << "vel_x: " << vel_x << std::endl;
    // std::cout << "vel_z: " << vel_z << std::endl;

    if (vel_z > 0) rotation.y = 0.0f;
    if (vel_z < 0) rotation.y = M_PI;

    if (vel_x > 0) rotation.y = M_PI / 2;
    if (vel_x < 0) rotation.y = -M_PI / 2;


    position.x += vel_x * delta_time;
    position.z += vel_z * delta_time;


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