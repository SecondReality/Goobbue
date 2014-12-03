#include "action.h"

#include <iostream>
#include <algorithm>

std::vector<std::string> actionNames()
{
    std::vector<std::string> names = {
            "basicSynth",
            "standardSynthesis",
            "flawlessSynthesis",
            "carefulSynthesis",
            "carefulSynthesis2",
            "pieceByPiece",
            "rapidSynthesis",
            "basicTouch",
            "standardTouch",
            "advancedTouch",
            "hastyTouch",
            "byregotsBlessing",
            "comfortZone",
            "rumination",
            "mastersMend",
            "mastersMend2",
            "wasteNot",
            "wasteNot2",
            "manipulation",
            "innerQuiet",
            "steadyHand",
            "steadyHand2",
            "ingenuity",
            "ingenuity2",
            "greatStrides",
            "innovation",
            "tricksOfTheTrade"
    };
    return names;
}

std::string actionIdentifierToString(Action::Identifier actionIdentifier)
{
    return actionNames()[actionIdentifier];
}

Action::Identifier stringToActionIdentifier(std::string actionIdentifierString)
{
    std::vector<std::string> actionNameVector = actionNames();

    auto it = std::find(actionNameVector.begin(), actionNameVector.end(), actionIdentifierString);
    if(actionNameVector.end()==it)
    {
        std::cout << "Action identifier not found!" << std::endl;
    }
    else
    {
        return (Action::Identifier)std::distance(actionNameVector.begin(), it);
    }

    // TODO: Return error code or throw exception.
    return Action::Identifier::advancedTouch;
}

void Action::print()
{
    std::cout << "Action: " << shortName << " " << name << " " << level << " " << common << std::endl;
}

std::vector<Action> readActions(Json::Value root)
{
    std::vector<Action> actions;
    for(int i = 0; i < root.size(); ++i)
    {
        Action action;
        action.shortName = root[i]["shortName"].asString();
        action.name = root[i]["name"].asString();
        action.craftingClass = stringToCraftingClass(root[i]["cls"].asString());
        action.level = root[i]["level"].asInt();
        action.common = root[i]["common"].asBool();

        // Read data array:
        Json::Value data = root[i]["data"];

        action.durabilityCost = data[0].asInt();
        action.cpCost = data[1].asInt();
        action.successProbability = data[2].asFloat();
        action.qualityIncreaseMultiplier = data[3].asFloat();
        action.progressIncreaseMultiplier = data[4].asFloat();

        std::string  effectTypeString = data[5].asString();

        if(effectTypeString=="immediate")
        {
            action.effectType = Action::EffectType::Immediate;
        }
        else if(effectTypeString=="countup")
        {
            action.effectType = Action::EffectType::CountUp;
        }
        else if(effectTypeString=="countdown")
        {
            action.effectType = Action::EffectType::CountDown;
        }
        else
        {
            std::cout << "Bad effect type descriptor " << effectTypeString << std::endl;
        }

        action.activeTurns = data[6].asInt();
        action.identifier = (Action::Identifier)i;

        actions.push_back(action);
    }
    return actions;
}