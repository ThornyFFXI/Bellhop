#include "BellhopConfig.h"
ConfigLoader* ConfigLoader::pLoader = nullptr;

BellhopConfig::BellhopConfig()
    : ConfigBase("Bellhop")
    , mSettings(BellhopSettings_t())
{}

bool BellhopConfig::GetNomadStorage()
{
    return mSettings.NomadStorage;
}
bool BellhopConfig::GetEnforceTradeWindow()
{
    return mSettings.EnforceTradeWindow;
}
bool BellhopConfig::GetIgnoreCraftSkill()
{
    return mSettings.IgnoreCraftSkill;
}
bool BellhopConfig::GetLegacyCommands()
{
    return mSettings.LegacyCommands;
}
bool BellhopConfig::GetShortOutput()
{
    return mSettings.ShortOutput;
}
uint32_t BellhopConfig::GetRetryDelay()
{
    return mSettings.RetryDelay;
}
std::vector<int> BellhopConfig::GetDisabledBags()
{
    return mSettings.ForceDisableBags;
}
std::vector<int> BellhopConfig::GetEnabledBags()
{
    return mSettings.ForceEnableBags;
}
AutoTradeSetting BellhopConfig::GetAutoTradeSetting()
{
    return mSettings.AutoTradeSetting;
}
std::vector<std::string> BellhopConfig::GetWhitelist()
{
    return mSettings.Whitelist;
}
std::vector<std::string> BellhopConfig::GetBlacklist()
{
    return mSettings.Blacklist;
}
ItemTypeMap BellhopConfig::GetItemTypes()
{
    return mSettings.ItemTypes;
}
ItemTypeEntryMap BellhopConfig::GetItemTypeEntries()
{
    return mSettings.ItemTypeEntries;
}

