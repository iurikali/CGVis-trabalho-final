#include "enemy.hpp"
#include "game_object.hpp"
#include "bezier.hpp"
#include <iostream>
#include <cmath>
#include "player.hpp"
#include <random>
#include <cstdlib>
#include <vector>
#include <ctime>

extern bool restart;

Enemy::Enemy(std::string name, int obj_id, int tex_id, glm::vec3 pos, std::vector<glm::vec3> curve, float curve_duration):
    StaticObject(name, obj_id, tex_id, false, true, true, true, true, pos)
{
    //std::cout << "Fruta " << std::endl;
    hitbox = new AABB(glm::vec3(-0.5, -0.5, -0.5), glm::vec3(0.5, 0.5, 0.5));
    this->UpdateHitbox();

    scale = glm::vec3(0.5, 0.5, 0.5);
    speed_rotation = 15.0;
    speed = 1.0;
    vel_x = speed;
    rotation.y = 0.0f;
    killed_player = false;
    this->curve = curve;
    curve_t = 0;
    this->curve_duration = curve_duration;
    for (size_t i = 0; i < this->curve.size(); i++)
        this->curve[i] += pos;
}


void Enemy::Update(float delta_time)
{
    //Mandando o inimigo pra pqp
    if (go_away)
    {
        position.x += sinf(dir) * delta_time * 15.0;
        position.z += cosf(dir) * delta_time * 15.0;


    }
    else if (shrink)
    {
        rotation.y += delta_time * speed_rotation;
    }

    if (shrink || go_away)
    {
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

    //Colisao e fazendo ele andar
    
    // CollisionLimits(delta_time);

    // position.x += vel_x * delta_time;
    curve_t += delta_time;
    position = bezier(fmod(curve_t/curve_duration, 1.0f), curve);
    hitbox->Update(position, glm::vec3(1.0, 1.0, 1.0));
}


void Enemy::on_trigger_spin(float dir)
{
    if (!is_destroyed && !go_away && !shrink && !killed_player)
    {
        hitbox->disabled = true;
        this->dir = dir;
        go_away = true;
    }
}

void Enemy::on_trigger_jump()
{
    if (!is_destroyed && !go_away && !shrink && !killed_player)
    {
        hitbox->disabled = true;
        shrink = true;
    }
}

void Enemy::on_trigger_player()
{
    if (!is_destroyed && !go_away && !shrink && !killed_player)
    {
        hitbox->disabled = true;
        vel_x = 0;
        restart = true;
    }
}


void Enemy::CollisionLimits(float delta_time)
{
    float half_hitbox = (hitbox->box_max_original.x - hitbox->box_min_original.x) * .5;
    if (position.x + vel_x * delta_time + half_hitbox > X_LIMIT)
    {
        vel_x = -vel_x;
        rotation.y = 0.0;
    }

    if (position.x + vel_x * delta_time - half_hitbox < -X_LIMIT)
    {
        vel_x = -vel_x;
        rotation.y = M_PI;
    }
}