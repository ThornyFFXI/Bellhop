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
        mSettings.Default = AutoTradeSetting::Accept;
        pOutput->message("AutoTrade default set to $Haccept$R.");
        SaveSettings();
    }
    else if (_stricmp(Args[2].c_str(), "deny") == 0)
    {
        mSettings.Default = AutoTradeSetting::Deny;
        pOutput->message("AutoTrade default set to $Hdeny$R.");
        SaveSettings();
    }
    else if (_stricmp(Args[2].c_str(), "ignore") == 0)
    {
        mSettings.Default = AutoTradeSetting::Ignore;
        pOutput->message("AutoTrade default set to $Hignore$R.");
        SaveSettings();
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

        std::string cleanName = FormatName(Args[3]);
        if (cleanName.length() == 0)
        {
            pOutput->error_f("Invalid name used: %s", Args[3].c_str());
            return;
        }

        if (AddToList(&(mSettings.BlackList), cleanName))
        {
            pOutput->message_f("$H%s$R added to trade blacklist.", cleanName.c_str());
            SaveSettings();
        }
        else
            pOutput->message_f("$H%s$R was already in your trade blacklist.", cleanName.c_str());
    }

    else if (_stricmp(Args[2].c_str(), "remove") == 0)
    {
        if (ArgCount < 4)
        {
            PrintHelpText(HelpText, false);
            return;
        }

        std::string cleanName = FormatName(Args[3]);
        if (cleanName.length() == 0)
        {
            pOutput->error_f("Invalid name used: %s", Args[3].c_str());
            return;
        }

        if (RemoveFromList(&(mSettings.BlackList), cleanName))
        {
            pOutput->message_f("$H%s$R removed from trade blacklist.", cleanName.c_str());
            SaveSettings();
        }
        else
            pOutput->message_f("$H%s$R was not in your trade blacklist.", cleanName.c_str());
    }

    else if (_stricmp(Args[2].c_str(), "clear") == 0)
    {
        mSettings.BlackList.clear();
        pOutput->message("Trade blacklist cleared.");
        SaveSettings();
    }

    else if (_stricmp(Args[2].c_str(), "print") == 0)
    {
        pOutput->message("$HTrade Blacklist");
        if (mSettings.BlackList.size() == 0)
            pOutput->message("[empty]");
        for (std::list<string>::iterator iter = mSettings.BlackList.begin(); iter != mSettings.BlackList.end(); iter++)
        {
            pOutput->message_f("%s", iter->c_str());
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

        std::string cleanName = FormatName(Args[3]);
        if (cleanName.length() == 0)
        {
            pOutput->error_f("Invalid name used: %s", Args[3].c_str());
            return;
        }

        if (AddToList(&(mSettings.WhiteList), cleanName))
        {
            pOutput->message_f("$H%s$R added to trade whitelist.", cleanName.c_str());
            SaveSettings();
        }
        else
            pOutput->message_f("$H%s$R was already in your trade whitelist.", cleanName.c_str());
    }

    else if (_stricmp(Args[2].c_str(), "remove") == 0)
    {
        if (ArgCount < 4)
        {
            PrintHelpText(HelpText, false);
            return;
        }

        std::string cleanName = FormatName(Args[3]);
        if (cleanName.length() == 0)
        {
            pOutput->error_f("Invalid name used: %s", Args[3].c_str());
            return;
        }

        if (RemoveFromList(&(mSettings.WhiteList), cleanName))
        {
            pOutput->message_f("$H%s$R removed from trade whitelist.", cleanName.c_str());
            SaveSettings();
        }
        else
            pOutput->message_f("$H%s$R was not in your trade whitelist.", cleanName.c_str());
    }

    else if (_stricmp(Args[2].c_str(), "clear") == 0)
    {
        mSettings.WhiteList.clear();
        pOutput->message("Trade whitelist cleared.");
        SaveSettings();
    }

    else if (_stricmp(Args[2].c_str(), "print") == 0)
    {
        pOutput->message("$HTrade Whitelist");
        if (mSettings.WhiteList.size() == 0)
            pOutput->message("[empty]");
        for (std::list<string>::iterator iter = mSettings.WhiteList.begin(); iter != mSettings.WhiteList.end(); iter++)
        {
            pOutput->message_f("%s", iter->c_str());
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
            pOutput->error_f("$H%s$R is not a valid bag.");
            return;
        }

        if (std::find(mSettings.ForceEnableBags.begin(), mSettings.ForceEnableBags.end(), index) != mSettings.ForceEnableBags.end())
        {
            pOutput->message_f("$H%s$R was already in your force enabled bags.", ContainerNames[index]);
        }
        else
        {
            mSettings.ForceEnableBags.push_back(index);
            pOutput->message_f("$H%s$R added to force enabled bags.", ContainerNames[index]);
            SaveSettings();
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
            pOutput->error_f("$H%s$R is not a valid bag.");
            return;
        }

        std::list<int>::iterator find = std::find(mSettings.ForceEnableBags.begin(), mSettings.ForceEnableBags.end(), index);
        if (find == mSettings.ForceEnableBags.end())
        {
            pOutput->message_f("$H%s$R was not in your force enabled bags.", ContainerNames[index]);
        }
        else
        {
            mSettings.ForceEnableBags.erase(find);
            pOutput->message_f("$H%s$R removed from force enabled bags.", ContainerNames[index]);
            SaveSettings();
        }
    }

    else if (_stricmp(Args[2].c_str(), "clear") == 0)
    {
        if (mSettings.ForceEnableBags.size() == 0)
        {
            pOutput->message("You have no force enabled bags.");
        }
        else
        {
            mSettings.ForceEnableBags.clear();
            pOutput->message("Force enabled bags cleared.");
            SaveSettings();
        }
    }

    else if (_stricmp(Args[2].c_str(), "print") == 0)
    {
        pOutput->message("$HForce Enabled Bags");
        if (mSettings.WhiteList.size() == 0)
            pOutput->message("[empty]");
        for (std::list<int>::iterator iter = mSettings.ForceEnableBags.begin(); iter != mSettings.ForceEnableBags.end(); iter++)
        {
            pOutput->message_f("%s", ContainerNames[*iter]);
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
            pOutput->error_f("$H%s$R is not a valid bag.");
            return;
        }

        if (std::find(mSettings.ForceDisableBags.begin(), mSettings.ForceDisableBags.end(), index) != mSettings.ForceDisableBags.end())
        {
            pOutput->message_f("$H%s$R was already in your force disabled bags.", ContainerNames[index]);
        }
        else
        {
            mSettings.ForceDisableBags.push_back(index);
            pOutput->message_f("$H%s$R added to force disabled bags.", ContainerNames[index]);
            SaveSettings();
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
            pOutput->error_f("$H%s$R is not a valid bag.");
            return;
        }

        std::list<int>::iterator find = std::find(mSettings.ForceDisableBags.begin(), mSettings.ForceDisableBags.end(), index);
        if (find == mSettings.ForceDisableBags.end())
        {
            pOutput->message_f("$H%s$R was not in your force disabled bags.", ContainerNames[index]);
        }
        else
        {
            mSettings.ForceDisableBags.erase(find);
            pOutput->message_f("$H%s$R removed from force disabled bags.", ContainerNames[index]);
            SaveSettings();
        }
    }

    else if (_stricmp(Args[2].c_str(), "clear") == 0)
    {
        if (mSettings.ForceDisableBags.size() == 0)
        {
            pOutput->message("You have no force disabled bags.");
        }
        else
        {
            mSettings.ForceDisableBags.clear();
            pOutput->message("Force disabled bags cleared.");
            SaveSettings();
        }
    }

    else if (_stricmp(Args[2].c_str(), "print") == 0)
    {
        pOutput->message("$HForce Disabled Bags");
        if (mSettings.WhiteList.size() == 0)
            pOutput->message("[empty]");
        for (std::list<int>::iterator iter = mSettings.ForceDisableBags.begin(); iter != mSettings.ForceDisableBags.end(); iter++)
        {
            pOutput->message_f("%s", ContainerNames[*iter]);
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
            mSettings.NomadStorage = true;
            pOutput->message("NomadStorage $Henabled$R.");
            SaveSettings();
        }
        else if (_stricmp(Args[3].c_str(), "disable") == 0)
        {
            mSettings.NomadStorage = false;
            pOutput->message("NomadStorage $Hdisabled$R.");
            SaveSettings();
        }
        else
        {
            pOutput->error_f("You must specify 'enable' or 'disable' to change NomadStorage setting.");
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
            mSettings.IgnoreCraftSkill = true;
            pOutput->message("IgnoreCraftSkill $Henabled$R.");
            SaveSettings();
        }
        else if (_stricmp(Args[3].c_str(), "disable") == 0)
        {
            mSettings.IgnoreCraftSkill = false;
            pOutput->message("IgnoreCraftSkill $Hdisabled$R.");
            SaveSettings();
        }
        else
        {
            pOutput->error_f("You must specify 'enable' or 'disable' to change IgnoreCraftSkill setting.");
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
            mSettings.LegacyCommands = true;
            pOutput->message("LegacyCommands $Henabled$R.");
            SaveSettings();
        }
        else if (_stricmp(Args[3].c_str(), "disable") == 0)
        {
            mSettings.LegacyCommands = false;
            pOutput->message("LegacyCommands $Hdisabled$R.");
            SaveSettings();
        }
        else
        {
            pOutput->error_f("You must specify 'enable' or 'disable' to change LegacyCommands setting.");
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
            mSettings.ShortOutput = true;
            pOutput->message("ShortOutput $Henabled$R.");
            SaveSettings();
        }
        else if (_stricmp(Args[3].c_str(), "disable") == 0)
        {
            mSettings.ShortOutput = false;
            pOutput->message("ShortOutput $Hdisabled$R.");
            SaveSettings();
        }
        else
        {
            pOutput->error_f("You must specify 'enable' or 'disable' to change ShortOutput setting.");
        }
    }

    if (_stricmp(Args[2].c_str(), "retrydelay") == 0)
    {
        if (ArgCount < 4)
        {
            PrintHelpText(HelpText, false);
            return;
        }

        uint16_t Value = StringToUint16OrZero(Args[3]);
        if ((Value == 0) || (Value > 9999))
        {
            pOutput->error("You must put a valid number between 1 and 9999 for retrydelay.");
            return;
        }

        mSettings.RetryDelay = Value;
        pOutput->message_f("RetryDelay set to $H%d$R.", mSettings.RetryDelay);
        SaveSettings();
    }
}