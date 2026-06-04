#pragma once
#include "game_object.hpp"


class Spikes : public StaticObject
{
private:

public:
    Spikes(glm::vec3 pos);

    void on_trigger_player() override;
};