void BellhopConfig::SetNomadStorage(bool enabled)
{
    if (mSettings.NomadStorage != enabled)
    {
        mSettings.NomadStorage = enabled;
        ConfigLoader::pLoader->SaveSettings();
    }
}
void BellhopConfig::SetEnforceTradeWindow(bool enabled)
{
    if (mSettings.EnforceTradeWindow != enabled)
    {
        mSettings.EnforceTradeWindow = enabled;
        ConfigLoader::pLoader->SaveSettings();
    }
}
void BellhopConfig::SetIgnoreCraftSkill(bool enabled)
{
    if (mSettings.IgnoreCraftSkill != enabled)
    {
        mSettings.IgnoreCraftSkill = enabled;
        ConfigLoader::pLoader->SaveSettings();
    }
}
void BellhopConfig::SetLegacyCommands(bool enabled)
{
    if (mSettings.LegacyCommands != enabled)
    {
        mSettings.LegacyCommands = enabled;
        ConfigLoader::pLoader->SaveSettings();
    }
}
void BellhopConfig::SetShortOutput(bool enabled)
{
    if (mSettings.ShortOutput != enabled)
    {
        mSettings.ShortOutput = enabled;
        ConfigLoader::pLoader->SaveSettings();
    }
}
void BellhopConfig::SetRetryDelay(uint32_t delay)
{
    delay = min(10000, delay);
    if (mSettings.RetryDelay != delay)
    {
        mSettings.RetryDelay = delay;
        ConfigLoader::pLoader->SaveSettings();
    }
}
void BellhopConfig::SetDisabledBags(std::vector<int> disabledBags)
{
    if (mSettings.ForceDisableBags != disabledBags)
    {
        mSettings.ForceDisableBags = disabledBags;
        ConfigLoader::pLoader->SaveSettings();
    }
}
void BellhopConfig::SetEnabledBags(std::vector<int> enabledBags)
{
    if (mSettings.ForceEnableBags != enabledBags)
    {
        mSettings.ForceEnableBags = enabledBags;
        ConfigLoader::pLoader->SaveSettings();
    }
}
void BellhopConfig::SetAutoTradeSetting(AutoTradeSetting setting)
{
    if (mSettings.AutoTradeSetting != setting)
    {
        mSettings.AutoTradeSetting = setting;
        ConfigLoader::pLoader->SaveSettings();
    }
}
void BellhopConfig::SetWhitelist(std::vector<std::string> whitelist)
{
    if (mSettings.Whitelist != whitelist)
    {
        mSettings.Whitelist = whitelist;
        ConfigLoader::pLoader->SaveSettings();
    }
}
void BellhopConfig::SetBlacklist(std::vector<std::string> blacklist)
{
    if (mSettings.Blacklist != blacklist)
    {
        mSettings.Blacklist = blacklist;
        ConfigLoader::pLoader->SaveSettings();
    }
}
void BellhopConfig::SetItemTypeEntries(ItemTypeEntryMap itemTypeEntries)
{
    mSettings.ItemTypeEntries = itemTypeEntries;
    ProcessItemTypes();
    ConfigLoader::pLoader->SaveSettings();
}
void BellhopConfig::ProcessItemTypes()
{
    ItemTypeMap itemTypes;
    for (ItemTypeEntryMap::iterator iter = mSettings.ItemTypeEntries.begin(); iter != mSettings.ItemTypeEntries.end(); iter++)
    {
        std::vector<uint16_t> items;
        for (std::vector<ItemTypeEntry>::iterator iType = iter->second.begin(); iType != iter->second.end(); iType++)
        {
            if (iType->Id != -1)
            {
                IItem* pResource = OutputHelper::pOutputHelper->pAshitaCore->GetResourceManager()->GetItemById(iType->Id);
                if ((pResource) && (pResource->Name[0]))
                {
                    if ((strlen(iType->Name) == 0) || (_stricmp(iType->Name, pResource->Name[0]) == 0))
                    {
                        if ((strlen(iType->Wildcard) == 0) || (CheckWildcardMatch(iType->Wildcard, pResource->Name[0])))
                        {
                            items.push_back(static_cast<uint16_t>(iType->Id));
                        }
                    }
                }
            }
            else
            {
                for (int x = 1; x < 65535; x++)
                {
                    IItem* pResource = OutputHelper::pOutputHelper->pAshitaCore->GetResourceManager()->GetItemById(x);
                    if ((pResource) && (pResource->Name[0]))
                    {
                        if ((strlen(iType->Name) == 0) || (_stricmp(iType->Name, pResource->Name[0]) == 0))
                        {
                            if ((strlen(iType->Wildcard) == 0) || (CheckWildcardMatch(iType->Wildcard, pResource->Name[0])))
                            {
                                items.push_back(static_cast<uint16_t>(x));
                            }
                        }
                    }
                }
            }
        }
        itemTypes[iter->first] = items;
    }
    mSettings.ItemTypes = itemTypes;
}

bool BellhopConfig::CheckWildcardMatch(const char* wc, const char* compare)
{
    while (*wc)
    {
        if (wc[0] == '*')
        {
            if (wc[1] == 0)
                return true;
            while (*compare)
            {
                if (CheckWildcardMatch((wc + 1), compare))
                {
                    return true;
                }
                compare++;
            }
            return false;
        }
        if ((wc[0] | 32) != (compare[0] | 32))
            return false;
        wc++;
        compare++;
    }
    return (*compare == 0);
}
std::string BellhopConfig::FormatName(std::string input)
{
    std::string buffer = std::string();
    for (int x = 0; x < input.length(); x++)
    {
        if (!isalpha(input[x]))
            return std::string();
        else if (buffer.length() == 0)
            buffer += toupper(input[x]);
        else
            buffer += tolower(input[x]);
    }
    return buffer;
}

