#pragma once
#include <string>

struct Recipe
{
    std::string name;
    int maxQuality;
    int level;
    int durability;
    int difficulty;

    // For testing purposes this is an Iron Ingot recipe:
    Recipe()
    {
        maxQuality = 726;
        name = "Iron Ingot";
        level = 13;
        durability = 40;
        difficulty = 27;
    }
};