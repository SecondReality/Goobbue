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
    return cp >= action.cpCost;
}