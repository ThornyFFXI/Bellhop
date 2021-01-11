#include "Bellhop.h"

void Bellhop::Help(vector<string> Args, int ArgCount, CommandHelp HelpText)
{
    if (ArgCount > 2)
    {
        auto iter = mCommandMap.find(Args[2]);
        if (iter != mCommandMap.end())
        {
            PrintHelpText(iter->second.help, true);
            return;
        }
    }

    pOutput->message("Command List");
    for (auto iter = mCommandMap.begin(); iter != mCommandMap.end(); iter++)
    {
        PrintHelpText(iter->second.help, false);
    }
}