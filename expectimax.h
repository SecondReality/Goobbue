#pragma once

// This algorithm will evaluate a set of actions and return the best action to choose.

#include "action.h"
#include "worldState.h"

class Expectimax
{
public:
    Expectimax() : maxDepth(5)
    {
    }

    Action::Identifier evaluateAction(WorldState worldState);

    typedef std::vector<Action> ActionVector;
    ActionVector actions;

private:
    std::pair<float, Action::Identifier> evaluate(WorldState worldState, int depth);
    float evaluateQualities(const WorldState & worldState, int depth);
    bool isTerminal(const WorldState & worldState);
    float fitness(const WorldState & worldState);
    std::map<WorldState::Condition, float> conditionMap(WorldState::Condition condition);

    int maxDepth;
    float pGood = 0.23;
    float pExcellent = 0.01;
    int terminalWorldsEvaluated = 0;
    bool considerExcellentCondition = false;
};