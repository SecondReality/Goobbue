#include "expectimax.h"
#include "actionApplication.h"
//#include <limits>
//#include <future>
//#define BOOST_ALL_NO_LIB
#include <boost/thread.hpp>
#define BOOST_THREAD_PROVIDES_FUTURE
#include <boost/thread/future.hpp>

using namespace boost;

Expectimax::Expectimax() : maxDepth(8)
{
}

// Updates the condition of a world.
// Note that this is deterministic and only gives an approximation.
void updateCondition(WorldState& worldState)
{
    WorldState::Condition newCondition;
    switch(worldState.condition)
    {
        case WorldState::Condition::Excellent: newCondition = WorldState::Condition::Poor; break;
        case WorldState::Condition::Good: newCondition = WorldState::Condition::Normal; break;
        case WorldState::Condition::Poor: newCondition = WorldState::Condition::Normal; break;
        case WorldState::Condition::Normal: // Intentional fallthrough.
        default: newCondition = WorldState::Condition::Normal; break;
    }

    worldState.condition = newCondition;
}

inline ActionEvaluationResult Expectimax::getActionScore(const WorldState &worldState, int depth, Action::Identifier actionIdentifier)
{
    ActionEvaluationResult actionEvaluationResult;

    Action action = actions[actionIdentifier];

    if (!worldState.canUseAction(action))
    {
        actionEvaluationResult.score = -std::numeric_limits<float>::infinity();
        return actionEvaluationResult;
    }

    Outcomes outcomes = applyAction(worldState, action);
    updateCondition(outcomes.first.worldState);
    updateCondition(outcomes.second.worldState);

    if (outcomes.first.worldState.isTerminal() || depth >= maxDepth)
    {
#ifdef RAH
        actionEvaluationResult.actionHistory.push_back(actionIdentifier);
#endif

        terminalWorldsEvaluated++;
        if (1.0f == outcomes.first.probability)
        {
            actionEvaluationResult.score = fitness(outcomes.first.worldState);
        }
        else
        {
            float delta = fitness(outcomes.first.worldState) - fitness(outcomes.second.worldState);
            actionEvaluationResult.score = fitness(outcomes.second.worldState) + delta * outcomes.first.probability;
        }
    }
    else
    {
        if (1.0f == outcomes.first.probability)
        {
            actionEvaluationResult.score = evaluateNoConditionsNoFailure(outcomes.first.worldState, depth
#ifdef RAH
                    , actionEvaluationResult.actionHistory
#endif
            ).first;
        }
        else
        {
            auto successPair = evaluateNoConditionsNoFailure(outcomes.first.worldState, depth
#ifdef RAH
                    , actionEvaluationResult.actionHistory
#endif
            );

            // TODO: Remove failureActionEvaluationResult somehow - it is unused and wasting processing
            ActionEvaluationResult failureActionEvaluationResult;
            auto failurePair = evaluateNoConditionsNoFailure(outcomes.second.worldState, depth
#ifdef RAH
                    , failureActionEvaluationResult.actionHistory
#endif
            );

            float delta = successPair.first - failurePair.first;
            actionEvaluationResult.score = failurePair.first + delta * outcomes.first.probability;
        }
    }

    if (depth == 1)
    {
        // std::cout << "Action: " << actionIdentifierToString(actionIdentifier) << " fitness: " << actionEvaluationResult.score << std::endl;
    }

    return actionEvaluationResult;
}

