#include "Bellhop.h"
#pragma comment(lib, "psapi.lib")
#include <psapi.h>
OutputHelper* OutputHelper::pOutputHelper = nullptr;

const char* ContainerNames[CONTAINER_MAX] =
    {
        "inventory",
        "safe",
        "storage",
        "temporary",
        "locker",
        "satchel",
        "sack",
        "case",
        "wardrobe",
        "safe2",
        "wardrobe2",
        "wardrobe3",
        "wardrobe4",
        "wardrobe5",
        "wardrobe6",
        "wardrobe7",
        "wardrobe8"};

__declspec(dllexport) IPlugin* __stdcall expCreatePlugin(const char* args)
{
    UNREFERENCED_PARAMETER(args);

    return (IPlugin*)(new Bellhop());
}

__declspec(dllexport) double __stdcall expGetInterfaceVersion(void)
{
    return ASHITA_INTERFACE_VERSION;
}

bool Bellhop::Initialize(IAshitaCore* core, ILogManager* logger, const uint32_t id)
{
    m_AshitaCore = core;
    m_LogManager = logger;
    m_PluginId   = id;
    OutputHelper::Initialize(core, logger, "Bellhop");
    ConfigLoader::pLoader = new ConfigLoader(core, &mConfig);

    MODULEINFO mod = {0};
    ::GetModuleInformation(::GetCurrentProcess(), ::GetModuleHandle("FFXiMain.dll"), &mod, sizeof(MODULEINFO));
    pWardrobe  = Ashita::Memory::FindPattern((uintptr_t)mod.lpBaseOfDll, (uintptr_t)mod.SizeOfImage, "A1????????8B88B4000000C1E907F6C101E9", 1, 0);
    pZoneFlags = Ashita::Memory::FindPattern((uintptr_t)mod.lpBaseOfDll, (uintptr_t)mod.SizeOfImage, "8B8C24040100008B90????????0BD18990????????8B15????????8B82", 0, 0);

    if (pWardrobe == 0)
    {
        OutputHelper::Output(Ashita::LogLevel::Error, "Wardrobe status signature scan failed.");
        OutputHelper::Destroy();
        return false;
    }

    if (pZoneFlags == 0)
    {
        OutputHelper::Output(Ashita::LogLevel::Error, "Zone flag signature scan failed.");
        OutputHelper::Destroy();
        return false;
    }

    pZoneOffset = Read32(pZoneFlags, 0x09);
    if (pZoneOffset == 0)
    {
        OutputHelper::Output(Ashita::LogLevel::Error, "Zone flag offset not found.");
        OutputHelper::Destroy();
        return false;
    }

    pZoneFlags = Read32(pZoneFlags, 0x17);
    if (pZoneFlags == 0)
    {
        OutputHelper::Output(Ashita::LogLevel::Error, "Zone flag sub pointer not found.");
        OutputHelper::Destroy();
        return false;
    }
        
    InitializeCommands();
    InitializeState();

    return true;
}

void Bellhop::Release(void)
{
    OutputHelper::Destroy();
}

