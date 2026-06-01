#include "box.hpp"
#include "game_object.hpp"
#include <iostream>
#include <cmath>
#include "player.hpp"
#include "particles.hpp"
#include <random>
#include "fruit.hpp"

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
        Destroy();
        is_destroyed = true;
    }
}

void Box::on_trigger_jump()
{
    if (!is_destroyed)
    {
        Destroy();
        is_destroyed = true;
    }
}


void Box::Destroy()
{
    glm::vec3 center = glm::vec3(
        position.x + (hitbox->box_max.x - hitbox->box_min.x) * .5,
        position.y + (hitbox->box_max.y - hitbox->box_min.y) * .5,
        position.z + (hitbox->box_max.z - hitbox->box_min.z) * .5
    );

    //Instanciando as particulas
    for (int i=0; i < 20; i++)
    {
        float theta = getRandomFloat(0.0, M_PI * 2);
        float fi = getRandomFloat(-M_PI * .5, M_PI * .5);

        //std::cout << theta << std::endl;

        new Particles(glm::vec3(0.34, 0.12, 0.05), center, theta, fi, 5.0, 0.5, 
        glm::vec3(.2, .2, .2));
    }

    //Instanciando a fruta
    Fruit *fruit = new Fruit("the_wumpa", PLANE, WUMPA, center);
    fruit->Jump(5.0);
}