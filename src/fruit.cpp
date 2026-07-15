#include "fruit.hpp"
#include "game_object.hpp"
#include <iostream>
#include <cmath>
#include "player.hpp"

extern int g_fruit_eaten;

Fruit::Fruit(std::string name, int obj_id, int tex_id, glm::vec3 pos):
    StaticObject(name, obj_id, tex_id, false, true, true, false, true, pos)
{
    scale = glm::vec3(0.03, 0.03, 0.03);
    //std::cout << "Fruta " << std::endl;
    hitbox = new AABB(glm::vec3(-7.0, 0.0, -7.0), glm::vec3(7.0, 14.0, 7.0));
    this->UpdateHitbox();

    velv = 0.0;
    original_y = pos.y;

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
            scale.x -= delta_time * .1;
            scale.y -= delta_time * .1;
            scale.z -= delta_time * .1;
        }
        else
        {
            is_destroyed = true;
        }
    }

    if (animation)
        Animation(delta_time);

    this->UpdateHitbox();
}


void Fruit::on_trigger_player()
{
    if (!go_away && velv <= 0)
    {
        is_destroyed = true;
        g_fruit_eaten++;
    }
    
}

void Fruit::on_trigger_spin(float dir)
{
    if (!go_away && !is_destroyed && !animation)
    {
        go_away = true;
        this->dir = dir;
        g_fruit_eaten++;
    }
}

void Fruit::Animation(float delta_time)
{
    if (position.y <= original_y && velv <= 0)
    {
        position.y = original_y;
        animation = false;
    }
    else
    {
        velv -= GRAVITY * delta_time;

        position.y += velv * delta_time;
    }
}

void Fruit::Jump(float height)
{
    animation = true;
    velv = height;
}