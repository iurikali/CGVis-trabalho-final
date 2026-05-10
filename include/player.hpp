#pragma once
#include "game_object.hpp"


class Player : public AnimatedObject
{
private:
    bool is_w_pressed;
    bool is_s_pressed;
    bool is_a_pressed;
    bool is_d_pressed;

    float vel_x;
    float vel_z;
    float speed;
public:
    Player(std::string name, int obj_id, int tex_id, float vel);

    void Update(float delta_time) override;

    void set_w_pressed(bool b);
    void set_s_pressed(bool b);
    void set_a_pressed(bool b);
    void set_d_pressed(bool b);
};