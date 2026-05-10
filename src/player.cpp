#include "player.hpp"
#include "game_object.hpp"
#include <iostream>

Player::Player(std::string n, int o_id, int t_id): 
    AnimatedObject(n, o_id, t_id)
{
    std::cout << "PLAYER CRIADO" << std::endl;
}


void Player::Update(float delta_time)
{
    AnimatedObject::Update(delta_time);
}