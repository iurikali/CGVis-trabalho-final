#include "box.hpp"
#include "game_object.hpp"
#include <iostream>
#include <cmath>
#include "player.hpp"
#include "particles.hpp"
#include <random>


#include <cstdlib>
#include <ctime>

float getRandomFloat(float min, float max)
{
    // static_cast prevents integer truncation during division
    return min + static_cast<float>(rand()) / (static_cast<float>(RAND_MAX / (max - min)));
}

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
        Destroy();
    }
}

void Box::on_trigger_jump()
{
    if (!is_destroyed)
    {
        is_destroyed = true;
        Destroy();
    }
}


void Box::Destroy()
{
    for (int i=0; i < 10; i++)
    {
        float theta = getRandomFloat(0.0, M_PI * 2);
        
        //std::cout << theta << std::endl;

        new Particles(glm::vec3(1.0, 1.0, 1.0), position, 0.0, 0.0, 1.0, 1.0, 
        glm::vec3(.1, .1, .1));
    }
}