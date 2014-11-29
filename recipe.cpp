#include "recipe.h"
#include <iostream>

Recipe readRecipe(Json::Value root)
{
    Recipe recipe;
    recipe.name = root["name"].asString();
    recipe.maxQuality = root["maxQuality"].asInt();
    recipe.level = root["level"].asInt();
    recipe.durability = root["durability"].asInt();
    recipe.difficulty = root["difficulty"].asInt();
    return recipe;
}

bool readRecipe(Json::Value root, CraftingClass craftingClass, std::string recipeName, Recipe & recipe)
{
    std::string craftingClassString = craftingClassToString(craftingClass);

    bool found = false;

    if(root.isMember(craftingClassString))
    {
        Json::Value classRecipes = root[craftingClassString];

        for(int i = 0; i < classRecipes.size(); ++i)
        {
            if(classRecipes[i]["name"]==recipeName)
            {
                recipe = readRecipe(classRecipes[i]);
                found = true;
            }
        }
    }

    if(!found)
    {
        return false;
    }

    return true;
}

void Recipe::print()
{
    std::cout << "Recipe: " << name << " level: " << level << " difficulty: " << difficulty << " max quality: " << maxQuality << " durability: " << durability << std::endl;
}