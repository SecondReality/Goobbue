#pragma once
#include "action.h"

struct Crafter
{
    CraftingClass craftingClass;
    int level;
    int craftsmanship;
    int control;
    int cp;
    std::vector<Action::Identifier> actions;
};
