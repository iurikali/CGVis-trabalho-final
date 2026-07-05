#include "ai_player.hpp"
#include "player.hpp"
#include <vector>
#include <iostream>
#include <string>
#include <glm/vec3.hpp>


AIstate::AIstate(Player *player):
    player(player)
{}


AIplayer::AIplayer(Player *player):
    player(player)
{}

void AIplayer::Move(AIstate state)
{
    Actions action = getAction(state);
    // std::cout << std::string(action) << std::endl;
    player->set_w_pressed(action == Actions::W);
    player->set_a_pressed(action == Actions::A);
    player->set_s_pressed(action == Actions::S);
    player->set_d_pressed(action == Actions::D);
    if(action == Actions::JUMP) player->jump();
    if(action == Actions::SPIN) player->spin_attack();
}

Actions AIplayer::getAction(AIstate state)
{
    if (player->position.z > -3 && player->position.x <= 0) return Actions::W;
    if (player->position.z <= -3 && player->position.x < 2) return Actions::D;
    if (player->position.x >= 2 && player->position.z < -1) return Actions::S;
    if (player->position.z >= -1 && player->position.x > 0) return Actions::A;
    return Actions::JUMP;
}


