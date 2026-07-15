#include "ai_player.hpp"
#include "player.hpp"
#include <vector>
#include <iostream>
#include <string>
#include <glm/vec3.hpp>
#include <random>
#include <algorithm>
#include <iterator>




template<typename Iter, typename RandomGenerator>
Iter select_randomly(Iter start, Iter end, RandomGenerator& g) {
    std::uniform_int_distribution<> dis(0, std::distance(start, end) - 1);
    std::advance(start, dis(g));
    return start;
}

template<typename Iter>
Iter select_randomly(Iter start, Iter end) {
    static std::random_device rd;
    static std::mt19937 gen(rd());
    return select_randomly(start, end, gen);
}

float random_float(float start, float end) {
    std::random_device rd;  // Will be used to obtain a seed for the random number engine
    std::mt19937 gen(rd()); // Standard mersenne_twister_engine seeded with rd()
    std::uniform_real_distribution<> dist_float(start, end);
    return dist_float(gen);
}

extern int g_fruit_eaten;
AIstate::AIstate(Player *player):
    x(floorf(player->position.x)), y(floorf(player->position.y)), z(floorf(player->position.z)), fruit_eaten(g_fruit_eaten)
{}


AIplayer::AIplayer(Player *player):
    player(player), epsilon(0.3), values({}), alpha(0.8), discount(0.9), lastAction(Actions::NONE), lastState(player)
{}

void AIplayer::Move(AIstate state, float reward)
{
    Update(lastState, lastAction, state, reward);

    Actions action = getAction(state);
    // std::cout << std::string(action) << std::endl;
    player->set_w_pressed(action == Actions::W);
    player->set_a_pressed(action == Actions::A);
    player->set_s_pressed(action == Actions::S);
    player->set_d_pressed(action == Actions::D);
    if(action == Actions::JUMP) player->jump();
    if(action == Actions::SPIN) player->spin_attack();

    lastAction = action;
    lastState = state;
}

void AIplayer::Update(AIstate state, Actions action, AIstate nextState, float reward)
{
    float q_new = (1 - alpha) * getQValue(state, action);
    q_new += alpha * (reward + discount * computeValueFromQValues(nextState));
    values[{state, action}] = q_new;
}

Actions AIplayer::getAction(AIstate state)
{
    std::vector<Actions> legalActions = getLegalActions(state);

    if (random_float(0.0f, 1.0f) < epsilon) 
    {
        return *select_randomly(legalActions.begin(), legalActions.end());
    }
    return computeActionFromQValues(state);



    if (player->position.z > -3 && player->position.x <= 0) return Actions::W;
    if (player->position.z <= -3 && player->position.x < 2) return Actions::D;
    if (player->position.x >= 2 && player->position.z < -1) return Actions::S;
    if (player->position.z >= -1 && player->position.x > 0) return Actions::A;
    return Actions::JUMP;
}

Actions AIplayer::computeActionFromQValues(AIstate state)
{
    std::vector<Actions> legalActions = getLegalActions(state);

    float max_q = -INFINITY;
    for (Actions action : legalActions){
        float q = getQValue(state, action);
        if (q > max_q)
            max_q = q;
    }

    std::vector<Actions> best = {};
    for (Actions action : legalActions){
        if (getQValue(state, action) == max_q)
            best.push_back(action);
    }

    return *select_randomly(best.begin(), best.end());
}

float AIplayer::computeValueFromQValues(AIstate state)
{
    std::vector<Actions> legalActions = getLegalActions(state);
    if (legalActions.empty())
        return 0.0f;
    float max_q = -INFINITY;
    for (Actions action : legalActions){
        float q = getQValue(state, action);
        if (q > max_q)
            max_q = q;
    }
    return max_q;  
}



float AIplayer::getQValue(AIstate state, Actions action)
{
    // std::unordered_map<Actions, float> values = {{Actions::W, 1.0f}, {Actions::A, 0.0f}, {Actions::S, 0.0f}, 
    // {Actions::D, 0.0f}, {Actions::JUMP, 1.0f}, {Actions::SPIN, 0.0f}, {Actions::NONE, 0.0f}};
    values.insert({{state, action}, 0.0f});
    return values[{state, action}];
    
}


std::vector<Actions> AIplayer::getLegalActions(AIstate state)
{
    std::vector<Actions> legalActions = {Actions::W, Actions::A, Actions::S, Actions::D, Actions::JUMP, Actions::SPIN, Actions::NONE};
    // std::vector<Actions> legalActions = {Actions::W, Actions::S, Actions::JUMP, Actions::NONE};
    if (player->get_on_air())
        legalActions.erase(std::remove(legalActions.begin(), legalActions.end(), Actions::JUMP), legalActions.end());
    return legalActions;
}

// if (player->position.z > -3 && player->position.x <= 0) return Actions::W;
// if (player->position.z <= -3 && player->position.x < 2) return Actions::D;
// if (player->position.x >= 2 && player->position.z < -1) return Actions::S;
// if (player->position.z >= -1 && player->position.x > 0) return Actions::A;
// return Actions::JUMP;