#pragma once
#include "game_object.hpp"

class Particles : public StaticObject
{
private:
    float theta;
    float fi;
    float speed_disappear;
    float speed;
    glm::vec3 color;

public:
    Particles(glm::vec3 color, glm::vec3 pos_inicial, float theta, float fi, 
    float speed, float speed_disappear, glm::vec3 scale);

    void Update(float delta_time) override;

    void Draw() override;
};