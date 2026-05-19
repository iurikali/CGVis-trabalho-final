#pragma once
#include "game_object.hpp"

#define IDLE 3
#define WALKING 1
#define AIR 2
#define ATTACKING 0

#define GRAVITY 10.0

class Player : public AnimatedObject
{
private:
    bool is_w_pressed;
    bool is_s_pressed;
    bool is_a_pressed;
    bool is_d_pressed;
    bool is_space_pressed;

    float vel_x;
    float vel_y;
    float vel_z;
    float speed;

    float index_angle;
    float angle_looking;

    int state;
    bool on_air;
public:
    Player(std::string name, int obj_id, int tex_id, float vel);

    void Update(float delta_time) override;

    void set_w_pressed(bool b);
    void set_s_pressed(bool b);
    void set_a_pressed(bool b);
    void set_d_pressed(bool b);
    void set_space_pressed(bool b);
    void state_machine(float delta_time);

    void jump(float height);

    bool get_on_air();
};