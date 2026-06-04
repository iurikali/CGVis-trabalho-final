#include "spikes.hpp"
#include "game_object.hpp"
#include <iostream>
#include <cmath>
#include "player.hpp"
#include <random>
#include <cstdlib>
#include <ctime>

extern bool restart;

Spikes::Spikes(glm::vec3 pos):
    StaticObject("the_spike", PLANE, SPIKE, false, true, false, false, false, pos)
{
    //std::cout << "Fruta " << std::endl;
    hitbox = new AABB(glm::vec3(-0.20 / 6, 0.0, -0.20 / 6), glm::vec3(0.20 / 6, 0.25 / 6, 0.20 / 6));
    
    this->UpdateHitbox();

    scale = glm::vec3(6.0, 6.0, 6.0);
}

void Spikes::on_trigger_player()
{
    hitbox->disabled = true;
    restart = true;
}