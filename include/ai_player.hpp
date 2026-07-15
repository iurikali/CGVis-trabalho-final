#pragma once
#include "player.hpp"
#include <vector>
#include <string>
#include <string_view>
#include <glm/vec3.hpp>

enum class Actions {W, A, S, D, JUMP, SPIN, NONE};

class AIstate
{
public:
    int x;
    int y;
    int z;
    int fruit_eaten;
    AIstate(Player *player);
};

// AI code

struct AIStateActionHash {
    std::size_t operator()(const std::pair<AIstate, Actions>& p) const noexcept {
        // Get the memory address of the referenced object
        // std::size_t h1 = std::hash<const Player*>{}(p.first.player);
        std::size_t h1 = std::hash<int>{}(p.first.x);
        std::size_t h2 = std::hash<int>{}(p.first.y);
        std::size_t h3 = std::hash<int>{}(p.first.z); 
        std::size_t h4 = std::hash<int>{}(p.first.fruit_eaten); 
        std::size_t h5 = std::hash<Actions>{}(p.second);

        std::size_t combined = h1;
        combined ^= h2 + 0x9e3779b9 + (combined << 6) + (combined >> 2);
        combined ^= h3 + 0x9e3779b9 + (combined << 6) + (combined >> 2);
        combined ^= h4 + 0x9e3779b9 + (combined << 6) + (combined >> 2);
        combined ^= h5 + 0x9e3779b9 + (combined << 6) + (combined >> 2);
        return combined;
    }
};

// 3. Custom Equality Functor (Required when using unordered_map with custom keys)
struct AIStateActionEqual {
    bool operator()(const std::pair<AIstate, Actions>& lhs, const std::pair<AIstate, Actions>& rhs) const {
        return (lhs.first.x == rhs.first.x) &&  
               (lhs.first.y == rhs.first.y) &&
               (lhs.first.z == rhs.first.z) &&
               (lhs.second == rhs.second);
    }
};

class AIplayer
{
private:
Player *player;
Actions lastAction;
AIstate lastState;
public:
float epsilon;
float alpha;
float discount;
std::unordered_map<std::pair<AIstate, Actions>, float, AIStateActionHash, AIStateActionEqual> values;

AIplayer(Player *player);

void Move(AIstate state, float reward);

void Update(AIstate state, Actions action, AIstate nextState, float reward);

Actions getAction(AIstate state);

Actions computeActionFromQValues(AIstate state);

float computeValueFromQValues(AIstate state);

float getQValue(AIstate state, Actions action);

std::vector<Actions> getLegalActions(AIstate state);
};