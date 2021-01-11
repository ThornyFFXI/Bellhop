#include "Bellhop.h"

bool Bellhop::HandleCommand(int32_t mode, const char* command, bool injected)
{
    UNREFERENCED_PARAMETER(mode);
    UNREFERENCED_PARAMETER(injected);

    std::vector<string> args;
    int argcount = 0;
    if (mSettings.LegacyCommands)
        argcount = ParseLegacyCommands(command, &args);
    else
        argcount = Ashita::Commands::GetCommandArgs(command, &args);

    if (argcount == 0)
        return false;

    if ((_stricmp(args[0].c_str(), "/bellhop") == 0) || (_stricmp(args[0].c_str(), "/bh") == 0))
    {
        auto iter = mCommandMap.find(args[1]);
        if (iter != mCommandMap.end())
        {
            (this->*(iter->second.handler))(args, argcount, iter->second.help);
        }
        return true;
    }
    return false;
}

uint32_t Bellhop::ParseLegacyCommands(const char* command, std::vector<std::string>* args)
{
    /*
    Rather than make the handlers ugly going forward, I opted to convert old commands to the new system here by simply inserting them as correct arguments for the new commands.
    This allows me to maintain the clean handlers for new version, while still maintaining support for old commands if user chooses to enable it.
    */

    std::vector<string> tempArgs;
    int argCount = Ashita::Commands::GetCommandArgs(command, &tempArgs);

    if (argCount == 0)
        return 0;

    if ((_stricmp(tempArgs[0].c_str(), "/drop") == 0) && (argCount > 1))
    {
        args->push_back("/bh");
        args->push_back("drop");
        args->push_back(tempArgs[1]);
        return 3;
    }

    if ((_stricmp(tempArgs[0].c_str(), "/dropall") == 0) && (argCount > 1))
    {
        args->push_back("/bh");
        args->push_back("dropall");
        args->push_back(tempArgs[1]);
        return 3;
    }

    if ((_stricmp(tempArgs[0].c_str(), "/gets") == 0) && (argCount > 1))
    {
        args->push_back("/bh");
        args->push_back("gets");
        args->push_back(tempArgs[1]);
        if (argCount > 2)
        {
            args->push_back(tempArgs[2]);
            return 4;
        }
        return 3;
    }

    if ((_stricmp(tempArgs[0].c_str(), "/getall") == 0) && (argCount > 1))
    {
        args->push_back("/bh");
        args->push_back("getall");
        args->push_back(tempArgs[1]);
        if (argCount > 2)
        {
            args->push_back(tempArgs[2]);
            return 4;
        }
        return 3;
    }

    if ((_strnicmp(tempArgs[0].c_str(), "/get", 4) == 0) && (argCount > 1))
    {
        if (tempArgs[0].length() == 4)
        {
            args->push_back("/bh");
            args->push_back("get");
            args->push_back(tempArgs[1]);
            if (argCount > 2)
            {
                args->push_back("1");
                args->push_back(tempArgs[2]);
                return 5;
            }
            return 3;
        }

        uint16_t count = StringToUint16OrZero(tempArgs[0].substr(4));
        if (count != 0)
        {
            args->push_back("/bh");
            args->push_back("get");
            args->push_back(tempArgs[1]);
            args->push_back(std::to_string(count));
            if (argCount > 2)
            {
                args->push_back(tempArgs[2]);
                return 5;
            }
            return 4;
        }
    }

    if ((_stricmp(tempArgs[0].c_str(), "/puts") == 0) && (argCount > 1))
    {
        args->push_back("/bh");
        args->push_back("puts");
        args->push_back(tempArgs[1]);
        if (argCount > 2)
        {
            args->push_back(tempArgs[2]);
            return 4;
        }
        return 3;
    }

    if ((_stricmp(tempArgs[0].c_str(), "/putall") == 0) && (argCount > 1))
    {
        args->push_back("/bh");
        args->push_back("putall");
        args->push_back(tempArgs[1]);
        if (argCount > 2)
        {
            args->push_back(tempArgs[2]);
            return 4;
        }
        return 3;
    }

    if ((_strnicmp(tempArgs[0].c_str(), "/put", 4) == 0) && (argCount > 1))
    {
        if (tempArgs[0].length() == 4)
        {
            args->push_back("/bh");
            args->push_back("put");
            args->push_back(tempArgs[1]);
            if (argCount > 2)
            {
                args->push_back("1");
                args->push_back(tempArgs[2]);
                return 5;
            }
            return 3;
        }

        uint16_t count = StringToUint16OrZero(tempArgs[0].substr(4));
        if (count != 0)
        {
            args->push_back("/bh");
            args->push_back("put");
            args->push_back(tempArgs[1]);
            args->push_back(std::to_string(count));
            if (argCount > 2)
            {
                args->push_back(tempArgs[2]);
                return 5;
            }
            return 4;
        }
    }

    if ((_stricmp(tempArgs[0].c_str(), "/sell") == 0) && (argCount > 1))
    {
        args->push_back("/bh");
        args->push_back("sell");
        args->push_back(tempArgs[1]);
        return 3;
    }

    if ((_stricmp(tempArgs[0].c_str(), "/sellall") == 0) && (argCount > 1))
    {
        args->push_back("/bh");
        args->push_back("sellall");
        args->push_back(tempArgs[1]);
        return 3;
    }

    if ((_stricmp(tempArgs[0].c_str(), "/buy") == 0) && (argCount > 1))
    {
        args->push_back("/bh");
        args->push_back("buy");
        args->push_back(tempArgs[1]);
        if (argCount > 2)
        {
            args->push_back(tempArgs[2]);
            return 4;
        }
        return 3;
    }

    if ((_stricmp(tempArgs[0].c_str(), "/baz") == 0) && (argCount > 2))
    {
        args->push_back("/bh");
        args->push_back("bazaar");
        args->push_back(tempArgs[1]);
        args->push_back(tempArgs[2]);
        return 4;
    }

    if ((_stricmp(tempArgs[0].c_str(), "/bazall") == 0) && (argCount > 2))
    {
        args->push_back("/bh");
        args->push_back("bazaarall");
        args->push_back(tempArgs[1]);
        args->push_back(tempArgs[2]);
        return 4;
    }

    if ((_stricmp(tempArgs[0].c_str(), "/sort") == 0) && (argCount > 1))
    {
        args->push_back("/bh");
        args->push_back("sort");
        args->push_back(tempArgs[1]);
        return 3;
    }

    if ((_stricmp(tempArgs[0].c_str(), "/tadd") == 0) && (argCount > 1))
    {
        args->push_back("/bh");
        args->push_back("tradeplayerall");
        args->push_back(tempArgs[1]);
        return 3;
    }

    if (_stricmp(tempArgs[0].c_str(), "/autotrade") == 0)
    {
        if ((argCount > 1) && (_stricmp(tempArgs[1].c_str(), "on") == 0))
        {
            args->push_back("/bh");
            args->push_back("default");
            args->push_back("allow");
            return 3;
        }
        else if ((argCount > 1) && (_stricmp(tempArgs[1].c_str(), "off") == 0))
        {
            args->push_back("/bh");
            args->push_back("default");
            args->push_back("deny");
            return 3;
        }
        else
        {
            args->push_back("/bh");
            args->push_back("default");
            if (mSettings.Default == AutoTradeSetting::Accept)
                args->push_back("deny");
            else if (mSettings.Default == AutoTradeSetting::Deny)
                args->push_back("ignore");
            else
                args->push_back("allow");
            return 3;
        }
    }

    if ((_stricmp(tempArgs[0].c_str(), "/trade") == 0) && (argCount > 3) && (((argCount - 1) % 2) == 0))
    {
        args->push_back("/bh");
        args->push_back("tradenpc");
        int count = 2;
        for (int x = 1; x < argCount; x++)
        {
            args->push_back(tempArgs[x]);
            count++;
        }
        return count;
    }

    if ((_stricmp(tempArgs[0].c_str(), "/tradeall") == 0) && (argCount > 1))
    {
        args->push_back("/bh");
        args->push_back("tradenpcall");
        args->push_back(tempArgs[1]);
        return 3;
    }

    //If no match, return normal command parse.
    return Ashita::Commands::GetCommandArgs(command, args);
}