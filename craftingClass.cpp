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

std::string craftingClassToString(CraftingClass craftingClass)
{
    switch(craftingClass)
    {
        case All: return "All";
        case Alchemist: return "Alchemist";
        case Armorer: return "Armorer";
        case Blacksmith: return "Blacksmith";
        case Carpenter: return "Carpenter";
        case Culinarian: return "Culinarian";
        case Goldsmith: return "Goldsmith";
        case Leatherworker: return "Leatherworker";
        case Weaver: return "Weaver";
    }

    return "[unknown crafting class]";
}
