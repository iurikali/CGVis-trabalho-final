#pragma once
#include "game_object.hpp"


class Player : public AnimatedObject
{
public:
    Player(std::string name, int obj_id, int tex_id);

    void Update(float delta_time) override;
};