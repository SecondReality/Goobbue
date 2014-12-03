#include "expectimax.h"
#include "actionApplication.h"

Expectimax::Expectimax() : maxDepth(7)
{
}

std::pair<float, Action::Identifier> Expectimax::evaluate(WorldState worldState, int depth)
{
    float bestScore = -std::numeric_limits<float>::infinity();
    Action::Identifier bestActionIdentifier;

    for(auto actionIdentifier : worldState.crafter.actions)
    {
        Action action = actions[actionIdentifier];

        if (!worldState.canUseAction(action))
        {
            continue;
        }

        Outcomes outcomes = applyAction(worldState, action);

        float score = 0;

        score += evaluateQualities(outcomes.first.worldState, depth + 1) * outcomes.first.probability;

        if (outcomes.first.probability == 1.0f)
        {
            // TODO
        }
        else
        {
            score+=evaluateQualities(outcomes.second.worldState, depth + 1) * outcomes.second.probability;
        }

        if(score > bestScore)
        {
            bestScore = score;
            bestActionIdentifier = actionIdentifier;
        }
    }

    return std::make_pair(bestScore, bestActionIdentifier);
}

std::pair<float, Action::Identifier> Expectimax::evaluateNoConditionsNoFailure(const WorldState& worldState, int depth
        #ifdef RAH
        , std::vector<Action::Identifier>& actionHistory
        #endif
)
{
    depth++;

    float bestScore = -std::numeric_limits<float>::infinity();
    Action::Identifier bestActionIdentifier;

#ifdef RAH
    std::vector<Action::Identifier> bestChildActionHistory;
#endif

    for(auto actionIdentifier : worldState.crafter.actions)
    {
        Action action = actions[actionIdentifier];

#ifdef RAH
        std::vector<Action::Identifier> childActionHistory;
#endif

        if(!worldState.canUseAction(action))
        {
            continue;
        }

        Outcomes outcomes = applyAction(worldState, action);

        float score = 0;

        if(outcomes.first.worldState.isTerminal() || depth >= maxDepth)
        {
#ifdef RAH
            childActionHistory.push_back(actionIdentifier);
            #endif

            terminalWorldsEvaluated++;
            if(1.0f==outcomes.first.probability)
            {
                score = fitness(outcomes.first.worldState);
            }
            else
            {
                float delta = fitness(outcomes.first.worldState) - fitness(outcomes.second.worldState);
                score = fitness(outcomes.second.worldState) + delta * outcomes.first.probability;
            }
        }
        else
        {
            // Update the condition:
            WorldState::Condition newCondition;
            switch(outcomes.first.worldState.condition)
            {
                case WorldState::Condition::Excellent: newCondition = WorldState::Condition::Poor; break;
                case WorldState::Condition::Good: newCondition = WorldState::Condition::Normal; break;
                case WorldState::Condition::Poor: newCondition = WorldState::Condition::Normal; break;
                case WorldState::Condition::Normal: // Intentional fallthrough.
                default: newCondition = WorldState::Condition::Normal; break;
            }

            WorldState evaluationWorld = outcomes.first.worldState;
            evaluationWorld.condition = newCondition;

            if(1.0f==outcomes.first.probability)
            {
                score = evaluateNoConditionsNoFailure(evaluationWorld, depth
#ifdef RAH
                        , childActionHistory
                        #endif
                ).first;
            }
            else
            {
                float delta = evaluateNoConditionsNoFailure(evaluationWorld, depth
#ifdef RAH
                        , childActionHistory
                        #endif
                ).first - fitness(outcomes.second.worldState);
                score = fitness(outcomes.second.worldState) + delta * outcomes.first.probability;
            }
        }

        if(score > bestScore)
        {
            bestScore = score;
            bestActionIdentifier = actionIdentifier;
#ifdef RAH
            bestChildActionHistory = childActionHistory;
            #endif
        }
    }

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

float Expectimax::evaluateQualities(const WorldState & worldState, int depth)
{
    if(worldState.isTerminal() || depth > maxDepth)
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
    return (0.65f-0.05f) * qualityFraction + 0.05f * progressFraction + 0.15f * cpFraction + 0.2f * durabilityFraction + qualityBonus;
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