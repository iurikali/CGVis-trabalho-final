#pragma once
#include "game_object.hpp"
#include "player.hpp"

class ParticleSpin : public StaticObject
{
private:
    float timer;
    float dir;
    float radius; 
    float speed;
    float y;
    Player *player;
    bool ended = false;


public:
    ParticleSpin(glm::vec3 pos, float radius, float dir, float speed, Player *player);

    void Update(float delta_time) override;

};