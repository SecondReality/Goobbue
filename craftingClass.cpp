#include "craftingClass.h"

#include <iostream>

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
