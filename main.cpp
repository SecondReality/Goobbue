// Goobbue : Gives you the optimal instructions to create crafts.
// FFXIV Craft source file locations:
// actions.js: skills and actions
// recipedb.js: All recipes

#include <iostream>
#include "ThirdParty/json.h"

#include <vector>
#include <map>
#include <math.h>

using namespace std;

#include "craftingClass.h"
#include "action.h"
#include "jsonLoader.h"
#include "recipe.h"
#include "crafter.h"
#include "effects.h"
#include "worldState.h"
#include "actionApplication.h"
#include "expectimax.h"

// Action -> Reaction -> Condition

// Applies data from a recipe to the world state:
void ApplyRecipe(WorldState & worldState, Crafter crafter, Recipe recipe)
{
    worldState.cp = crafter.cp;
    worldState.crafter = crafter;
    worldState.recipe = recipe;

    worldState.durability = recipe.durability;
    worldState.progress = 0;
    worldState.quality = 0;
}

int main()
{
    std::cout << "Welcome to Goobbue!" << std::endl;

    // Test reading some json:
    std::string dataPath = "E:/Dropbox/Goobbue/Data/";

    typedef std::vector<Action> ActionVector;
    ActionVector actions = ReadActions(loadJson(dataPath+"Skills.json"));

    std::vector<Action::Identifier> crafterActions;

    /*
    for(int i=0; i<actions.size(); ++i)
    {
        Action action = actions[i];
        if(action.craftingClass == Armorer || action.craftingClass == All)
        {
            crafterActions.push_back((Action::Identifier)i);
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
    */

    for(int i=0; i<actions.size(); ++i)
    {
        Action action = actions[i];
        if( (action.craftingClass == Blacksmith || action.craftingClass == All) && action.level<=25)
        {
            crafterActions.push_back((Action::Identifier)i);
        }
    }

    crafterActions.push_back(Action::Identifier::rapidSynthesis);
    crafterActions.push_back(Action::Identifier::hastyTouch);
    crafterActions.push_back(Action::Identifier::wasteNot);
    crafterActions.push_back(Action::Identifier::manipulation);

    std::cout << "Loaded with : " << crafterActions.size() << " actions." << std::endl;

    // For now we just give the class skills to our crafter:

    Crafter crafter;
    /*
    crafter.actions = crafterActions;
    crafter.level = 18;
    crafter.craftsmanship = 68;
    crafter.control = 87;
    crafter.cp = 237+17;
    */

    crafter.actions = crafterActions;
    crafter.level = 25;
    crafter.craftsmanship = 136;
    crafter.control = 140;
    crafter.cp = 261+17;


    Recipe recipe;

    /*
   // Hard leather
    recipe.difficulty = 20;
    recipe.durability = 40;
    recipe.maxQuality = 866;
    recipe.level = 16;
    */

    /*
    // Hard Leather belt:
    recipe.difficulty = 63;
    recipe.durability = 70;
    recipe.maxQuality = 526;
    recipe.level = 16;
    */


    // Iron ingot
    recipe.difficulty = 31;
    recipe.durability = 40;
    recipe.maxQuality = 866;
    recipe.level = 16;


    /*
    // Initiates Head Knife
    recipe.difficulty = 68;
    recipe.durability = 70;
    recipe.maxQuality = 982;
    recipe.level = 19;
    */

    WorldState worldState;
    worldState.condition = WorldState::Condition::Normal;
    ApplyRecipe(worldState, crafter, recipe);

    //worldState.quality = 376;

    //worldState.durability = 5;
    //worldState.quality = 785;
    //worldState.cp = 22;

    /*
    Crafter crafter;
    crafter.actions = crafterActions;
    crafter.level = 17;
    crafter.craftsmanship = 68;
    crafter.control = 87;
    crafter.cp = 237+17;

    Recipe recipe;
    recipe.difficulty = 20;
    recipe.durability = 40;
    recipe.maxQuality = 526;
    recipe.level = 8;

    WorldState worldState;
    worldState.condition = WorldState::Condition::Normal;

    ApplyRecipe(worldState, crafter, recipe);
    worldState.effects.countDowns[Action::Identifier::steadyHand]=2;
    worldState.cp =104;
    worldState.durability = 40;
    worldState.quality = 0;
    */


    /*
    Outcomes outcomes = ApplyAction(worldState, actions[Action::Identifier::basicTouch]);

    outcomes = ApplyAction(outcomes.first.worldState, actions[Action::Identifier::basicTouch]);
    outcomes.first.worldState.print();
    outcomes = ApplyAction(outcomes.first.worldState, actions[Action::Identifier::basicTouch]);
    outcomes.first.worldState.print();
    outcomes = ApplyAction(outcomes.first.worldState, actions[Action::Identifier::basicTouch]);
    outcomes.first.worldState.print();
   // outcomes = applyAction(outcomes.first.worldState, actions[Action::Identifier::basicTouch]);
    outcomes.first.worldState.print();
    */

    while(true)
    {
        Expectimax expectimax;
        expectimax.actions = actions;

        worldState.print();

        Action::Identifier id = expectimax.evaluateAction(worldState);

        std::cout << "Use the " << actions[id].name << " skill!" << std::endl;
        Outcomes outcomes = applyAction(worldState, actions[id], true);

        if(outcomes.first.worldState.durability<=0)
        {
            std::cout << "Congratulations on finishing your craft! (durability)" << std::endl;

            outcomes.first.worldState.print();
            break;
        }

        if(outcomes.first.probability==1.0f)
        {
            worldState = outcomes.first.worldState;
        }
        else
        {
            std::cout << "Did the action succeed? (y or n)" << std::endl;
            char decision;
            std::cin >> decision;
            if(decision=='y')
            {
                worldState = outcomes.first.worldState;
            }
            else if(decision=='n')
            {
                worldState = outcomes.second.worldState;
            }
            else
            {
                break;
            }
        }

        if(worldState.progress >= worldState.recipe.difficulty)
        {
            worldState.print();

            if(worldState.quality < worldState.recipe.maxQuality)
            {
                std::cout << "Congratulations on finishing your craft! (progress)" << std::endl;
            }
            else
            {
                std::cout << "Congratulations on HQing your craft!" << std::endl;
            }
            break;
        }

        if(worldState.condition==WorldState::Condition::Normal)
        {
            std::cout << "What is the condition (p, n, g or e)" << std::endl;
            char condition;
            std::cin >> condition;
            switch (condition) {
                case 'p':
                    worldState.condition = WorldState::Condition::Poor;
                    break;
                case 'n':
                    worldState.condition = WorldState::Condition::Normal;
                    break;
                case 'g':
                    worldState.condition = WorldState::Condition::Good;
                    break;
                case 'e':
                    worldState.condition = WorldState::Condition::Excellent;
                    break;
            }
        }
        else
        {
            if(worldState.condition==WorldState::Condition::Excellent)
            {
                worldState.condition=WorldState::Condition::Poor;
            }
            else if(worldState.condition==WorldState::Condition::Good || worldState.condition==WorldState::Condition::Poor)
            {
                worldState.condition=WorldState::Condition::Normal;
            }
        }
    }

    std::cout << "End" << endl;
    return 0;
}