#pragma once
#include "game_object.hpp"


class Box : public StaticObject
{
private:
    float timer = 0.0;
    float dir = 0.0;

public:
    Box(std::string name, int obj_id, int tex_id, glm::vec3 pos);

    void Update(float delta_time) override;

    void on_trigger_spin(float dir) override; 

    void on_trigger_jump() override;
};