#include "Bellhop.h"
#include "..\common\thirdparty\rapidxml.hpp"
using namespace rapidxml;

void Bellhop::LoadSettings()
{
    std::string Path = pSettings->GetCharacterSettingsPath(mState.MyName.c_str());

    if (Path == "FILE_NOT_FOUND")
    {
        mSettings = Settings_t();
        Path = pSettings->GetDefaultSettingsPath();
        SaveSettings(Path);
        LoadSettings(Path); //This won't change settings, but it'll set the path in settings helper for currently loaded settings.
    }
    else if (Path == pSettings->GetLoadedXmlPath())
        return;
    else
        LoadSettings(Path);
}
void Bellhop::LoadSettings(std::string Path)
{
    //Reset settings.
    mSettings                 = Settings_t();
    xml_document<>* XMLReader = pSettings->LoadSettingsXml(Path);
    if (XMLReader == NULL)
    {
        pOutput->error_f("Could not load settings file.  Resetting to defaults.  [$H%s$R]", Path.c_str());
        return;
    }

    //Make sure XML has a Bellhop node.
    xml_node<>* Node = XMLReader->first_node("bellhop");
    if (!Node)
    {
        pSettings->UnloadSettings();
        pOutput->error_f("Settings file did not have a bellhop node at root level.  Resetting to defaults.  [$H%s$R]", Path.c_str());
        return;
    }

    //Parse settings.
    for (Node = Node->first_node(); Node; Node = Node->next_sibling())
    {
        if (_stricmp(Node->name(), "autotrade") == 0)
        {
            if (_stricmp(Node->value(), "accept") == 0)
                mSettings.Default = AutoTradeSetting::Accept;
            else if (_stricmp(Node->value(), "deny") == 0)
                mSettings.Default = AutoTradeSetting::Deny;
        }

        if (_stricmp(Node->name(), "blacklist") == 0)
        {
            for (xml_node<>* SubNode = Node->first_node(); SubNode; SubNode = SubNode->next_sibling())
            {
                if (_stricmp(SubNode->name(), "entry") == 0)
                {
                    std::string cleanName = FormatName(SubNode->value());
                    if (cleanName.length() > 0)
                        AddToList(&(mSettings.BlackList), cleanName);
                }
            }
            continue;
        }

        if (_stricmp(Node->name(), "whitelist") == 0)
        {
            for (xml_node<>* SubNode = Node->first_node(); SubNode; SubNode = SubNode->next_sibling())
            {
                if (_stricmp(SubNode->name(), "entry") == 0)
                {
                    std::string cleanName = FormatName(SubNode->value());
                    if (cleanName.length() > 0)
                        AddToList(&(mSettings.WhiteList), cleanName);
                }
            }
            continue;
        }


        if (_stricmp(Node->name(), "forceenablebags") == 0)
        {
            stringstream stream(Node->value());
            while (stream.good())
            {
                string bag;
                getline(stream, bag, ',');
                if (bag.length() < 1)
                    break;
                int bagIndex = atoi(bag.c_str());
                if ((bagIndex < 1) || (bagIndex > 12))
                    continue;
                mSettings.ForceEnableBags.push_back(bagIndex);
            }
            continue;
        }

        if (_stricmp(Node->name(), "forcedisablebags") == 0)
        {
            stringstream stream(Node->value());
            while (stream.good())
            {
                string bag;
                getline(stream, bag, ',');
                if (bag.length() < 1)
                    break;
                int bagIndex = atoi(bag.c_str());
                if ((bagIndex < 0) || (bagIndex > 12))
                    continue;
                mSettings.ForceDisableBags.push_back(bagIndex);
            }
            continue;
        }


        if (_stricmp(Node->name(), "nomadstorage") == 0)
        {
            if (_stricmp(Node->value(), "enabled") == 0)
                mSettings.NomadStorage = true;
            else if (_stricmp(Node->value(), "disabled") == 0)
                mSettings.NomadStorage = false;
            continue;
        }

        if (_stricmp(Node->name(), "ignorecraftskill") == 0)
        {
            if (_stricmp(Node->value(), "enabled") == 0)
                mSettings.IgnoreCraftSkill = true;
            else if (_stricmp(Node->value(), "disabled") == 0)
                mSettings.IgnoreCraftSkill = false;
            continue;
        }

        if (_stricmp(Node->name(), "enforcetradewindow") == 0)
        {
            if (_stricmp(Node->value(), "enabled") == 0)
                mSettings.EnforceTradeWindow = true;
            else if (_stricmp(Node->value(), "disabled") == 0)
                mSettings.EnforceTradeWindow = false;
            continue;
        }

        if (_stricmp(Node->name(), "legacycommands") == 0)
        {
            if (_stricmp(Node->value(), "enabled") == 0)
                mSettings.LegacyCommands = true;
            else if (_stricmp(Node->value(), "disabled") == 0)
                mSettings.LegacyCommands = false;
            continue;
        }

        if (_stricmp(Node->name(), "shortoutput") == 0)
        {
            if (_stricmp(Node->value(), "enabled") == 0)
                mSettings.ShortOutput = true;
            else if (_stricmp(Node->value(), "disabled") == 0)
                mSettings.ShortOutput = false;
            continue;
        }

        if (_stricmp(Node->name(), "retrydelay") == 0)
        {
            int value = atoi(Node->value());
            value     = min(10000, max(0, value));
            mSettings.RetryDelay = (uint32_t)value;
            continue;
        }

        if (_stricmp(Node->name(), "itemtypes") == 0)
        {
            for (xml_node<>* SubNode = Node->first_node(); SubNode; SubNode = SubNode->next_sibling())
            {
                if (_stricmp(SubNode->name(), "type") == 0)
                {
                    xml_attribute<>* attr = SubNode->first_attribute("name");
                    if (attr)
                    {
                        std::string key(attr->value());
                        std::list<uint16_t> MatchIds;
                        for (xml_node<>* EntryNode = SubNode->first_node(); EntryNode; EntryNode = EntryNode->next_sibling())
                        {
                            if (_stricmp(EntryNode->name(), "entry") != 0)
                                continue;

                            xml_attribute<>* subAttr = EntryNode->first_attribute("id");
                            if (subAttr)
                            {
                                int32_t id = atoi(subAttr->value());
                                if ((id > 0) && (id < 0xFFFF))
                                    MatchIds.push_back((uint16_t)id);
                                continue;
                            }

                            subAttr = EntryNode->first_attribute("name");
                            if (subAttr)
                            {
                                for (int x = 1; x < 65535; x++)
                                {
                                    IItem* pResource = m_AshitaCore->GetResourceManager()->GetItemById(x);
                                    if ((pResource) && (pResource->Name[0]) && (_stricmp(subAttr->value(), pResource->Name[0]) == 0))
                                    {
                                        MatchIds.push_back(x);
                                    }
                                }
                            }

                            subAttr = EntryNode->first_attribute("wildcard");
                            if (subAttr)
                            {
                                for (int x = 1; x < 65535; x++)
                                {
                                    IItem* pResource = m_AshitaCore->GetResourceManager()->GetItemById(x);
                                    if ((pResource) && (pResource->Name[0]) && (CheckWildcardMatch(subAttr->value(), pResource->Name[0])))
                                    {
                                        MatchIds.push_back(x);
                                    }
                                }
                            }
                        }
                        mSettings.ItemTypes[key] = MatchIds;
                    }
                }
            }
            continue;
        }
    }
}
void Bellhop::SaveSettings()
{
    std::string Path = pSettings->GetLoadedXmlPath();

    ofstream outStream(Path.c_str());
    if (!outStream.is_open())
    {
        pOutput->error_f("Failed to write file.  [%s]", Path.c_str());
        return;
    }

    WriteSettings(&outStream);

    outStream.close();

}
void Bellhop::SaveSettings(std::string Path)
{
    pSettings->CreateDirectories(Path.c_str());

    ofstream outStream(Path.c_str());
    if (!outStream.is_open())
    {
        pOutput->error_f("Failed to write file.  [%s]", Path.c_str());
        return;
    }

    WriteSettings(&outStream);

    outStream.close();
    pOutput->message_f("Wrote settings XML. [$H%s$R]", Path.c_str());
}
void Bellhop::WriteSettings(ofstream * outStream)
{
    *outStream << "<bellhop>\n\n";

    *outStream << "\t<!--Set the default behavior of autotrade. (Accept/Deny/Ignore)-->\n";
    *outStream << "\t<autotrade>";
    if (mSettings.Default == AutoTradeSetting::Accept)
        *outStream << "Accept";
    else if (mSettings.Default == AutoTradeSetting::Deny)
        *outStream << "Deny";
    else
        *outStream << "Ignore";
    *outStream << "</autotrade>\n\n";

    *outStream << "\t<!-- Any entries contained here will automatically have trades denied. -->\n";
    *outStream << "\t<blacklist>\n";
    for (std::list<std::string>::iterator iter = mSettings.BlackList.begin(); iter != mSettings.BlackList.end(); iter++)
    {
        *outStream << "\t\t<entry>" << *iter << "</entry>\n";
    }
    *outStream << "\t</blacklist>\n\n";

    *outStream << "\t<!-- Any entries contained here will automatically have trades accepted. -->\n";
    *outStream << "\t<whitelist>\n";
    for (std::list<std::string>::iterator iter = mSettings.WhiteList.begin(); iter != mSettings.WhiteList.end(); iter++)
    {
        *outStream << "\t\t<entry>" << *iter << "</entry>\n";
    }
    *outStream << "\t</whitelist>\n\n";

    *outStream << "\t<!-- Any bag indices listed here will be treated as if you have access to them.  This is primarily for DSP. -->\n";
    *outStream << "\t<forceenablebags>";
    bool firstEntry = true;
    for (std::list<int>::iterator iter = mSettings.ForceEnableBags.begin(); iter != mSettings.ForceEnableBags.end(); iter++)
    {
        if (!firstEntry)
            *outStream << ",";
        *outStream << *iter;
        firstEntry = false;
    }
    *outStream << "</forceenablebags>\n\n";

    *outStream << "\t<!-- Any bag indices listed here will be treated as if you do not have access to them.  This is primarily for DSP. -->\n";
    *outStream << "\t<forcedisablebags>";
    firstEntry = true;
    for (std::list<int>::iterator iter = mSettings.ForceDisableBags.begin(); iter != mSettings.ForceDisableBags.end(); iter++)
    {
        if (!firstEntry)
            *outStream << ",";
        *outStream << *iter;
        firstEntry = false;
    }
    *outStream << "</forcedisablebags>\n\n";

    *outStream << "\t<!--If enabled, bellhop will allow you to remove and place items in storage while at a nomad moogle.  Retail servers allow this, but it cannot be accomplished with a legitimate client so it may entail ban risk.-->\n";
    *outStream << "\t<nomadstorage>" << (mSettings.NomadStorage ? "Enabled" : "Disabled") << "</nomadstorage>\n\n";

    *outStream << "\t<!--If enabled, bellhop will allow you to purchase items you do not have the rank for from guild NPCs.  Retail servers allow this, but it cannot be accomplished with a legitimate client so it may entail ban risk.-->\n";
    *outStream << "\t<ignorecraftskill>" << (mSettings.IgnoreCraftSkill ? "Enabled" : "Disabled") << "</ignorecraftskill>\n\n";

    *outStream << "\t<!--If enabled, bellhop will prevent tradeplayer commands unless a trade window is actually open.-->\n";
    *outStream << "\t<enforcetradewindow>" << (mSettings.EnforceTradeWindow ? "Enabled" : "Disabled") << "</enforcetradewindow>\n\n";

    *outStream << "\t<!--If enabled, commands used for Ashita3's version of bellhop will work.  This is disabled by default as some of them overlap with ingame commands or are generic enough to compete with other plugins. -->\n";
    *outStream << "\t<legacycommands>" << (mSettings.LegacyCommands ? "Enabled" : "Disabled") << "</legacycommands>\n\n";

    *outStream << "\t<!-- If enabled, complex commands will be summarized in one line of output instead of using multiple. -->\n";
    *outStream << "\t<shortoutput>" << (mSettings.ShortOutput ? "Enabled" : "Disabled") << "</shortoutput>\n\n";

    *outStream << "\t<!-- The time, in milliseconds, before an autotrade-related packet will be resent if the server does not respond to it. -->\n";
    *outStream << "\t<retrydelay>" << mSettings.RetryDelay << "</retrydelay>\n\n";

    *outStream << "\t<!--This is used to specify item types, which allows you to batch commands on a whole type of item.  See documentation for more information.-->\n";
    *outStream << "\t<itemtypes>\n";
    for (std::map<std::string, std::list<uint16_t>>::iterator iter = mSettings.ItemTypes.begin(); iter != mSettings.ItemTypes.end(); iter++)
    {
        *outStream << "\t\t<type name=\"" << iter->first << "\">\n";
        std::list<IItem*> resourceList;
        for (std::list<uint16_t>::iterator entryIter = iter->second.begin(); entryIter != iter->second.end(); entryIter++)
        {
            resourceList.push_back(m_AshitaCore->GetResourceManager()->GetItemById(*entryIter));
        }
        
        resourceList.sort([](const IItem* a, const IItem* b)
        {
            return _stricmp(a->Name[0], b->Name[0]);
        });

        for (std::list<IItem*>::iterator itemIter = resourceList.begin(); itemIter != resourceList.end(); itemIter++)
        {
            *outStream << "\t\t\t<entry id=\"" << (*itemIter)->Id << "\" /> <!-- " << (*itemIter)->Name[0] << " -->\n";
        }

        *outStream << "\t\t</type>\n";
    }
    *outStream << "\t</itemtypes>\n\n";

    *outStream << "</bellhop>";
}