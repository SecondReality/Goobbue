#pragma once
#include "worldState.h"

struct Outcome
{
    float probability;
    WorldState worldState;
};

typedef std::pair<Outcome, Outcome> Outcomes;

Outcomes applyAction(const WorldState &worldState, Action action);
float actionSuccessProbability(const WorldState& worldState, Action action);