#pragma once
#include "game_object.hpp"


class Fruit : public StaticObject
{
private:
    bool go_away = false;
    float dir = 0.0;
    float original_y;
    float velv; 

    void Animation(float delta_time);

public:
    Fruit(std::string name, int obj_id, int tex_id, glm::vec3 pos);

    void Update(float delta_time) override;


    void on_trigger_player() override;
    void on_trigger_spin(float dir) override; 
    bool animation = false;
    void Jump(float height);
};