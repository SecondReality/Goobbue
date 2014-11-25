#pragma once

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
        return countDowns.find(actionIdentifier) != countDowns.end();
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