#include "fruit.hpp"
#include "game_object.hpp"
#include <iostream>
#include <cmath>




Fruit::Fruit(std::string name, int obj_id, int tex_id, glm::vec3 pos):
    StaticObject(name, obj_id, tex_id, LAYER_TRIGGER, pos)
{
    //std::cout << "Fruta " << std::endl;
    hitbox = new AABB(glm::vec3(-0.5, 0.0, -0.5), glm::vec3(0.5, 0.5, 0.5));
    hitbox->Update(pos);

    scale = glm::vec3(0.5, 0.5, 0.5);
}


void Fruit::Update(float delta_time)
{
    rotation.y += 1.0 * delta_time;
}


void Fruit::on_trigger_player(float delta_time)
{
    is_destroyed = true;
    std::cout << "Fruta " << std::endl;
}

void Fruit::on_trigger_spin(float delta_time)
{

}