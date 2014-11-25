#pragma once
#include <string>

enum CraftingClass
{
    All,
    Alchemist,
    Armorer,
    Blacksmith,
    Carpenter,
    Culinarian,
    Goldsmith,
    Leatherworker,
    Weaver
};

CraftingClass stringToCraftingClass(std::string s);