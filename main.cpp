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

// Action -> Reaction -> Condition








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

    std::vector<Action::Identifier> crafterActions;
    //for(CraftingData::Action & action : actions)
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

    // Apply the selected recipe to the world state:
    //Evaluate(worldState, 10);
    worldState.print();
    Outcomes outcomes = ApplyAction(worldState, actions[Action::Identifier::basicSynth]);
    //outcomes = ApplyAction(outcomes.first.worldState, actions[Action::Identifier::basicSynth]);
    outcomes.first.worldState.print();

    std::cout << "Press return to exit." << endl;
    int a;
    std::cin >> a;
    return 0;
}