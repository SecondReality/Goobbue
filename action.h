#pragma once

#include "craftingClass.h"

#include <vector>
#include <string>
#include "ThirdParty/json.h"

struct Action
{
    enum EffectType { Immediate, CountUp, CountDown };

    std::string shortName;
    std::string name;
    CraftingClass craftingClass;
    int level;
    bool common;

    // Values read from the data array:
    int durabilityCost;
    int cpCost;
    float successProbability;
    float qualityIncreaseMultiplier;
    float progressIncreaseMultiplier;
    EffectType effectType;
    int activeTurns;

    enum Identifier
    {
        basicSynth = 0,
        standardSynthesis,
        flawlessSynthesis,
        carefulSynthesis,
        carefulSynthesis2,
        pieceByPiece,
        rapidSynthesis,
        basicTouch,
        standardTouch,
        advancedTouch,
        hastyTouch,
        byregotsBlessing,
        comfortZone,
        rumination,
        mastersMend,
        mastersMend2,
        wasteNot,
        wasteNot2,
        manipulation,
        innerQuiet,
        steadyHand,
        steadyHand2,
        ingenuity,
        ingenuity2,
        greatStrides,
        innovation,
        tricksOfTheTrade
    };

    Identifier identifier;

    void print();
};

Action::Identifier stringToActionIdentifier(std::string actionIdentifierString);
std::string actionIdentifierToString(Action::Identifier actionIdentifier);
std::vector<std::string> actionNames();
std::vector<Action> readActions(Json::Value root);