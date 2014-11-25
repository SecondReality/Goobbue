// Goobbue : Gives you the optimal instructions to create crafts.
// FFXIV Craft source file locations:
// actions.js: skills and actions
// recipedb.js: All recipes

#include <iostream>
#include "ThirdParty/json.h"
#include <sstream>
#include <fstream>
#include <vector>
#include <map>
#include <math.h>

using namespace std;

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

    CraftingClass stringToCraftingClass(std::string s)
    {
        if(s=="Alchemist")
        {
            return Alchemist;
        }
        else if(s=="Armorer")
        {
            return Armorer;
        }
        else if(s=="Blacksmith")
        {
            return Blacksmith;
        }
        else if(s=="Carpenter")
        {
            return Carpenter;
        }
        else if(s=="Culinarian")
        {
            return Culinarian;
        }
        else if(s=="Goldsmith")
        {
            return Goldsmith;
        }
        else if(s=="Leatherworker")
        {
            return Leatherworker;
        }
        else if(s=="Weaver")
        {
            return Weaver;
        }
        else if(s=="All")
        {
            return All;
        }

        std::cout << "Warning: Unknown crafting profession " << s << std::endl;
        return All;
    }

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

        void print()
        {
            std::cout << "Action: " << shortName << " " << name << " " << level << " " << common << std::endl;
        }
    };

    std::vector<Action> ReadActions(Json::Value root)
    {
        std::vector<Action> actions;
        for (int i = 0; i < root.size(); ++i)
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

Json::Value loadJson(const std::string& filePath)
{
    std::ifstream fileStream(filePath);
    std::stringstream buffer;
    buffer << fileStream.rdbuf();
    fileStream.close();

    Json::Value root;
    Json::Reader reader;
    bool parsingSuccessful = reader.parse(buffer.str(), root);

    if (!parsingSuccessful)
    {
        std::cout << "Error parsing json file at " << filePath << std::endl;
    }

    return root;
}

typedef int ActionIdentifier;

struct Recipe
{
    string name;
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

struct Crafter
{
    CraftingClass craftingClass;
    int level;
    int craftsmanship;
    int control;
    int cp;
    std::vector<ActionIdentifier> actions;
};

struct Effects
{
    std::map<Action::Identifier, int> countUps;
    std::map<Action::Identifier, int> countDowns;

    bool countUpsContainsAction(Action::Identifier actionIdentifier) const
    {
        return countUps.find(actionIdentifier) != countUps.end();
    }

    bool countDownsContainsAction(Action::Identifier actionIdentifier) const
    {
        return countDowns.find(actionIdentifier) != countUps.end();
    }

    int getCountUpsValue(Action::Identifier actionIdentifier) const
    {
        return countUps.at(actionIdentifier);
    }

    int getCountDownsValue(Action::Identifier actionIdentifier) const
    {
        return countDowns.at(actionIdentifier);
    }

    void removeCountUp(Action::Identifier actionIdentifier)
    {
        countUps.erase(actionIdentifier);
    }

    void removeCountDown(Action::Identifier actionIdentifier)
    {
        countDowns.erase(actionIdentifier);
    }

    void updateCountDowns()
    {
        for(auto &kv : countDowns)
        {
            kv.second -= 1;
            if (kv.second <= 0)
            {
                countDowns.erase(kv.first);
            }
        }
    }

};

// Not included: stepCount, reliabilityOk,
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

    void print()
    {
        std::cout << "WorldState: durability " << durability << " Quality " << quality << " Progress " << progress << std::endl;
    }
};

// Action -> Rection -> Condition

struct Outcome
{
    float probability;
    WorldState worldState;
};

float round(float d)
{
    return floor(d + 0.5f);
}

float calculateBaseProgressIncrease(int levelDifference, int craftsmanship)
{
    float levelCorrectionFactor = 0;

    if((levelDifference < -5))
    {
        levelCorrectionFactor = 0.0152f * levelDifference;
    }
    else if ((-5 <= levelDifference) && (levelDifference <= 0))
    {
        levelCorrectionFactor = 0.10f * levelDifference;
    }
    else if ((0 < levelDifference) && (levelDifference <= 5))
    {
        levelCorrectionFactor = 0.05f * levelDifference;
    }
    else if ((5 < levelDifference) && (levelDifference <= 15))
    {
        levelCorrectionFactor = 0.022f * levelDifference + 0.15f;
    }
    else
    {
        levelCorrectionFactor = 0.0033f * levelDifference + 0.43f;
    }

    float baseProgress = 0.21f * craftsmanship + 1.6f;
    float levelCorrectedProgress = baseProgress * (1 + levelCorrectionFactor);

    return round(levelCorrectedProgress);
}

float calculateBaseQualityIncrease(int levelDifference, int control)
{
    float levelCorrectionFactor = 0;

    if (levelDifference < -20)
    {
        levelCorrectionFactor = 0.02867f * levelDifference;
    }
    else if (-20 <= levelDifference && levelDifference < -5)
    {
        levelCorrectionFactor = 0.02867f * levelDifference;
    }
    else if ((-5 <= levelDifference) && (levelDifference <= 0))
    {
        levelCorrectionFactor = 0.05f * levelDifference;
    }
    else
    {
        levelCorrectionFactor = 0;
    }

    float baseQuality = 0;
    if(levelDifference < -20)
    {
        baseQuality = 0.6642f * control + 41.80f;
    }
    else if(-20 <= levelDifference && levelDifference < -5)
    {
        baseQuality = 0.6642f * control + 41.80f;
    }
    else
    {
        baseQuality = 0.36f * control + 34;
    }
    float levelCorrectedQuality = baseQuality * (1 + levelCorrectionFactor);

    return round(levelCorrectedQuality);
}

void SimulateAction(WorldState& worldState, Action action, float success);

typedef std::pair<Outcome, Outcome> Outcomes;

Outcomes ApplyAction(const WorldState& worldState, Action action)
{
    const Effects &effects = worldState.effects;

    float successProbability = 0.0f;

    if (effects.countDownsContainsAction(Action::Identifier::steadyHand2))
    {
        successProbability = action.successProbability + 0.3f;
    }
    else if(effects.countDownsContainsAction(Action::Identifier::steadyHand))
    {
        successProbability = action.successProbability + 0.2f;
    }
    else
    {
        successProbability = action.successProbability;
    }

    successProbability = std::min(successProbability, 1.0f);

    float failureProbability = 1.0f - successProbability;

    Outcome successWorld;
    successWorld.worldState = worldState;
    successWorld.probability = successProbability;

    Outcome failureWorld;
    failureWorld.worldState = worldState;
    failureWorld.probability = failureProbability;

    SimulateAction(successWorld.worldState, action, 1.0f);
    SimulateAction(failureWorld.worldState, action, 0.0f);

    return std::make_pair(successWorld, failureWorld);
}

void SimulateAction(WorldState& worldState, Action action, float success)
{
    const Crafter &crafter = worldState.crafter;
    const Recipe &recipe = worldState.recipe;
    const Effects &effects = worldState.effects;

    int craftsmanship = crafter.craftsmanship;
    int control = crafter.control;

    if(effects.countUpsContainsAction(Action::Identifier::innerQuiet))
    {
        control *= (1 + 0.2 * effects.getCountUpsValue(Action::Identifier::innerQuiet));
    }

    if(effects.countDownsContainsAction(Action::Identifier::innovation))
    {
        control *= 1.5;
    }

    int levelDifference = worldState.crafter.level - recipe.level;

    if(effects.countDownsContainsAction(Action::Identifier::ingenuity2) && (levelDifference < -20) && (crafter.level == 50))
    {
        levelDifference = levelDifference + 20;
    }
    else if(effects.countDownsContainsAction(Action::Identifier::ingenuity2))
    {
        levelDifference = 3;
    }
    else if(effects.countDownsContainsAction(Action::Identifier::ingenuity) && (levelDifference < -20) && (crafter.level == 50))
    {
        levelDifference = levelDifference + 10;
    }
    else if(effects.countDownsContainsAction(Action::Identifier::ingenuity))
    {
        levelDifference = 0;
    }

    float qualityIncreaseMultiplier = action.qualityIncreaseMultiplier;

    if(effects.countDownsContainsAction(Action::Identifier::greatStrides))
    {
        qualityIncreaseMultiplier *= 2;
    }

    if(worldState.condition==WorldState::Condition::Poor)
    {
        qualityIncreaseMultiplier *= 0.5;
    }
    else if(worldState.condition==WorldState::Condition::Excellent)
    {
        qualityIncreaseMultiplier *= 4;
    }
    else if(worldState.condition==WorldState::Condition::Good)
    {
        qualityIncreaseMultiplier *= 1.5;
    }
    else if(worldState.condition==WorldState::Condition::Normal)
    {
        qualityIncreaseMultiplier *= 1;
    }
    else
    {
        std::cout << "Error - unknown condition";
    }

    float bProgressGain = action.progressIncreaseMultiplier * calculateBaseProgressIncrease(levelDifference, craftsmanship);

    if(action.identifier==Action::Identifier::flawlessSynthesis)
    {
        bProgressGain = 40;
    }
    else if(action.identifier==Action::Identifier::pieceByPiece)
    {
        bProgressGain = (recipe.difficulty - worldState.progress)/3;
    }

    float progressGain = success * bProgressGain;

    float bQualityGain = qualityIncreaseMultiplier * calculateBaseQualityIncrease(levelDifference, control);
    float qualityGain = success * bQualityGain;

    if(action.identifier==Action::Identifier::byregotsBlessing && effects.countUpsContainsAction(Action::Identifier::innerQuiet))
    {
        qualityGain *= (1 + 0.2f * effects.getCountUpsValue(Action::Identifier::innerQuiet));
    }

    float durabilityCost = action.durabilityCost;

    if(effects.countDownsContainsAction(Action::Identifier::wasteNot) || effects.countDownsContainsAction(Action::Identifier::wasteNot2))
    {
        durabilityCost = 0.5f * action.durabilityCost;
    }

    // State tracking
    worldState.progress += round(progressGain);
    worldState.quality += round(qualityGain);
    worldState.durability -= durabilityCost;
    worldState.cp -= action.cpCost;

    // Effect management
    //==================================
    // Special Effect Actions

    if(action.identifier==Action::Identifier::mastersMend)
    {
        worldState.durability += 30;
    }

    if(action.identifier==Action::Identifier::mastersMend2)
    {
        worldState.durability += 60;
    }

    if(effects.countDownsContainsAction(Action::Identifier::manipulation) && worldState.durability > 0)
    {
        worldState.durability += 10;
    }

    if(effects.countDownsContainsAction(Action::Identifier::comfortZone) && worldState.cp > 0)
    {
        worldState.cp += 8;
    }

    if(action.identifier==Action::Identifier::rumination && worldState.cp > 0)
    {
        if(effects.countUpsContainsAction(Action::Identifier::innerQuiet) && effects.getCountUpsValue(Action::Identifier::innerQuiet) > 0)
        {
            int innerQuietValue = effects.getCountUpsValue(Action::Identifier::innerQuiet);
            worldState.cp += (21 * innerQuietValue - pow(innerQuietValue, 2) + 10) / 2;
            worldState.effects.removeCountUp(Action::Identifier::innerQuiet);
        }
    }

    if(action.identifier==Action::Identifier::byregotsBlessing)
    {
        if(effects.countUpsContainsAction(Action::Identifier::innerQuiet))
        {
            worldState.effects.removeCountUp(Action::Identifier::innerQuiet);
        }
    }

    if(action.qualityIncreaseMultiplier > 0 && effects.countDownsContainsAction(Action::Identifier::greatStrides))
    {
        worldState.effects.removeCountDown(Action::Identifier::greatStrides);
    }

    if(action.identifier==Action::Identifier::tricksOfTheTrade && worldState.cp > 0)
    {
        worldState.cp += 20;
    }

    // Decrement countdowns
    worldState.effects.updateCountDowns();

    // Increment countups
    if(action.qualityIncreaseMultiplier > 0 && effects.countUpsContainsAction(Action::Identifier::innerQuiet))
    {
        worldState.effects.countUps[Action::Identifier::innerQuiet] += 1 * success;
    }

    // Add new action to countUps
    if(action.effectType==Action::EffectType::CountUp)
    {
        worldState.effects.countUps[action.identifier] = 0;
    }

    // Add new action to countDowns
    if(action.effectType==Action::EffectType::CountDown)
    {
        worldState.effects.countDowns[action.identifier] = action.activeTurns;
    }

    // Sanity checks for state variables
    if ((worldState.durability >= -5) && (worldState.progress >= worldState.recipe.difficulty))
    {
        worldState.durability = 0;
    }

    worldState.durability = std::min(worldState.durability, worldState.recipe.durability);
    worldState.cp = std::min(worldState.cp, worldState.crafter.cp);
}

void Evaluate(WorldState worldState, int depth)
{

}

// Applies data from a recipe to the world state:
void ApplyRecipe(WorldState & worldState, Crafter crafter, Recipe recipe)
{
    worldState.crafter = crafter;
    worldState.recipe = recipe;

    worldState.durability = recipe.durability;
    worldState.progress = 0;
    worldState.quality = 0;
}

int main()
{
    std::cout << "Welcome to Goobbue!" << endl;

    // Test reading some json:
    string dataPath = "E:/Dropbox/Goobbue/Data/";

    typedef std::vector<Action> ActionVector;

    ActionVector actions = ReadActions(loadJson(dataPath+"Skills.json"));

    std::vector<ActionIdentifier> crafterActions;
    //for(CraftingData::Action & action : actions)
    for(int i=0; i<actions.size(); ++i)
    {
        Action action = actions[i];
        if(action.craftingClass == Armorer || action.craftingClass == All)
        {
            crafterActions.push_back(i);
        }
    }

    std::cout << "Loaded with : " << crafterActions.size() << " actions." << std::endl;

    // For now we just give the class skills to our crafter:
    Crafter crafter;
    crafter.actions = crafterActions;
    crafter.level = 30;
    crafter.craftsmanship = 200;
    crafter.control = 200;
    crafter.cp = 250;

    Recipe recipe;

    WorldState worldState;
    worldState.condition = WorldState::Condition::Normal;
    ApplyRecipe(worldState, crafter, recipe);

    // Apply the selected recipe to the world state:
    //Evaluate(worldState, 10);
    worldState.print();
    Outcomes outcomes = ApplyAction(worldState, actions[Action::Identifier::basicSynth]);
    outcomes.first.worldState.print();

    std::cout << "Press return to exit." << endl;
    int a;
    std::cin >> a;
    return 0;
}