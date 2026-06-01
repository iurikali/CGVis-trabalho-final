#pragma once
#include "game_object.hpp"

#define IDLE 3
#define WALKING 1
#define AIR 2
#define ATTACKING 0



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
    float jump_height;

    bool spin;
    float time_spin;
    float time_spin_index;
    float spin_speed;

    int sector;
    int next_sector;

    void CollisionPhysics(float delta_time);
    
    void CheckCollisionTrigger(int sector_index);

    void CheckCollisionSpin(int sector_index);

    void CheckCollisionJump(int sector_index);

public:
    Player(std::string name, int obj_id, int tex_id, glm::vec3 pos, float vel);

    void Update(float delta_time) override;

    void set_w_pressed(bool b);
    void set_s_pressed(bool b);
    void set_a_pressed(bool b);
    void set_d_pressed(bool b);
    void set_space_pressed(bool b);
    void state_machine(float delta_time);
    int get_current_state();

    void jump(float height);

    bool get_on_air();

    float get_jump_height();

    bool get_spin();
    void set_spin(bool b);
    
    AABB hit_box;

    AABB spin_hitbox;

    AABB jump_hitbox;

    void CreateParticleSpin(int amount, float y, float radius, float speed);

    float player_center_x = 0.0;
    float player_center_z = 0.0;
};