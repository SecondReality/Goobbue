#include "actionApplication.h"

#include <math.h>

float round(float d)
{
    return floor(d + 0.5f);
}

float calculateBaseProgressIncrease(int levelDifference, int craftsmanship)
{
    float levelCorrectionFactor = 0;

    if((levelDifference < -5))
    {
        levelCorrectionFactor = 0.0152f * levelDifference;
    }
    else if ((-5 <= levelDifference) && (levelDifference <= 0))
    {
        levelCorrectionFactor = 0.10f * levelDifference;
    }
    else if ((0 < levelDifference) && (levelDifference <= 5))
    {
        levelCorrectionFactor = 0.05f * levelDifference;
    }
    else if ((5 < levelDifference) && (levelDifference <= 15))
    {
        levelCorrectionFactor = 0.022f * levelDifference + 0.15f;
    }
    else
    {
        levelCorrectionFactor = 0.0033f * levelDifference + 0.43f;
    }

    float baseProgress = 0.21f * craftsmanship + 1.6f;
    float levelCorrectedProgress = baseProgress * (1 + levelCorrectionFactor);

    return round(levelCorrectedProgress);
}

float calculateBaseQualityIncrease(int levelDifference, int control)
{
    float levelCorrectionFactor = 0;

    if (levelDifference < -20)
    {
        levelCorrectionFactor = 0.02867f * levelDifference;
    }
    else if (-20 <= levelDifference && levelDifference < -5)
    {
        levelCorrectionFactor = 0.02867f * levelDifference;
    }
    else if ((-5 <= levelDifference) && (levelDifference <= 0))
    {
        levelCorrectionFactor = 0.05f * levelDifference;
    }
    else
    {
        levelCorrectionFactor = 0;
    }

    float baseQuality = 0;
    if(levelDifference < -20)
    {
        baseQuality = 0.6642f * control + 41.80f;
    }
    else if(-20 <= levelDifference && levelDifference < -5)
    {
        baseQuality = 0.6642f * control + 41.80f;
    }
    else
    {
        baseQuality = 3.46e-5 * control * control + 0.3514 * control + 34.66;
    }
    float levelCorrectedQuality = baseQuality * (1 + levelCorrectionFactor);

    return round(levelCorrectedQuality);
}

void SimulateAction(WorldState& worldState, Action action, float success);

float actionSuccessProbability(const WorldState& worldState, Action action)
{
    float successProbability = 0.0f;

    if (worldState.effects.countDownsContainsAction(Action::Identifier::steadyHand2))
    {
        successProbability = action.successProbability + 0.3f;
    }
    else if(worldState.effects.countDownsContainsAction(Action::Identifier::steadyHand))
    {
        successProbability = action.successProbability + 0.2f;
    }
    else
    {
        successProbability = action.successProbability;
    }

    successProbability = std::min(successProbability, 1.0f);

    return successProbability;
}

Outcomes applyAction(const WorldState &worldState, Action action, bool verbose)
{
    float successProbability=actionSuccessProbability(worldState, action);
    float failureProbability = 1.0f - successProbability;

    Outcome successWorld;
    successWorld.worldState = worldState;
    successWorld.probability = successProbability;

    Outcome failureWorld;
    failureWorld.worldState = worldState;
    failureWorld.probability = failureProbability;

    SimulateAction(successWorld.worldState, action, 1.0f);

    if(1.0f!=successProbability)
    {
        SimulateAction(failureWorld.worldState, action, 0.0f);
    }

    return std::make_pair(successWorld, failureWorld);
}

