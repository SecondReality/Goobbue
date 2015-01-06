#include "worldState.h"

void WorldState::print()
{
    std::cout << "WorldState " << (isTerminal() ? "(terminal)" : "") << ": durability " << durability << " Quality " << quality << " Progress " << progress << " Cp " << cp << " Buffs " << effects.countDowns.size() + effects.countUps.size() << std::endl;
}

bool WorldState::isTerminal() const
{
    if(0>=durability)
    {
        return true;
    }

    return progress>=recipe.difficulty;
}

bool WorldState::canUseAction(const Action& action) const
{
    // We can only use tricks of the trade when the condition is good:
    if(Action::Identifier::tricksOfTheTrade==action.identifier)
    {
        return Condition::Good == condition;
    }

    return cp >= action.cpCost;
}

float WorldState::hqPercentage() const
{
    /*
    TODO: Finish.
    float qualityPercent = quality / recipe.maxQuality * 100;

    float hqPercent = 1;

    if(qualityPercent==0)
    {
        hqPercent=1;
    }
    else if(qualityPercent>=100)
    {
        hqPercent=100;
    }
    else
    {
        while (qualityFromHqPercent(hqPercent) < qualityPercent && hqPercent < 100) {
            hqPercent += 1;
        }
    }
    return hqPercent;
    */
    return 0;
}