#pragma once
#include <string>
#include <map>
#include "ThirdParty/json.h"
#include "craftingClass.h"

struct Recipe
{
    std::string name;
    int maxQuality;
    int level;
    int durability;
    int difficulty;

    void print();
};

bool readRecipe(Json::Value root, CraftingClass craftingClass, std::string recipeName, Recipe & recipe);