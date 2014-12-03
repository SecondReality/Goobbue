#include <iostream>
#include "crafter.h"

Crafter readCrafter(Json::Value root)
{
    Crafter crafter = Crafter();

    // Read the cross class actions:
    if(root.isMember("crossClassActions"))
    {
        Json::Value crossClassActionValue = root["crossClassActions"];
        for(int i = 0; i < crossClassActionValue.size(); ++i)
        {
            Action::Identifier actionIdentifier = stringToActionIdentifier(crossClassActionValue[i].asString());
            crafter.actions.push_back(actionIdentifier);
        }
    }

    crafter.craftingClass = stringToCraftingClass(root["class"].asString());
    crafter.level = root["level"].asInt();
    crafter.craftsmanship = root["craftsmanship"].asInt();
    crafter.control = root["control"].asInt();
    crafter.cp = root["cp"].asInt();

    return crafter;
}

std::map<CraftingClass, Crafter> readCrafters(Json::Value root, const std::vector<Action>& actions)
{
    std::map<CraftingClass, Crafter> crafterMap;

    for(int i = 0; i < root.size(); ++i)
    {
       Crafter crafter = readCrafter(root[i]);

        for(int n=0; n<actions.size(); ++n)
        {
            Action action = actions[n];
            if( (action.craftingClass == crafter.craftingClass || action.craftingClass == All) && action.level<=crafter.level)
            {
                crafter.actions.push_back(actions[n].identifier);
            }
        }

       crafterMap[crafter.craftingClass] = crafter;
    }

    return crafterMap;
}

void Crafter::print()
{
    std::cout << "Crafter: " << craftingClassToString(craftingClass) << " level: " << level << " craftmanship: " << craftsmanship << " control: " << control << " cp: " << cp << " [";

    for(Action::Identifier actionIdentifier : actions)
    {
        std::cout << actionIdentifierToString(actionIdentifier) << ", ";
    }

    std::cout << "]" << std::endl;
}