void SimulateAction(WorldState& worldState, Action action, float success)
{
    const Crafter &crafter = worldState.crafter;
    const Recipe &recipe = worldState.recipe;
    const Effects &effects = worldState.effects;

    int craftsmanship = crafter.craftsmanship;
    int control = crafter.control;

    if(effects.countUpsContainsAction(Action::Identifier::innerQuiet))
    {
        control *= (1 + 0.2 * effects.getCountUpsValue(Action::Identifier::innerQuiet));
    }

    if(effects.countDownsContainsAction(Action::Identifier::innovation))
    {
        control *= 1.5;
    }

    int levelDifference = worldState.crafter.level - recipe.level;

    if(effects.countDownsContainsAction(Action::Identifier::ingenuity2) && (levelDifference < -20) && (crafter.level == 50))
    {
        levelDifference = levelDifference + 20;
    }
    else if(effects.countDownsContainsAction(Action::Identifier::ingenuity2))
    {
        levelDifference = 3;
    }
    else if(effects.countDownsContainsAction(Action::Identifier::ingenuity) && (levelDifference < -20) && (crafter.level == 50))
    {
        levelDifference = levelDifference + 10;
    }
    else if(effects.countDownsContainsAction(Action::Identifier::ingenuity))
    {
        levelDifference = 0;
    }

    float qualityIncreaseMultiplier = action.qualityIncreaseMultiplier;

    if(effects.countDownsContainsAction(Action::Identifier::greatStrides))
    {
        qualityIncreaseMultiplier *= 2;
    }

    if(worldState.condition==WorldState::Condition::Poor)
    {
        qualityIncreaseMultiplier *= 0.5f;
    }
    else if(worldState.condition==WorldState::Condition::Excellent)
    {
        qualityIncreaseMultiplier *= 4;
    }
    else if(worldState.condition==WorldState::Condition::Good)
    {
        qualityIncreaseMultiplier *= 1.5f;
    }
    else if(worldState.condition==WorldState::Condition::Normal)
    {
        qualityIncreaseMultiplier *= 1;
    }
    else
    {
        std::cout << "Error - unknown condition";
    }

    float bProgressGain = action.progressIncreaseMultiplier * calculateBaseProgressIncrease(levelDifference, craftsmanship);

    if(action.identifier==Action::Identifier::flawlessSynthesis)
    {
        bProgressGain = 40;
    }
    else if(action.identifier==Action::Identifier::pieceByPiece)
    {
        bProgressGain = (recipe.difficulty - worldState.progress)/3;
    }

    float progressGain = success * bProgressGain;

    float bQualityGain = qualityIncreaseMultiplier * calculateBaseQualityIncrease(levelDifference, control);
    float qualityGain = success * bQualityGain;

    if(action.identifier==Action::Identifier::byregotsBlessing && effects.countUpsContainsAction(Action::Identifier::innerQuiet))
    {
        qualityGain *= (1 + 0.2f * effects.getCountUpsValue(Action::Identifier::innerQuiet));
    }

    float durabilityCost = action.durabilityCost;

    if(effects.countDownsContainsAction(Action::Identifier::wasteNot) || effects.countDownsContainsAction(Action::Identifier::wasteNot2))
    {
        durabilityCost = 0.5f * action.durabilityCost;
    }

    // State tracking
    worldState.progress += round(progressGain);
    worldState.quality += round(qualityGain);
    worldState.durability -= durabilityCost;
    worldState.cp -= action.cpCost;

    // Effect management
    //==================================
    // Special Effect Actions

    if(action.identifier==Action::Identifier::mastersMend)
    {
        worldState.durability += 30;
    }

    if(action.identifier==Action::Identifier::mastersMend2)
    {
        worldState.durability += 60;
    }

    if(effects.countDownsContainsAction(Action::Identifier::manipulation) && worldState.durability > 0)
    {
        worldState.durability += 10;
    }

    if(effects.countDownsContainsAction(Action::Identifier::comfortZone) && worldState.cp > 0)
    {
        worldState.cp += 8;
    }

    if(action.identifier==Action::Identifier::rumination && worldState.cp > 0)
    {
        if(effects.countUpsContainsAction(Action::Identifier::innerQuiet) && effects.getCountUpsValue(Action::Identifier::innerQuiet) > 0)
        {
            int innerQuietValue = effects.getCountUpsValue(Action::Identifier::innerQuiet);
            worldState.cp += (21 * innerQuietValue - pow(innerQuietValue, 2) + 10) / 2;
            worldState.effects.removeCountUp(Action::Identifier::innerQuiet);
        }
    }

    if(action.identifier==Action::Identifier::byregotsBlessing)
    {
        if(effects.countUpsContainsAction(Action::Identifier::innerQuiet))
        {
            worldState.effects.removeCountUp(Action::Identifier::innerQuiet);
        }
    }

    if(action.qualityIncreaseMultiplier > 0 && effects.countDownsContainsAction(Action::Identifier::greatStrides))
    {
        worldState.effects.removeCountDown(Action::Identifier::greatStrides);
    }

    if(action.identifier==Action::Identifier::tricksOfTheTrade && worldState.cp > 0)
    {
        worldState.cp += 20;
    }

    // Decrement countdowns
    worldState.effects.updateCountDowns();

    // Increment countups
    if(action.qualityIncreaseMultiplier > 0 && effects.countUpsContainsAction(Action::Identifier::innerQuiet))
    {
        worldState.effects.countUps[Action::Identifier::innerQuiet] += 1 * success;
    }

    // Add new action to countUps
    if(action.effectType==Action::EffectType::CountUp)
    {
        worldState.effects.countUps[action.identifier] = 0;
    }

    // Add new action to countDowns
    if(action.effectType==Action::EffectType::CountDown)
    {
        worldState.effects.countDowns[action.identifier] = action.activeTurns;
    }

    worldState.progress = std::min(worldState.progress, worldState.recipe.difficulty);
    worldState.quality = std::min(worldState.quality, worldState.recipe.maxQuality);
    worldState.durability = std::min(worldState.durability, worldState.recipe.durability);
    worldState.cp = std::min(worldState.cp, worldState.crafter.cp);

    worldState.progress = std::max(worldState.progress, 0);
    worldState.quality = std::max(worldState.quality, 0);
    worldState.durability = std::max(worldState.durability, 0);
    worldState.cp = std::max(worldState.cp, 0);
}