ConfigSaveStyle BellhopConfig::GetDefaultSaveStyle()
{
    return ConfigSaveStyle::CharacterSpecific;
}
bool BellhopConfig::LoadConfig(xml_node<>* pRoot, const char* errorBuffer)
{
    for (xml_node<>* pNode = pRoot->first_node(); pNode; pNode = pNode->next_sibling())
    {
        if (_stricmp(pNode->name(), "autotrade") == 0)
        {
            if (_stricmp(pNode->value(), "accept") == 0)
                SetAutoTradeSetting(AutoTradeSetting::Accept);
            else if (_stricmp(pNode->value(), "deny") == 0)
                SetAutoTradeSetting(AutoTradeSetting::Deny);
        }

        if (_stricmp(pNode->name(), "blacklist") == 0)
        {
            std::vector<std::string> blacklist = GetBlacklist();
            for (xml_node<>* pSubNode = pNode->first_node(); pSubNode; pSubNode = pSubNode->next_sibling())
            {
                if (_stricmp(pSubNode->name(), "entry") == 0)
                {
                    std::string cleanName = FormatName(pSubNode->value());
                    if ((cleanName.length() > 0) && (std::find(blacklist.begin(), blacklist.end(), cleanName) == blacklist.end()))
                        blacklist.push_back(cleanName);
                }
            }
            SetBlacklist(blacklist);
            continue;
        }

        if (_stricmp(pNode->name(), "whitelist") == 0)
        {
            std::vector<std::string> whitelist = GetWhitelist();
            for (xml_node<>* pSubNode = pNode->first_node(); pSubNode; pSubNode = pSubNode->next_sibling())
            {
                if (_stricmp(pSubNode->name(), "entry") == 0)
                {
                    std::string cleanName = FormatName(pSubNode->value());
                    if ((cleanName.length() > 0) && (std::find(whitelist.begin(), whitelist.end(), cleanName) == whitelist.end()))
                        whitelist.push_back(cleanName);
                }
            }
            SetWhitelist(whitelist);
            continue;
        }

        if (_stricmp(pNode->name(), "forceenablebags") == 0)
        {
            std::vector<int> enabledbags = GetEnabledBags();
            stringstream stream(pNode->value());
            while (stream.good())
            {
                string bag;
                getline(stream, bag, ',');
                if (bag.length() < 1)
                    break;
                int bagIndex = atoi(bag.c_str());
                if ((bagIndex < 1) || (bagIndex > 12))
                    continue;
                if (std::find(enabledbags.begin(), enabledbags.end(), bagIndex) == enabledbags.end())
                    enabledbags.push_back(bagIndex);
            }
            SetEnabledBags(enabledbags);
            continue;
        }

        if (_stricmp(pNode->name(), "forcedisablebags") == 0)
        {
            std::vector<int> disabledbags = GetDisabledBags();
            stringstream stream(pNode->value());
            while (stream.good())
            {
                string bag;
                getline(stream, bag, ',');
                if (bag.length() < 1)
                    break;
                int bagIndex = atoi(bag.c_str());
                if ((bagIndex < 0) || (bagIndex > 12))
                    continue;
                if (std::find(disabledbags.begin(), disabledbags.end(), bagIndex) == disabledbags.end())
                    disabledbags.push_back(bagIndex);
            }
            SetDisabledBags(disabledbags);
            continue;
        }

        if (_stricmp(pNode->name(), "nomadstorage") == 0)
        {
            if (_stricmp(pNode->value(), "enabled") == 0)
                SetNomadStorage(true);
            else if (_stricmp(pNode->value(), "disabled") == 0)
                SetNomadStorage(false);
            continue;
        }

        if (_stricmp(pNode->name(), "ignorecraftskill") == 0)
        {
            if (_stricmp(pNode->value(), "enabled") == 0)
                SetIgnoreCraftSkill(true);
            else if (_stricmp(pNode->value(), "disabled") == 0)
                SetIgnoreCraftSkill(false);
            continue;
        }

        if (_stricmp(pNode->name(), "enforcetradewindow") == 0)
        {
            if (_stricmp(pNode->value(), "enabled") == 0)
                SetEnforceTradeWindow(true);
            else if (_stricmp(pNode->value(), "disabled") == 0)
                SetEnforceTradeWindow(false);
            continue;
        }

        if (_stricmp(pNode->name(), "legacycommands") == 0)
        {
            if (_stricmp(pNode->value(), "enabled") == 0)
                SetLegacyCommands(true);
            else if (_stricmp(pNode->value(), "disabled") == 0)
                SetLegacyCommands(false);
            continue;
        }

        if (_stricmp(pNode->name(), "shortoutput") == 0)
        {
            if (_stricmp(pNode->value(), "enabled") == 0)
                SetShortOutput(true);
            else if (_stricmp(pNode->value(), "disabled") == 0)
                SetShortOutput(false);
            continue;
        }

        if (_stricmp(pNode->name(), "retrydelay") == 0)
        {
            SetRetryDelay(static_cast<uint32_t>(atoi(pNode->value())));
            continue;
        }

        if (_stricmp(pNode->name(), "itemtypes") == 0)
        {
            ItemTypeEntryMap itemTypes;
            for (xml_node<>* pSubNode = pNode->first_node(); pSubNode; pSubNode = pSubNode->next_sibling())
            {
                if (_stricmp(pSubNode->name(), "type") == 0)
                {
                    xml_attribute<>* attr = pSubNode->first_attribute("name");
                    if (attr)
                    {
                        std::string key(attr->value());
                        std::vector<ItemTypeEntry> itemEntries;
                        std::vector<uint16_t> MatchIds;
                        for (xml_node<>* EntryNode = pSubNode->first_node(); EntryNode; EntryNode = EntryNode->next_sibling())
                        {
                            if (_stricmp(EntryNode->name(), "entry") != 0)
                                continue;

                            ItemTypeEntry entry = {0};
                            entry.Id            = -1;

                            xml_attribute<>* subAttr = EntryNode->first_attribute("id");
                            if (subAttr)
                            {
                                int32_t id = atoi(subAttr->value());
                                if ((id > 0) && (id < 0xFFFF))
                                    entry.Id = (uint16_t)id;
                            }

                            subAttr = EntryNode->first_attribute("name");
                            if (subAttr)
                            {
                                strcpy_s(entry.Name, 256, subAttr->value());
                            }

                            subAttr = EntryNode->first_attribute("wildcard");
                            if (subAttr)
                            {
                                strcpy_s(entry.Wildcard, 256, subAttr->value());
                            }

                            if ((entry.Id != -1) || (strlen(entry.Name) > 0) || (strlen(entry.Wildcard) > 0))
                            {
                                itemEntries.push_back(entry);
                            }
                        }
                        itemTypes[key] = itemEntries;
                    }
                }
            }
            SetItemTypeEntries(itemTypes);
            continue;
        }
    }
    return true;
}
void BellhopConfig::Reset()
{
    mSettings = BellhopSettings_t();
}
bool BellhopConfig::SaveConfig(ofstream* pStream, const char* errorBuffer)
{
    *pStream << "\t<!--Set the default behavior of autotrade. (Accept/Deny/Ignore)-->\n";
    *pStream << "\t<autotrade>";
    if (GetAutoTradeSetting() == AutoTradeSetting::Accept)
        *pStream << "Accept";
    else if (GetAutoTradeSetting() == AutoTradeSetting::Deny)
        *pStream << "Deny";
    else
        *pStream << "Ignore";
    *pStream << "</autotrade>\n\n";

    *pStream << "\t<!-- Any entries contained here will automatically have trades denied. -->\n";
    *pStream << "\t<blacklist>\n";
    std::vector<std::string> blacklist = GetBlacklist();
    for (std::vector<std::string>::iterator iter = blacklist.begin(); iter != blacklist.end(); iter++)
    {
        *pStream << "\t\t<entry>" << *iter << "</entry>\n";
    }
    *pStream << "\t</blacklist>\n\n";

    *pStream << "\t<!-- Any entries contained here will automatically have trades accepted. -->\n";
    *pStream << "\t<whitelist>\n";
    std::vector<std::string> whitelist = GetWhitelist();
    for (std::vector<std::string>::iterator iter = whitelist.begin(); iter != whitelist.end(); iter++)
    {
        *pStream << "\t\t<entry>" << *iter << "</entry>\n";
    }
    *pStream << "\t</whitelist>\n\n";

    *pStream << "\t<!-- Any bag indices listed here will be treated as if you have access to them.  This is primarily for DSP. -->\n";
    *pStream << "\t<forceenablebags>";
    std::vector<int> enabledbags = GetEnabledBags();
    bool firstEntry = true;
    for (std::vector<int>::iterator iter = enabledbags.begin(); iter != enabledbags.end(); iter++)
    {
        if (!firstEntry)
            *pStream << ",";
        *pStream << *iter;
        firstEntry = false;
    }
    *pStream << "</forceenablebags>\n\n";
    *pStream << "\t<!-- Any bag indices listed here will be treated as if you do not have access to them.  This is primarily for DSP. -->\n";
    *pStream << "\t<forcedisablebags>";

    std::vector<int> disabledbags = GetDisabledBags();
    firstEntry = true;
    for (std::vector<int>::iterator iter = disabledbags.begin(); iter != disabledbags.end(); iter++)
    {
        if (!firstEntry)
            *pStream << ",";
        *pStream << *iter;
        firstEntry = false;
    }
    *pStream << "</forcedisablebags>\n\n";

    *pStream << "\t<!--If enabled, bellhop will allow you to remove and place items in storage while at a nomad moogle.  Retail servers allow this, but it cannot be accomplished with a legitimate client so it may entail ban risk.-->\n";
    *pStream << "\t<nomadstorage>" << (GetNomadStorage() ? "Enabled" : "Disabled") << "</nomadstorage>\n\n";

    *pStream << "\t<!--If enabled, bellhop will allow you to purchase items you do not have the rank for from guild NPCs.  Retail servers allow this, but it cannot be accomplished with a legitimate client so it may entail ban risk.-->\n";
    *pStream << "\t<ignorecraftskill>" << (GetIgnoreCraftSkill() ? "Enabled" : "Disabled") << "</ignorecraftskill>\n\n";

    *pStream << "\t<!--If enabled, bellhop will prevent tradeplayer commands unless a trade window is actually open.-->\n";
    *pStream << "\t<enforcetradewindow>" << (GetEnforceTradeWindow() ? "Enabled" : "Disabled") << "</enforcetradewindow>\n\n";

    *pStream << "\t<!--If enabled, commands used for Ashita3's version of bellhop will work.  This is disabled by default as some of them overlap with ingame commands or are generic enough to compete with other plugins. -->\n";
    *pStream << "\t<legacycommands>" << (GetLegacyCommands() ? "Enabled" : "Disabled") << "</legacycommands>\n\n";

    *pStream << "\t<!-- If enabled, complex commands will be summarized in one line of output instead of using multiple. -->\n";
    *pStream << "\t<shortoutput>" << (GetShortOutput() ? "Enabled" : "Disabled") << "</shortoutput>\n\n";

    *pStream << "\t<!-- The time, in milliseconds, before an autotrade-related packet will be resent if the server does not respond to it. -->\n";
    *pStream << "\t<retrydelay>" << GetRetryDelay() << "</retrydelay>\n\n";

    *pStream << "\t<!--This is used to specify item types, which allows you to batch commands on a whole type of item.  See documentation for more information.-->\n";
    *pStream << "\t<itemtypes>\n";


    auto entries = GetItemTypeEntries();

    for (auto iter = entries.begin(); iter != entries.end(); iter++)
    {
        *pStream << "\t\t<type name=\"" << iter->first << "\">\n";

        for (auto iType = iter->second.begin(); iType != iter->second.end(); iType++)
        {
            *pStream << "\t\t\t<entry ";
            if (iType->Id != -1)
                *pStream << "id =\"" << iType->Id << "\" ";
            if (strlen(iType->Name) > 0)
                *pStream << "name=\"" << iType->Name << "\" ";
            if (strlen(iType->Wildcard) > 0)
                *pStream << "wildcard=\"" << iType->Wildcard << "\" ";
            *pStream << "/>\n";
        }
        *pStream << "\t\t</type>\n";
    }
    *pStream << "\t</itemtypes>\n\n";

    return true;
}