std::pair<float, Action::Identifier> Expectimax::evaluateNoConditionsNoFailure(const WorldState& worldState, int depth
        #ifdef RAH
        , std::vector<Action::Identifier>& actionHistory
        #endif
)
{
    depth++;

    // Early rejection of improbable worlds:
    /*
    if(depth==6) // && action has no future promise (e.g. it's not a buff)
    {
        if (fitness(worldState) < earlyAbandonmentScore * 0.8f)
        {
            return std::make_pair(0.0f, Action::Identifier::byregotsBlessing);
        }
        else if (fitness(worldState) > earlyAbandonmentScore)
        {
            earlyAbandonmentScore = fitness(worldState);
        }
    }
    */

    float bestScore = -std::numeric_limits<float>::infinity();
    Action::Identifier bestActionIdentifier;

#ifdef RAH
    std::vector<Action::Identifier> bestChildActionHistory;
#endif

#ifdef USE_THREADS
    std::map<Action::Identifier, unique_future<ActionEvaluationResult> > futures;
#endif

    // Lets just get an array of structures - it is simpler;
    int n=0;
    for(auto actionIdentifier : worldState.crafter.actions)
    {

        if(depth==1)
        {
            // std::cout << "Processing action " << n << " of " << worldState.crafter.actions.size() << std::endl;
        }
#ifdef USE_THREADS
        if(depth==1)
        {
            futures[actionIdentifier] = boost::async(boost::launch::async, boost::bind(&Expectimax::getActionScore, this, worldState, depth, actionIdentifier));
            // std::cout << "Creating thread" << std::endl;
            continue;
        }
        else
        #endif
        {
            ActionEvaluationResult actionEvaluationResult = getActionScore(worldState, depth, actionIdentifier);
            if(actionEvaluationResult.score > bestScore)
            {
                bestScore = actionEvaluationResult.score;
                bestActionIdentifier = actionIdentifier;
#ifdef RAH
                // TODO!
                bestChildActionHistory = actionEvaluationResult.actionHistory;
#endif
            }
        }
        n++;
    }

#ifdef USE_THREADS
    // Collate the results from our futures:
    // Wait for our futures to finish:
    for(auto & kv : futures)
    {
        //std::cout << "Waiting for thread" << std::endl;
        kv.second.wait();

        ActionEvaluationResult actionEvaluationResult = kv.second.get();
        if(actionEvaluationResult.score > bestScore)
        {
            bestScore = actionEvaluationResult.score;
            bestActionIdentifier = kv.first;
#ifdef RAH
            bestChildActionHistory = actionEvaluationResult.actionHistory;
#endif
        }

        //std::cout << "Received result" << std::endl;

    }
    #endif

#ifdef RAH
    actionHistory.push_back(bestActionIdentifier);
    std::copy( bestChildActionHistory.begin(), bestChildActionHistory.end(), std::back_inserter(actionHistory));
    #endif
    return std::make_pair(bestScore, bestActionIdentifier);
}

Action::Identifier Expectimax::evaluateAction(WorldState worldState)
{

    std::vector<Action::Identifier> actionHistory;
    std::pair<float, Action::Identifier> result = evaluateNoConditionsNoFailure(worldState, 0
#ifdef RAH
            , actionHistory
#endif
    );
    // std::cout << "world score: " << result.first << " terminal worlds evaluated: " << terminalWorldsEvaluated << std::endl;

    for(auto actionIdentifier : actionHistory)
    {
        std::cout << actionIdentifierToString(actionIdentifier) << " > ";
        worldState = applyAction(worldState, actions[actionIdentifier]).first.worldState;
    }
    std::cout << std::endl;

    std::cout << "With normal conditions and all successes the algorithm looked forwards to this world state: " << std::endl;
    worldState.print();

    return result.second;
}

float Expectimax::fitness(const WorldState & worldState)
{
    float qualityFraction = worldState.quality / (float) worldState.recipe.maxQuality;
    float progressFraction = worldState.progress / (float) worldState.recipe.difficulty;
    float cpFraction = worldState.cp / (float) worldState.crafter.cp;
    float durabilityFraction = worldState.durability / (float) worldState.recipe.durability;

    // Used on leaves only:
    if(0 == worldState.durability && progressFraction != 1.0f)
    {
        return 0;
    }

    float qualityBonus = qualityFraction==1.0f ? 0.1f : 0.0f;
    return (0.65f-0.05f) * qualityFraction + 0.05f * progressFraction + 0.10f * cpFraction + 0.2f * durabilityFraction + qualityBonus;
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