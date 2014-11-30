#include <iostream>
#include <map>
#include "ThirdParty/json.h"
#include "craftingClass.h"
#include "action.h"
#include "jsonLoader.h"
#include "recipe.h"
#include "crafter.h"
#include "worldState.h"
#include "actionApplication.h"
#include "expectimax.h"

// Applies data from a recipe and crafter to the world state:
void initializeWorldState(WorldState &worldState, Crafter crafter, Recipe recipe)
{
    worldState.condition = WorldState::Condition::Normal;
    worldState.cp = crafter.cp;
    worldState.crafter = crafter;
    worldState.recipe = recipe;

    worldState.durability = recipe.durability;
    worldState.progress = 0;
    worldState.quality = 0;
}

void mainMenu();
void configureCrafters();
void performCraft();

int main()
{
    std::cout << "Welcome to Goobbue!" << std::endl;
    mainMenu();
    return 0;
}

void mainMenu()
{
    while(true)
    {
        std::cout << "-- Main Menu --" << std::endl;
        std::cout << "\t1. Configure your crafters." << std::endl;
        std::cout << "\t2. Perform a craft" << std::endl;
        std::cout << "Please enter a choice (1 or 2):" << std::endl;

        char choice;
        std::cin >> choice;

        if (choice == '1')
        {
            configureCrafters();
        }
        else if (choice == '2')
        {
            performCraft();
        }
        else
        {
            std::cout << "Unrecognized selection, quitting (goodbye!)" << std::endl;
            break;
        }
    }
}

void configureCrafters()
{
    std::cout << "-- Crafter Configuration --" << std::endl;
    std::cout << "Select your crafting class:" << std::endl;

    std::cout << "\t1. Alchemist" << std::endl;
    std::cout << "\t2. Armorer" << std::endl;
    std::cout << "\t3. Blacksmith" << std::endl;
    std::cout << "\t4. Carpenter" << std::endl;
    std::cout << "\t5. Culinarian" << std::endl;
    std::cout << "\t6. Goldsmith" << std::endl;
    std::cout << "\t7. Leatherworker" << std::endl;
    std::cout << "\t8. Weaver" << std::endl;
    std::cout << "Please enter a choice (1 to 8):" << std::endl;

    char choice;
    std::cin >> choice;

    if(choice>'8' || choice <'1')
    {
        std::cout << "Unrecognized selection, quitting" << std::endl;
        return;
    }

    CraftingClass craftingClass = (CraftingClass)(choice-'1');
    std::cout << "Configure your " << craftingClassToString(craftingClass) << std::endl;
    std::cout << "Feature not supported, coming soon!" << std::endl;
}

typedef std::vector<Action> ActionVector;

Crafter selectCrafter(const std::string& dataPath, const ActionVector& actions)
{
    std::map<CraftingClass, Crafter> crafters = readCrafters(loadJson(dataPath + "Crafters.json"), actions);

    Crafter crafter;

    while(true)
    {
        std::cout << "Enter the name of your crafting class" << std::endl;
        std::string craftingClassName;
        std::cin >> craftingClassName;

        CraftingClass craftingClass = stringToCraftingClass(craftingClassName);
        if(craftingClass==CraftingClass::All)
        {
            continue;
        }
        else if(crafters.end()==crafters.find(craftingClass))
        {
            std::cout << "Crafter does not exist in JSON file" << std::endl;
        }
        else
        {
            crafter = crafters[craftingClass];
            crafter.print();
            break;
        }
    }

    return crafter;
}

Recipe selectRecipe(const std::string& dataPath, CraftingClass craftingClass)
{
    Recipe recipe;

    while(true)
    {
        std::cin.ignore();
        std::cout << "Enter the name of the recipe" << std::endl;
        std::string recipeName;
        std::getline(std::cin, recipeName);
        std::cout << "Searching for " << recipeName << std::endl;

        if(readRecipe(loadJson(dataPath + "Recipes.json"), craftingClass, recipeName, recipe))
        {
            std::cout << "Found recipe." << std::endl;
            recipe.print();
            break;
        }
        else
        {
            std::cout << "Could not find specified recipe" << std::endl;
        }
    }

    return recipe;
}

int getStartingQuality()
{
    std::cout << "Enter the starting quality" << std::endl;
    int quality = 0;
    std::cin >> quality;
    return quality;
}

void performCraftingAlgorithm(const WorldState & worldStateIn, const ActionVector& actions)
{
    WorldState worldState = worldStateIn;

    std::cout << "Starting Craft!" << std::endl;

    while(true)
    {
        Expectimax expectimax;
        expectimax.actions = actions;

        worldState.print();

        Action::Identifier id = expectimax.evaluateAction(worldState);

        std::cout << "Use the " << actions[id].name << " skill!" << std::endl;
        Outcomes outcomes = applyAction(worldState, actions[id]);

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

        if(worldState.condition==WorldState::Condition::Normal && worldState.quality < worldState.recipe.maxQuality)
        {
            while(true)
            {
                std::cout << "What is the condition: (p)oor, (n)ormal, (g)ood or (e)xcellent?" << std::endl;
                char condition;
                std::cin >> condition;

                switch (condition)
                {
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
                    default:
                        std::cout << "Condition not understood, try again" << std::endl;
                        continue;
                }

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
}

bool yesNoQuestion(const std::string& text)
{
    std::cout << text << " (y or n)" << std::endl;
    char decision;
    std::cin >> decision;

    if(decision=='y')
    {
        return true;
    }

    return false;
}

void performCraft()
{
    std::string dataPath = "E:/Dropbox/Goobbue/Data/";
    ActionVector actions = readActions(loadJson(dataPath + "Skills.json"));

    while (true)
    {
        Crafter crafter = selectCrafter(dataPath, actions);

        while (true)
        {
            Recipe recipe = selectRecipe(dataPath, crafter.craftingClass);

            WorldState worldState;
            initializeWorldState(worldState, crafter, recipe);
            worldState.quality = getStartingQuality();

            while (true)
            {
                performCraftingAlgorithm(worldState, actions);

                if (!yesNoQuestion("Repeat the craft with the same recipe and quality?"))
                {
                    break;
                }
            }

            if (!yesNoQuestion("Repeat the craft with the same crafter?"))
            {
                break;
            }
        }

        if (yesNoQuestion("Go to the main menu?"))
        {
            break;
        }

    }
}
/*
Outcomes outcomes = applyAction(worldState, actions[Action::Identifier::greatStrides]);
outcomes = applyAction(outcomes.first.worldState, actions[Action::Identifier::steadyHand]);
outcomes.first.worldState.condition = WorldState::Condition::Good;
outcomes = applyAction(outcomes.first.worldState, actions[Action::Identifier::standardTouch]);
std::cout << "Quality: " << outcomes.first.worldState.quality << std::endl;
*/