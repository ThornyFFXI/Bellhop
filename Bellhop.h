#ifndef __ASHITA_Bellhop_H_INCLUDED__
#define __ASHITA_Bellhop_H_INCLUDED__

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

#include "C:\Ashita 4\plugins\sdk\Ashita.h"
#include "..\common\Utilities.h"
#include "..\common\Output.h"
#include "..\common\Settings.h"
#include "Packets.h"
#include "Structs.h"

#define CONTAINER_MAX 13
extern const char* ContainerNames[CONTAINER_MAX];

class Bellhop : IPlugin
{
private:
    IAshitaCore* m_AshitaCore;
    ILogManager* m_LogManager;
    uint32_t m_PluginId;

    OutputHelpers* pOutput;
    SettingsHelper* pSettings;
    Settings_t mSettings;
    State_t mState;
    DWORD pWardrobe;
    DWORD pZoneFlags;
    DWORD pZoneOffset;
    typedef void (Bellhop::*CommandHandler)(vector<string>, int, CommandHelp help);
    struct CommandInformation
    {
        CommandHandler handler;
        CommandHelp help;
    };
    std::map<string, CommandInformation, cistringcmp> mCommandMap;

public:
    const char* GetName(void) const override
    {
        return u8"Bellhop";
    }
    const char* GetAuthor(void) const override
    {
        return u8"Thorny";
    }
    const char* GetDescription(void) const override
    {
        return u8"Insert description here.";
    }
    const char* GetLink(void) const override
    {
        return u8"Insert link here.";
    }
    double GetVersion(void) const override
    {
        return 1.11f;
    }
    int32_t GetPriority(void) const override
    {
        return 0;
    }
    uint32_t GetFlags(void) const override
    {
        return (uint32_t)Ashita::PluginFlags::Legacy;
    }
	
    // Main.cpp
    bool Initialize(IAshitaCore* core, ILogManager* logger, const uint32_t id) override;
    void Release(void) override;
    void InitializeCommands();
    void InitializeState();

    // Commands.cpp
    bool HandleCommand(int32_t mode, const char* command, bool injected) override;
    uint32_t ParseLegacyCommands(const char* command, std::vector<std::string>* args);

    //Command Handlers
    void Bazaar(vector<string> Args, int ArgCount, CommandHelp HelpText);
    void BazaarAll(vector<string> Args, int ArgCount, CommandHelp HelpText);
    void Drop(vector<string> Args, int ArgCount, CommandHelp HelpText);
    void DropAll(vector<string> Args, int ArgCount, CommandHelp HelpText);
    void Get(vector<string> Args, int ArgCount, CommandHelp HelpText);
    void Gets(vector<string> Args, int ArgCount, CommandHelp HelpText);
    void GetAll(vector<string> Args, int ArgCount, CommandHelp HelpText);
    void Put(vector<string> Args, int ArgCount, CommandHelp HelpText);
    void Puts(vector<string> Args, int ArgCount, CommandHelp HelpText);
    void PutAll(vector<string> Args, int ArgCount, CommandHelp HelpText);
    void Sell(vector<string> Args, int ArgCount, CommandHelp HelpText);
    void Sells(vector<string> Args, int ArgCount, CommandHelp HelpText);
    void SellAll(vector<string> Args, int ArgCount, CommandHelp HelpText);
    void Buy(vector<string> Args, int ArgCount, CommandHelp HelpText);
    void Buys(vector<string> Args, int ArgCount, CommandHelp HelpText);
    void BuyAll(vector<string> Args, int ArgCount, CommandHelp HelpText);
    void Sort(vector<string> Args, int ArgCount, CommandHelp HelpText);
    void TradeToPlayer(vector<string> Args, int ArgCount, CommandHelp HelpText);
    void TradeAllToPlayer(vector<string> Args, int ArgCount, CommandHelp HelpText);
    void TradeToNpc(vector<string> Args, int ArgCount, CommandHelp HelpText);
    void TradeAllToNpc(vector<string> Args, int ArgCount, CommandHelp HelpText);
    void Help(vector<string> Args, int ArgCount, CommandHelp HelpText);


    void Default(vector<string> Args, int ArgCount, CommandHelp HelpText);
    void BlackList(vector<string> Args, int ArgCount, CommandHelp HelpText);
    void WhiteList(vector<string> Args, int ArgCount, CommandHelp HelpText);
    void EnableBags(vector<string> Args, int ArgCount, CommandHelp HelpText);
    void DisableBags(vector<string> Args, int ArgCount, CommandHelp HelpText);
    void Config(vector<string> Args, int ArgCount, CommandHelp HelpText);
    void HandleAutoTrade();

    // Packets.cpp
    bool HandleIncomingPacket(uint16_t id, uint32_t size, const uint8_t* data, uint8_t* modified, uint32_t sizeChunk, const uint8_t* dataChunk, bool injected, bool blocked) override;
    void HandleIncomingPacket_00A(uint32_t size, const uint8_t* data);
    void HandleIncomingPacket_00B(uint32_t size, const uint8_t* data);
    void HandleIncomingPacket_021(uint32_t size, const uint8_t* data);
    void HandleIncomingPacket_022(uint32_t size, const uint8_t* data);
    void HandleIncomingPacket_023(uint32_t size, const uint8_t* data);
    void HandleIncomingPacket_03C(uint32_t size, const uint8_t* data);
    bool HandleOutgoingPacket(uint16_t id, uint32_t size, const uint8_t* data, uint8_t* modified, uint32_t sizeChunk, const uint8_t* dataChunk, bool injected, bool blocked) override;
    void HandleOutgoingPacket_015(uint32_t size, const uint8_t* data);
    void HandleOutgoingPacket_01A(uint32_t size, const uint8_t* data);
    void HandleOutgoingPacket_109(uint32_t size, const uint8_t* data);
    void HandleOutgoingPacket_10B(uint32_t size, const uint8_t* data);

    // Settings.cpp
    void LoadSettings();
    void LoadSettings(std::string Path);
    void SaveSettings();
    void SaveSettings(std::string Path);
    void WriteSettings(ofstream* outStream);

    // Helpers.cpp
    std::list<ItemData_t> GetMatchingItemsNoInventory(std::string Parameter);
    std::list<ItemData_t> GetMatchingItems(std::string Parameter, int Container);
    int GetMatchingItemsByWildcard(std::string Parameter, int Container, std::list<ItemData_t>* Output);
    int GetMatchingItemsByResource(IItem* pResource, int Container, std::list<ItemData_t>* Output);
    int GetMatchingItemsByType(std::list<uint16_t> Ids, int Container, std::list<ItemData_t>* Output);
    uint8_t GetShopItem(std::string Parameter, ShopItem_t* buffer);
    int GetAvailableSpace(int Container);
    int GetContainerIndexFromName(const char* ContainerName);
    AutoTradeSetting GetTradeResponse(std::string Name);
    uint16_t StringToUint16OrZero(std::string Parameter);
    bool CanStoreItem(int Container, ItemData_t item);
    bool CheckItemMatch(std::string Parameter, IItem* pResource);
    bool CheckWildcardMatch(const char* wc, const char* compare);
    void CheckContainers(uint8_t* buffer);
    std::string FormatName(std::string Input);
    void PrintHelpText(CommandHelp help, bool Description);
    bool AddToList(std::list<string>* List, std::string Name);
    bool RemoveFromList(std::list<string>* List, std::string Name);
    void StoreResult(std::list<ItemActionInfo>* List, ItemActionInfo Entry);
};
#endif