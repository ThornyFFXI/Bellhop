#include "Bellhop.h"

void Bellhop::Default(vector<string> Args, int ArgCount, CommandHelp HelpText)
{
    if (ArgCount < 3)
    {
        PrintHelpText(HelpText, false);
        return;
    }

    if (_stricmp(Args[2].c_str(), "accept") == 0)
    {
        mConfig.SetAutoTradeSetting(AutoTradeSetting::Accept);
        OutputHelper::Output(Ashita::LogLevel::Info, "AutoTrade default set to $Haccept$R.");
    }
    else if (_stricmp(Args[2].c_str(), "deny") == 0)
    {
        mConfig.SetAutoTradeSetting(AutoTradeSetting::Deny);
        OutputHelper::Output(Ashita::LogLevel::Info, "AutoTrade default set to $Hdeny$R.");
    }
    else if (_stricmp(Args[2].c_str(), "ignore") == 0)
    {
        mConfig.SetAutoTradeSetting(AutoTradeSetting::Ignore);
        OutputHelper::Output(Ashita::LogLevel::Info, "AutoTrade default set to $Hignore$R.");
    }
    else
    {
        PrintHelpText(HelpText, false);
        return;
    }
}
void Bellhop::BlackList(vector<string> Args, int ArgCount, CommandHelp HelpText)
{
    if (ArgCount < 3)
    {
        PrintHelpText(HelpText, false);
        return;
    }

    if (_stricmp(Args[2].c_str(), "add") == 0)
    {
        if (ArgCount < 4)
        {
            PrintHelpText(HelpText, false);
            return;
        }

        std::string cleanName = BellhopConfig::FormatName(Args[3]);
        if (cleanName.length() == 0)
        {
            OutputHelper::Outputf(Ashita::LogLevel::Error, "Invalid name used: %s", Args[3].c_str());
            return;
        }

        std::vector<std::string> blacklist = mConfig.GetBlacklist();
        if (std::find(blacklist.begin(), blacklist.end(), cleanName) == blacklist.end())
        {
            blacklist.push_back(cleanName);
            mConfig.SetBlacklist(blacklist);
            OutputHelper::Outputf(Ashita::LogLevel::Info, "$H%s$R added to trade blacklist.", cleanName.c_str());
        }
        else
            OutputHelper::Outputf(Ashita::LogLevel::Info, "$H%s$R was already in your trade blacklist.", cleanName.c_str());
    }

    else if (_stricmp(Args[2].c_str(), "remove") == 0)
    {
        if (ArgCount < 4)
        {
            PrintHelpText(HelpText, false);
            return;
        }

        std::string cleanName = BellhopConfig::FormatName(Args[3]);
        if (cleanName.length() == 0)
        {
            OutputHelper::Outputf(Ashita::LogLevel::Error, "Invalid name used: %s", Args[3].c_str());
            return;
        }

        std::vector<std::string> blacklist = mConfig.GetBlacklist();
        auto iter                          = std::find(blacklist.begin(), blacklist.end(), cleanName);
        if (iter != blacklist.end())
        {
            blacklist.erase(iter);
            mConfig.SetBlacklist(blacklist);
            OutputHelper::Outputf(Ashita::LogLevel::Info, "$H%s$R removed from trade blacklist.", cleanName.c_str());
        }
        else
            OutputHelper::Outputf(Ashita::LogLevel::Info, "$H%s$R was not in your trade blacklist.", cleanName.c_str());
    }

    else if (_stricmp(Args[2].c_str(), "clear") == 0)
    {
        mConfig.SetBlacklist(std::vector<std::string>());
        OutputHelper::Output(Ashita::LogLevel::Info, "Trade blacklist cleared.");
    }

    else if (_stricmp(Args[2].c_str(), "print") == 0)
    {
        std::vector<std::string> blacklist = mConfig.GetBlacklist();
        OutputHelper::Output(Ashita::LogLevel::Info, "$HTrade Blacklist");
        if (blacklist.size() == 0)
            OutputHelper::Output(Ashita::LogLevel::Info, "[empty]");
        for (std::vector<string>::iterator iter = blacklist.begin(); iter != blacklist.end(); iter++)
        {
            OutputHelper::Outputf(Ashita::LogLevel::Info, "%s", iter->c_str());
        }
    }

    else
    {
        PrintHelpText(HelpText, false);
        return;
    }
}
void Bellhop::WhiteList(vector<string> Args, int ArgCount, CommandHelp HelpText)
{
    if (ArgCount < 3)
    {
        PrintHelpText(HelpText, false);
        return;
    }

    if (_stricmp(Args[2].c_str(), "add") == 0)
    {
        if (ArgCount < 4)
        {
            PrintHelpText(HelpText, false);
            return;
        }

        std::string cleanName = BellhopConfig::FormatName(Args[3]);
        if (cleanName.length() == 0)
        {
            OutputHelper::Outputf(Ashita::LogLevel::Error, "Invalid name used: %s", Args[3].c_str());
            return;
        }

        std::vector<std::string> whitelist = mConfig.GetWhitelist();
        if (std::find(whitelist.begin(), whitelist.end(), cleanName) == whitelist.end())
        {
            whitelist.push_back(cleanName);
            mConfig.SetWhitelist(whitelist);
            OutputHelper::Outputf(Ashita::LogLevel::Info, "$H%s$R added to trade whitelist.", cleanName.c_str());
        }
        else
            OutputHelper::Outputf(Ashita::LogLevel::Info, "$H%s$R was already in your trade whitelist.", cleanName.c_str());
    }

    else if (_stricmp(Args[2].c_str(), "remove") == 0)
    {
        if (ArgCount < 4)
        {
            PrintHelpText(HelpText, false);
            return;
        }

        std::string cleanName = BellhopConfig::FormatName(Args[3]);
        if (cleanName.length() == 0)
        {
            OutputHelper::Outputf(Ashita::LogLevel::Error, "Invalid name used: %s", Args[3].c_str());
            return;
        }

        std::vector<std::string> whitelist = mConfig.GetWhitelist();
        auto iter                          = std::find(whitelist.begin(), whitelist.end(), cleanName);
        if (iter != whitelist.end())
        {
            whitelist.erase(iter);
            mConfig.SetWhitelist(whitelist);
            OutputHelper::Outputf(Ashita::LogLevel::Info, "$H%s$R removed from trade whitelist.", cleanName.c_str());
        }
        else
            OutputHelper::Outputf(Ashita::LogLevel::Info, "$H%s$R was not in your trade whitelist.", cleanName.c_str());
    }

    else if (_stricmp(Args[2].c_str(), "clear") == 0)
    {
        mConfig.SetWhitelist(std::vector<std::string>());
        OutputHelper::Output(Ashita::LogLevel::Info, "Trade whitelist cleared.");
    }

    else if (_stricmp(Args[2].c_str(), "print") == 0)
    {
        std::vector<std::string> whitelist = mConfig.GetWhitelist();
        OutputHelper::Output(Ashita::LogLevel::Info, "$HTrade Whitelist");
        if (whitelist.size() == 0)
            OutputHelper::Output(Ashita::LogLevel::Info, "[empty]");
        for (std::vector<string>::iterator iter = whitelist.begin(); iter != whitelist.end(); iter++)
        {
            OutputHelper::Outputf(Ashita::LogLevel::Info, "%s", iter->c_str());
        }
    }

    else
    {
        PrintHelpText(HelpText, false);
        return;
    }
}
void Bellhop::EnableBags(vector<string> Args, int ArgCount, CommandHelp HelpText)
{
    if (ArgCount < 3)
    {
        PrintHelpText(HelpText, false);
        return;
    }

    if (_stricmp(Args[2].c_str(), "add") == 0)
    {
        if (ArgCount < 4)
        {
            PrintHelpText(HelpText, false);
            return;
        }

        int index = GetContainerIndexFromName(Args[3].c_str());
        if ((index < 0) || (index > CONTAINER_MAX))
        {
            OutputHelper::Outputf(Ashita::LogLevel::Error, "$H%s$R is not a valid bag.", Args[3].c_str());
            return;
        }

        std::vector<int> enabledbags = mConfig.GetEnabledBags();
        if (std::find(enabledbags.begin(), enabledbags.end(), index) != enabledbags.end())
        {
            OutputHelper::Outputf(Ashita::LogLevel::Info, "$H%s$R was already in your force enabled bags.", ContainerNames[index]);
        }
        else
        {
            enabledbags.push_back(index);
            mConfig.SetEnabledBags(enabledbags);
            OutputHelper::Outputf(Ashita::LogLevel::Info, "$H%s$R added to force enabled bags.", ContainerNames[index]);
        }
    }

    else if (_stricmp(Args[2].c_str(), "remove") == 0)
    {
        if (ArgCount < 4)
        {
            PrintHelpText(HelpText, false);
            return;
        }

        int index = GetContainerIndexFromName(Args[3].c_str());
        if ((index < 0) || (index > CONTAINER_MAX))
        {
            OutputHelper::Outputf(Ashita::LogLevel::Error, "$H%s$R is not a valid bag.", Args[3].c_str());
            return;
        }

        std::vector<int> enabledbags = mConfig.GetEnabledBags();
        auto iter                    = std::find(enabledbags.begin(), enabledbags.end(), index);
        if (iter== enabledbags.end())
        {
            OutputHelper::Outputf(Ashita::LogLevel::Info, "$H%s$R was not in your force enabled bags.", ContainerNames[index]);
        }
        else
        {
            enabledbags.erase(iter);
            mConfig.SetEnabledBags(enabledbags);
            OutputHelper::Outputf(Ashita::LogLevel::Info, "$H%s$R removed from force enabled bags.", ContainerNames[index]);
        }
    }

    else if (_stricmp(Args[2].c_str(), "clear") == 0)
    {
        mConfig.SetEnabledBags(std::vector<int>());
        OutputHelper::Output(Ashita::LogLevel::Info, "Force enabled bags cleared.");
    }

    else if (_stricmp(Args[2].c_str(), "print") == 0)
    {
        OutputHelper::Output(Ashita::LogLevel::Info, "$HForce Enabled Bags");
        std::vector<int> enabledbags = mConfig.GetEnabledBags();
        for (std::vector<int>::iterator iter = enabledbags.begin(); iter != enabledbags.end(); iter++)
        {
            OutputHelper::Outputf(Ashita::LogLevel::Info, "%s", ContainerNames[*iter]);
        }
    }

    else
    {
        PrintHelpText(HelpText, false);
        return;
    }
}
void Bellhop::DisableBags(vector<string> Args, int ArgCount, CommandHelp HelpText)
{
    if (ArgCount < 3)
    {
        PrintHelpText(HelpText, false);
        return;
    }

    if (_stricmp(Args[2].c_str(), "add") == 0)
    {
        if (ArgCount < 4)
        {
            PrintHelpText(HelpText, false);
            return;
        }

        int index = GetContainerIndexFromName(Args[3].c_str());
        if ((index < 0) || (index > CONTAINER_MAX))
        {
            OutputHelper::Outputf(Ashita::LogLevel::Error, "$H%s$R is not a valid bag.", Args[3].c_str());
            return;
        }

        std::vector<int> disabledbags = mConfig.GetDisabledBags();
        if (std::find(disabledbags.begin(), disabledbags.end(), index) != disabledbags.end())
        {
            OutputHelper::Outputf(Ashita::LogLevel::Info, "$H%s$R was already in your force disabled bags.", ContainerNames[index]);
        }
        else
        {
            disabledbags.push_back(index);
            mConfig.SetDisabledBags(disabledbags);
            OutputHelper::Outputf(Ashita::LogLevel::Info, "$H%s$R added to force disabled bags.", ContainerNames[index]);
        }
    }

    else if (_stricmp(Args[2].c_str(), "remove") == 0)
    {
        if (ArgCount < 4)
        {
            PrintHelpText(HelpText, false);
            return;
        }

        int index = GetContainerIndexFromName(Args[3].c_str());
        if ((index < 0) || (index > CONTAINER_MAX))
        {
            OutputHelper::Outputf(Ashita::LogLevel::Error, "$H%s$R is not a valid bag.", Args[3].c_str());
            return;
        }

        std::vector<int> disabledbags = mConfig.GetDisabledBags();
        auto iter                     = std::find(disabledbags.begin(), disabledbags.end(), index);
        if (iter == disabledbags.end())
        {
            OutputHelper::Outputf(Ashita::LogLevel::Info, "$H%s$R was not in your force disabled bags.", ContainerNames[index]);
        }
        else
        {
            disabledbags.erase(iter);
            mConfig.SetDisabledBags(disabledbags);
            OutputHelper::Outputf(Ashita::LogLevel::Info, "$H%s$R removed from force disabled bags.", ContainerNames[index]);
        }
    }

    else if (_stricmp(Args[2].c_str(), "clear") == 0)
    {
        mConfig.SetDisabledBags(std::vector<int>());
        OutputHelper::Output(Ashita::LogLevel::Info, "Force disabled bags cleared.");
    }

    else if (_stricmp(Args[2].c_str(), "print") == 0)
    {
        OutputHelper::Output(Ashita::LogLevel::Info, "$HForce Disabled Bags");
        std::vector<int> disabledbags = mConfig.GetDisabledBags();
        for (std::vector<int>::iterator iter = disabledbags.begin(); iter != disabledbags.end(); iter++)
        {
            OutputHelper::Outputf(Ashita::LogLevel::Info, "%s", ContainerNames[*iter]);
        }
    }

    else
    {
        PrintHelpText(HelpText, false);
        return;
    }

}
void Bellhop::Config(vector<string> Args, int ArgCount, CommandHelp HelpText)
{
    if (ArgCount < 3)
    {
        PrintHelpText(HelpText, false);
        return;
    }

    if (_stricmp(Args[2].c_str(), "nomadstorage") == 0)
    {
        if (ArgCount < 4)
        {
            PrintHelpText(HelpText, false);
            return;
        }

        if (_stricmp(Args[3].c_str(), "enable") == 0)
        {
            mConfig.SetNomadStorage(true);
            OutputHelper::Output(Ashita::LogLevel::Info, "NomadStorage $Henabled$R.");
        }
        else if (_stricmp(Args[3].c_str(), "disable") == 0)
        {
            mConfig.SetNomadStorage(false);
            OutputHelper::Output(Ashita::LogLevel::Info, "NomadStorage $Hdisabled$R.");
        }
        else
        {
            OutputHelper::Outputf(Ashita::LogLevel::Error, "You must specify 'enable' or 'disable' to change NomadStorage setting.");
        }
    }

    if (_stricmp(Args[2].c_str(), "ignorecraftskill") == 0)
    {
        if (ArgCount < 4)
        {
            PrintHelpText(HelpText, false);
            return;
        }

        if (_stricmp(Args[3].c_str(), "enable") == 0)
        {
            mConfig.SetIgnoreCraftSkill(true);
            OutputHelper::Output(Ashita::LogLevel::Info, "IgnoreCraftSkill $Henabled$R.");
        }
        else if (_stricmp(Args[3].c_str(), "disable") == 0)
        {
            mConfig.SetIgnoreCraftSkill(false);
            OutputHelper::Output(Ashita::LogLevel::Info, "IgnoreCraftSkill $Hdisabled$R.");
        }
        else
        {
            OutputHelper::Outputf(Ashita::LogLevel::Error, "You must specify 'enable' or 'disable' to change IgnoreCraftSkill setting.");
        }
    }

    if (_stricmp(Args[2].c_str(), "enforcetradewindow") == 0)
    {
        if (ArgCount < 4)
        {
            PrintHelpText(HelpText, false);
            return;
        }

        if (_stricmp(Args[3].c_str(), "enable") == 0)
        {
            mConfig.SetEnforceTradeWindow(true);
            OutputHelper::Output(Ashita::LogLevel::Info, "EnforceTradeWindow $Henabled$R.");
        }
        else if (_stricmp(Args[3].c_str(), "disable") == 0)
        {
            mConfig.SetEnforceTradeWindow(false);
            OutputHelper::Output(Ashita::LogLevel::Info, "EnforceTradeWindow $Hdisabled$R.");
        }
        else
        {
            OutputHelper::Outputf(Ashita::LogLevel::Error, "You must specify 'enable' or 'disable' to change EnforceTradeWindow setting.");
        }
    }

    if (_stricmp(Args[2].c_str(), "legacycommands") == 0)
    {
        if (ArgCount < 4)
        {
            PrintHelpText(HelpText, false);
            return;
        }

        if (_stricmp(Args[3].c_str(), "enable") == 0)
        {
            mConfig.SetLegacyCommands(true);
            OutputHelper::Output(Ashita::LogLevel::Info, "LegacyCommands $Henabled$R.");
        }
        else if (_stricmp(Args[3].c_str(), "disable") == 0)
        {
            mConfig.SetLegacyCommands(false);
            OutputHelper::Output(Ashita::LogLevel::Info, "LegacyCommands $Hdisabled$R.");
        }
        else
        {
            OutputHelper::Outputf(Ashita::LogLevel::Error, "You must specify 'enable' or 'disable' to change LegacyCommands setting.");
        }
    }

    if (_stricmp(Args[2].c_str(), "shortoutput") == 0)
    {
        if (ArgCount < 4)
        {
            PrintHelpText(HelpText, false);
            return;
        }

        if (_stricmp(Args[3].c_str(), "enable") == 0)
        {
            mConfig.SetShortOutput(true);
            OutputHelper::Output(Ashita::LogLevel::Info, "ShortOutput $Henabled$R.");
        }
        else if (_stricmp(Args[3].c_str(), "disable") == 0)
        {
            mConfig.SetShortOutput(false);
            OutputHelper::Output(Ashita::LogLevel::Info, "ShortOutput $Hdisabled$R.");
        }
        else
        {
            OutputHelper::Outputf(Ashita::LogLevel::Error, "You must specify 'enable' or 'disable' to change ShortOutput setting.");
        }
    }

    if (_stricmp(Args[2].c_str(), "retrydelay") == 0)
    {
        if (ArgCount < 4)
        {
            PrintHelpText(HelpText, false);
            return;
        }

        mConfig.SetRetryDelay(static_cast<uint32_t>(atoi(Args[3].c_str())));
        OutputHelper::Outputf(Ashita::LogLevel::Info, "RetryDelay set to $H%d$R.", mConfig.GetRetryDelay());
    }
}
