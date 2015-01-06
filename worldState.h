#pragma once
#include "recipe.h"
#include "crafter.h"
#include "effects.h"

#include <iostream>

struct Action;

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

    void print();

    bool isTerminal() const;

    bool canUseAction(const Action& action) const;
    float hqPercentage() const;
};

