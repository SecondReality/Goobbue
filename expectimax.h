#pragma once
#include "action.h"
#include "worldState.h"

// To enable threading:
#define USE_THREADS

// Recording Action History (slightly slows down program)
// #define RAH

struct ActionEvaluationResult
{
    Action::Identifier actionIdentifier;
    std::vector<Action::Identifier> actionHistory;
    float score;
};

// This algorithm will evaluate a set of actions and return the best action to choose.
class Expectimax
{
public:
    Expectimax();

    Action::Identifier evaluateAction(WorldState worldState);

    typedef std::vector<Action> ActionVector;
    ActionVector actions;

private:
    std::pair<float, Action::Identifier> evaluateNoConditionsNoFailure(const WorldState& worldState, int depth
        #ifdef RAH
        , std::vector<Action::Identifier>& actionHistory
        #endif
    );

    float fitness(const WorldState & worldState);
    std::map<WorldState::Condition, float> conditionMap(WorldState::Condition condition);

    int maxDepth;
    float pGood = 0.23;
    float pExcellent = 0.01;
    int terminalWorldsEvaluated = 0;
    bool considerExcellentCondition = false;

    inline ActionEvaluationResult getActionScore(const WorldState &worldState, int depth, Action::Identifier actionIdentifier);

    float earlyAbandonmentScore = 0;
};