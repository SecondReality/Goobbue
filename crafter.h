#pragma once
#include "action.h"
#include "ThirdParty/json.h"

struct Crafter
{
    CraftingClass craftingClass;
    int level;
    int craftsmanship;
    int control;
    int cp;
    std::vector<Action::Identifier> actions;

    void print();
};

std::map<CraftingClass, Crafter> readCrafters(Json::Value root, const std::vector<Action>& allActions);