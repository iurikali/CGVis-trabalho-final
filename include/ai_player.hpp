#pragma once
#include "player.hpp"
#include <vector>
#include <string>
#include <glm/vec3.hpp>

enum class Actions {W, A, S, D, JUMP, SPIN};

class AIstate
{
public:
    Player *player;
    AIstate(Player *player);
};

class AIplayer
{
private:
Player *player;
public:
AIplayer(Player *player);

void Move(AIstate state);

Actions getAction(AIstate state);
};