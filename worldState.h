#pragma once
#include "recipe.h"
#include "crafter.h"
#include "effects.h"

#include <iostream>

struct WorldState
{
    Crafter crafter;
    Recipe recipe;
    Effects effects;
    int durability;
    int quality;
    int progress;
    int cp;
    enum Condition { Poor, Normal, Good, Excellent };
    Condition condition;

    void print()
    {
        std::cout << "WorldState: durability " << durability << " Quality " << quality << " Progress " << progress << " Cp " << cp << " Buffs " << effects.countDowns.size() + effects.countUps.size() << std::endl;
    }
};
