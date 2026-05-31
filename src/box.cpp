#include "box.hpp"
#include "game_object.hpp"
#include <iostream>
#include <cmath>
#include "player.hpp"



Box::Box(std::string name, int obj_id, int tex_id, glm::vec3 pos):
    StaticObject(name, obj_id, tex_id, true, false, true, true, true, pos)
{
    //std::cout << "Fruta " << std::endl;
    hitbox = new AABB(glm::vec3(-0.5, 0.0, -0.5), glm::vec3(0.5, 1.0, 0.5));
    hitbox->Update(pos + glm::vec3(0.5, 0.0, 0.5));

    //scale = glm::vec3(0.5, 0.5, 0.5);
}


void Box::Update(float delta_time)
{

}


void Box::on_trigger_spin(float dir)
{
    if (!is_destroyed)
    {
        is_destroyed = true;
    }
}

void Box::on_trigger_jump()
{
    if (!is_destroyed)
    {
        is_destroyed = true;
    }
}


