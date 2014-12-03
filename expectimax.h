#pragma once
#include "action.h"
#include "worldState.h"

// Recording Action History (significantly effects speed)
// #define RAH

// This algorithm will evaluate a set of actions and return the best action to choose.
class Expectimax
{
public:
    Expectimax();

    Action::Identifier evaluateAction(WorldState worldState);

    typedef std::vector<Action> ActionVector;
    ActionVector actions;

private:
    std::pair<float, Action::Identifier> evaluate(WorldState worldState, int depth);
    std::pair<float, Action::Identifier> evaluateNoConditionsNoFailure(const WorldState& worldState, int depth
        #ifdef RAH
        , std::vector<Action::Identifier>& actionHistory
        #endif
    );
    float evaluateQualities(const WorldState & worldState, int depth);
    float fitness(const WorldState & worldState);
    std::map<WorldState::Condition, float> conditionMap(WorldState::Condition condition);

    int maxDepth;
    float pGood = 0.23;
    float pExcellent = 0.01;
    int terminalWorldsEvaluated = 0;
    bool considerExcellentCondition = false;
};