void Bellhop::InitializeCommands()
{
    CommandInformation buff;

    buff.handler          = &Bellhop::Bazaar;
    buff.help.command     = "/bh bazaar [required: item] [required: price]";
    buff.help.description = "Bazaar a matching item.  You must have the bazaar pricing menu already open.";
    mCommandMap["bazaar"] = buff;

    buff.handler             = &Bellhop::BazaarAll;
    buff.help.command        = "/bh bazaarall [required: item] [required: price]";
    buff.help.description    = "Bazaar all matching items.  You must have the bazaar pricing menu already open.";
    mCommandMap["bazaarall"] = buff;

    buff.handler          = &Bellhop::Drop;
    buff.help.command     = "/bh drop [required: item]";
    buff.help.description = "Drop a matching item from inventory.";
    mCommandMap["drop"]   = buff;

    buff.handler           = &Bellhop::DropAll;
    buff.help.command      = "/bh dropall [required: item]";
    buff.help.description  = "Drop all matching items from inventory.";
    mCommandMap["dropall"] = buff;

    buff.handler          = &Bellhop::Get;
    buff.help.command     = "/bh get [required: item] [optional: quantity] [optional: container]";
    buff.help.description = "Retrieve up to the specified quantity(or 1) of matching items from available containers.  If container is specified, quantity must be specified and Bellhop will only search that container.";
    mCommandMap["get"]    = buff;

    buff.handler          = &Bellhop::Gets;
    buff.help.command     = "/bh gets [required: item] [optional: container]";
    buff.help.description = "Retrieve up to a stack of matching items from available containers.  If container is specified, will only search that container.";
    mCommandMap["gets"]   = buff;

    buff.handler          = &Bellhop::GetAll;
    buff.help.command     = "/bh getall [required: item] [optional: container]";
    buff.help.description = "Retrieve as many matching items as your inventory can hold from available containers.  If container is specified, will only search that container.";
    mCommandMap["getall"] = buff;

    buff.handler          = &Bellhop::Put;
    buff.help.command     = "/bh put [required: item] [optional: quantity] [optional: container]";
    buff.help.description = "Store up to the specified quantity(or 1) of matching items in available containers.  If container is specified, quantity must be specified and Bellhop will only use that container.";
    mCommandMap["put"]    = buff;

    buff.handler          = &Bellhop::Puts;
    buff.help.command     = "/bh puts [required: item] [optional: container]";
    buff.help.description = "Store up to a stack of matching items in available containers.  If container is specified, will only use that container.";
    mCommandMap["puts"]    = buff;

    buff.handler          = &Bellhop::PutAll;
    buff.help.command     = "/bh putall [required: item] [optional: container]";
    buff.help.description = "Store as many matching items in available containers as those containers can hold.  If container is specified, will only use that container.";
    mCommandMap["putall"]    = buff;

    buff.handler = &Bellhop::Sell;
    buff.help.command = "/bh sell [required: item] [optional: quantity]";
    buff.help.description = "Sell up to the specified quantity(or 1) of matching items to NPC.  You must have a NPC shop open.";
    mCommandMap["sell"]   = buff;

    buff.handler = &Bellhop::Sells;
    buff.help.command = "/bh sells [required: item]";
    buff.help.description = "Sell up to a stack of matching items to NPC.  You must have a NPC shop open.";
    mCommandMap["sells"]  = buff;

    buff.handler = &Bellhop::SellAll;
    buff.help.command = "/bh sellall [required: item]";
    buff.help.description = "Sell all matching items to NPC.  You must have a NPC shop open.";
    mCommandMap["sellall"] = buff;

    buff.handler = &Bellhop::Buy;
    buff.help.command = "/bh buy [required: item] [optional: quantity]";
    buff.help.description = "Buy up to the specified quantity(or 1) of matching items from NPC.  You must have a NPC shop open that has that item available.";
    mCommandMap["buy"]    = buff;

    buff.handler = &Bellhop::Buys;
    buff.help.command = "/bh buys [required: item]";
    buff.help.description = "Buy up to a stack of matching items from NPC.  You must have a NPC shop open that has that item available.";
    mCommandMap["buys"]   = buff;

    buff.handler = &Bellhop::BuyAll;
    buff.help.command = "/bh buyall [required: item]";
    buff.help.description = "Buy as many matching items from NPC as your inventory can hold, or your gil can afford.  You must have a NPC shop open that has that item available.";
    mCommandMap["buyall"] = buff;

    buff.handler = &Bellhop::Sort;
    buff.help.command = "/bh sort [required: container]";
    buff.help.description = "Sort a container.  This does not rearrange items like the sort button, it only combines stacks.";
    mCommandMap["sort"]   = buff;

    buff.handler = &Bellhop::TradeToPlayer;
    buff.help.command = "/bh tradeplayer [required: item] [optional: quantity] [optional: item2] [optional: quantity2] [etc]";
    buff.help.description = "Trade exactly the specified quantity(or 1) of matching items to a player.  You must already have the trade menu open.  Quantity is required for all except the last item when using multiple items.";
    mCommandMap["tradeplayer"] = buff;

    buff.handler = &Bellhop::TradeAllToPlayer;
    buff.help.command = "/bh tradeplayerall [required: item] [optional: item] [etc]";
    buff.help.description = "Trade all matching items to a player.  You must already have the trade menu open.  If more items are specified than fit in the trade window, the 8 largest stacks of matching items will be used.";
    mCommandMap["tradeplayerall"] = buff;

    buff.handler               = &Bellhop::TradeToNpc;
    buff.help.command          = "/bh tradenpc [required: item] [optional: quantity] [optional: item2] [optional: quantity2] [etc]";
    buff.help.description      = "Trade exactly the specified quantity(or 1) of matching items to a npc.  You must have the NPC targeted.  Quantity is required for all except the last item when using multiple items.";
    mCommandMap["tradenpc"] = buff;

    buff.handler                  = &Bellhop::TradeAllToNpc;
    buff.help.command             = "/bh tradenpcall [required: item] [optional: item] [etc]";
    buff.help.description         = "Trade all matching items to a NPC.  You must have the NPC targeted.  If more items are specified than fit in the trade window, the 8 largest stacks of matching items will be used.";
    mCommandMap["tradenpcall"] = buff;

    buff.handler           = &Bellhop::Default;
    buff.help.command = "/bh default [accept/deny/ignore]";
    buff.help.description = "Set bellhop's default reaction to trade requests and confirmations.";
    mCommandMap["default"] = buff;

    buff.handler = &Bellhop::BlackList;
    buff.help.command = "/bh blacklist [add/remove/clear/print] [required: name]";
    buff.help.description = "Edits or displays the autotrade blacklist.  Name is required when using add or remove.";
    mCommandMap["blacklist"] = buff;

    buff.handler             = &Bellhop::WhiteList;
    buff.help.command        = "/bh whitelist [add/remove/clear/print] [required: name]";
    buff.help.description    = "Edits or displays the autotrade whitelist.  Name is required when using add or remove.";
    mCommandMap["whitelist"] = buff;

    buff.handler = &Bellhop::EnableBags;
    buff.help.command = "/bh enablebags [add/remove/clear/print] [required: name]";
    buff.help.description = "Edits or displays the forceenablebags list.  Name is required when using add or remove.";
    mCommandMap["enablebags"] = buff;

    buff.handler = &Bellhop::DisableBags;
    buff.help.command = "/bh disablebags [add/remove/clear/print] [required: name]";
    buff.help.description = "Edits or displays the forcedisablebags list.  Name is required when using add or remove.";
    mCommandMap["disablebags"] = buff;

    buff.handler = &Bellhop::Config;
    buff.help.command = "/bh config [nomadstorage/ignorecraftskill/legacycommands/shortoutput/retrydelay] [required: value]";
    buff.help.description = "Edits current settings.  Retrydelay takes a number in milliseconds, while the others only accept 'enable' or 'disable'";
    mCommandMap["config"] = buff;

    buff.handler             = &Bellhop::Help;
    buff.help.command        = "/bh help [optional: command]";
    buff.help.description    = "Displays available commands.  If a command is specified, describes it.";
    mCommandMap["help"] = buff;
}

void Bellhop::InitializeState()
{
    //Initialize to default values.
    mState.MyId   = 0;
    mState.MyName = "NO_NAME";
    mState.ShopActive = false;
    mState.BazaarOpen = false;
    mState.TradeDelay = std::chrono::steady_clock::now();

    //Check if we're already ingame
    uint16_t myIndex = m_AshitaCore->GetMemoryManager()->GetParty()->GetMemberTargetIndex(0);
    if (myIndex < 1)
        return;
    if (m_AshitaCore->GetMemoryManager()->GetEntity()->GetRawEntity(myIndex) == 0)
        return;
    if ((m_AshitaCore->GetMemoryManager()->GetEntity()->GetRenderFlags0(myIndex) & 0x200) == 0)
        return;
    if ((m_AshitaCore->GetMemoryManager()->GetEntity()->GetRenderFlags0(myIndex) & 0x4000) != 0)
        return;

    //We're ingame, so fill in ID/Name.
    mState.MyId   = m_AshitaCore->GetMemoryManager()->GetEntity()->GetServerId(myIndex);
    mState.MyName = m_AshitaCore->GetMemoryManager()->GetEntity()->GetName(myIndex);
}