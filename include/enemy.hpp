#pragma once
#include "game_object.hpp"


class Enemy : public StaticObject
{
private:
    bool go_away = false;
    float dir = 0.0;
    bool shrink = false;
    float speed_rotation;
    float speed;
    float vel_x;

    bool killed_player;

    std::vector<glm::vec3> curve;
    float curve_t;
    float curve_duration;
    bool curve_bounce;
public:
    Enemy(std::string name, int obj_id, int tex_id, glm::vec3 pos, std::vector<glm::vec3> curve, float curve_duration, bool curve_bounce);

    void Update(float delta_time) override;

    void on_trigger_spin(float dir) override; 

    void on_trigger_jump() override;

    void on_trigger_player() override;

    void CollisionLimits(float delta_time);
};