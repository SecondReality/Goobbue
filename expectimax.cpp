#include "expectimax.h"
#include "actionApplication.h"
#include <limits>

bool canUseAction(const WorldState & worldState, Action action)
{
    return worldState.cp >= action.cpCost;
}

std::pair<float, Action::Identifier> Expectimax::evaluate(WorldState worldState, int depth)
{
    float bestScore = -std::numeric_limits<float>::infinity();
    Action::Identifier bestActionIdentifier;

    for(auto actionIdentifier : worldState.crafter.actions)
    {
        Action action = actions[actionIdentifier];

        if(!canUseAction(worldState, action))
        {
            continue;
        }

        Outcomes outcomes = ApplyAction(worldState, action);

        float score = 0;

        score+=evaluateQualities(outcomes.first.worldState, depth+1) * outcomes.first.probability;

        if(action.successProbability!=1.0f)
        {
            score += evaluateQualities(outcomes.second.worldState, depth + 1) * outcomes.second.probability;
        }

        if(score > bestScore)
        {
            bestScore = score;
            bestActionIdentifier = actionIdentifier;
        }
    }

    return std::make_pair(bestScore, bestActionIdentifier);
}

Action::Identifier Expectimax::evaluateAction(WorldState worldState)
{
    Action::Identifier result = evaluate(worldState, 0).second;
    std::cout << "Terminal worlds evaluated: " << terminalWorldsEvaluated << std::endl;
    return result;
}

float Expectimax::evaluateQualities(const WorldState & worldState, int depth)
{
    if(isTerminal(worldState) || depth > maxDepth)
    {
        terminalWorldsEvaluated++;
        return fitness(worldState);
    }

    // create a list of the possible qualities, along with their probabilities.
    // for each entry in the list, apply the condition, evaluate it and multiply it with its probability.
    std::map<WorldState::Condition, float> conditionMapping = conditionMap(worldState.condition);

    float score = 0.0f;

    for(auto vk : conditionMapping)
    {
        WorldState newWorldState = worldState;
        newWorldState.condition = vk.first;

        score+= evaluate(newWorldState, depth).first * vk.second;
    }

    return score;
}

bool Expectimax::isTerminal(const WorldState & worldState)
{
    if(0>=worldState.durability)
    {
        return true;
    }

    if(worldState.progress>=worldState.recipe.difficulty)
    {
        return true;
    }

    return false;
}

float Expectimax::fitness(const WorldState & worldState)
{
    float qualityFraction=worldState.quality/(float)worldState.recipe.maxQuality;
    float progressFraction=worldState.progress/(float)worldState.recipe.difficulty;
    if(0==worldState.durability && progressFraction!=1.0f)
    {
        return 0;
    }

    return qualityFraction * progressFraction;

    if(worldState.progress>=worldState.recipe.difficulty)
    {
        //std::cout << worldState.quality << std::endl;
        return worldState.quality;
    }
    else
    {

    }

    return 0;
}

std::map<WorldState::Condition, float> Expectimax::conditionMap(WorldState::Condition condition)
{
    std::map<WorldState::Condition, float> result;

    if(condition==WorldState::Condition::Excellent)
    {
        result[WorldState::Condition::Poor] = 1.0f;
    }
    else if(condition==WorldState::Condition::Good || condition==WorldState::Condition::Poor)
    {
        result[WorldState::Condition::Normal] = 1.0f;
    }
    else if (condition==WorldState::Condition::Normal)
    {
        if(considerExcellentCondition)
        {
            result[WorldState::Condition::Excellent] = pExcellent;
            result[WorldState::Condition::Good] = pGood;
            result[WorldState::Condition::Normal] = 1.0f - (pExcellent + pGood);
        }
        else
        {
            result[WorldState::Condition::Good] = pGood + pExcellent;
            result[WorldState::Condition::Normal] = 1.0f - (pExcellent + pGood);
        }
    }

    return result;
}