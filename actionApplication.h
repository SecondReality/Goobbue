#pragma once
#include "worldState.h"

struct Outcome
{
    float probability;
    WorldState worldState;
};

typedef std::pair<Outcome, Outcome> Outcomes;

Outcomes ApplyAction(const WorldState& worldState, Action action);