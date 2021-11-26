#ifndef __ASHITA_FindAllConfig_H_INCLUDED__
#define __ASHITA_FindAllConfig_H_INCLUDED__

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

#include "ConfigLoader.h"
#include <map>

enum class AutoTradeSetting
{
    Accept,
    Deny,
    Ignore
};

struct ItemTypeEntry
{
    int32_t Id;
    char Name[256];
    char Wildcard[256];
};

typedef std::map<std::string, std::vector<ItemTypeEntry>> ItemTypeEntryMap;
typedef std::map<std::string, std::vector<uint16_t>> ItemTypeMap;

struct BellhopSettings_t
{
    bool NomadStorage;
    bool EnforceTradeWindow;
    bool IgnoreCraftSkill;
    bool LegacyCommands;
    bool ShortOutput;
    uint32_t RetryDelay;
    std::vector<int> ForceDisableBags;
    std::vector<int> ForceEnableBags;

    AutoTradeSetting AutoTradeSetting;
    std::vector<string> Whitelist;
    std::vector<string> Blacklist;
    ItemTypeEntryMap ItemTypeEntries;
    ItemTypeMap ItemTypes;

    BellhopSettings_t()
        : AutoTradeSetting(AutoTradeSetting::Ignore)
        , NomadStorage(false)
        , IgnoreCraftSkill(false)
        , EnforceTradeWindow(false)
        , LegacyCommands(true)
        , ShortOutput(false)
        , RetryDelay(3000)
        , ForceEnableBags(std::vector<int>())
        , ForceDisableBags(std::vector<int>())
        , Blacklist(std::vector<string>())
        , Whitelist(std::vector<string>())
        , ItemTypeEntries(ItemTypeEntryMap())
        , ItemTypes(std::map<string, std::vector<uint16_t>>())
    {}
};

class BellhopConfig : public ConfigBase
{
private:
    BellhopSettings_t mSettings;
public:
    BellhopConfig();

    bool GetNomadStorage();
    bool GetEnforceTradeWindow();
    bool GetIgnoreCraftSkill();
    bool GetLegacyCommands();
    bool GetShortOutput();
    uint32_t GetRetryDelay();
    std::vector<int> GetDisabledBags();
    std::vector<int> GetEnabledBags();
    AutoTradeSetting GetAutoTradeSetting();
    std::vector<std::string> GetWhitelist();
    std::vector<std::string> GetBlacklist();
    ItemTypeMap GetItemTypes();
    ItemTypeEntryMap GetItemTypeEntries();
        
    void SetNomadStorage(bool enabled);
    void SetEnforceTradeWindow(bool enabled);
    void SetIgnoreCraftSkill(bool enabled);
    void SetLegacyCommands(bool enabled);
    void SetShortOutput(bool enabled);
    void SetRetryDelay(uint32_t delay);
    void SetDisabledBags(std::vector<int> disabledBags);
    void SetEnabledBags(std::vector<int> enabledBags);
    void SetAutoTradeSetting(AutoTradeSetting setting);
    void SetWhitelist(std::vector<std::string> whitelist);
    void SetBlacklist(std::vector<std::string> blacklist);
    void SetItemTypeEntries(ItemTypeEntryMap itemTypes);
    void ProcessItemTypes();

    static bool CheckWildcardMatch(const char* wc, const char* compare);
    static std::string FormatName(std::string input);

    ConfigSaveStyle GetDefaultSaveStyle() override;
    bool LoadConfig(xml_node<>* pRoot, const char* errorBuffer) override;
    void Reset() override;
    bool SaveConfig(std::ofstream* pStream, const char* errorBuffer) override;
};

#endif