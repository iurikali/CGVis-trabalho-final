#include "fruit.hpp"
#include "game_object.hpp"
#include <iostream>
#include <cmath>




Fruit::Fruit(std::string name, int obj_id, int tex_id, glm::vec3 pos):
    StaticObject(name, obj_id, tex_id, false, true, true, true, pos)
{
    //std::cout << "Fruta " << std::endl;
    hitbox = new AABB(glm::vec3(-0.5, 0.0, -0.5), glm::vec3(0.5, 0.5, 0.5));
    hitbox->Update(pos);

    scale = glm::vec3(0.5, 0.5, 0.5);
}


void Fruit::Update(float delta_time)
{
    rotation.y += 1.0 * delta_time;

    //Mandando a fruta pra pqp
    if (go_away)
    {
        position.x += sinf(dir) * delta_time * 15.0;
        position.z += cosf(dir) * delta_time * 15.0;

        if (scale.x > 0.0)
        {
            scale.x -= delta_time;
            scale.y -= delta_time;
            scale.z -= delta_time;
        }
        else
        {
            is_destroyed = true;
        }
    }

    hitbox->Update(position);
}


void Fruit::on_trigger_player()
{
    if (!go_away)
        is_destroyed = true;
    
}

void Fruit::on_trigger_spin(float dir)
{
    if (!go_away && !is_destroyed)
    {
        go_away = true;
        this->dir = dir;
    }
}