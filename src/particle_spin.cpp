#include "particle_spin.hpp"
#include "game_object.hpp"
#include <iostream>
#include <cmath>
#include "player.hpp"



ParticleSpin::ParticleSpin(glm::vec3 pos, float radius, float dir, float speed, Player *player):
    StaticObject("the_cube", CUBE, COBBLESTONE, false, false, false, false, true, glm::vec3(pos.x, player->position.y + pos.y, pos.z))
{
    this->dir = dir;
    this->radius = radius;
    this->speed = speed;
    this->player = player;  
    y = pos.y;

    scale = glm::vec3(0.2, 0.2, 0.2);
}


void ParticleSpin::Update(float delta_time)
{
    if (!is_destroyed)
    {
        if (player->get_current_state() == ATTACKING && !ended)
        {
            dir += delta_time * speed;

            position.x = player->player_center_x + sinf(dir) * radius;
            position.z = player->player_center_z + cosf(dir) * radius;
            position.y = player->position.y + y;
        }
        else
        {
            ended = true;
            if (scale.x > 0)
            {
                scale.x -= delta_time;
                scale.z -= delta_time;
                scale.y -= delta_time;


                position.x += sinf(dir) * delta_time * speed;
                position.z += cosf(dir) * delta_time * speed;
            }
            else
            {
                is_destroyed = true;    
            }
        }
    }
    
}


