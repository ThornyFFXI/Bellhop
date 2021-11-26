#ifndef __ASHITA_Lootwhore_Structs_H_INCLUDED__
#define __ASHITA_Lootwhore_Structs_H_INCLUDED__

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

#include <chrono>
#include <list>
#include <map>
#include <stdint.h>
#include <string>

enum class TradeState
{
    TradeClosed,
    IncomingOffer,
    OutgoingOffer,
    TradeOpen,
    TradeConfirmedBySelf,
    TradeConfirmedByOther
};

struct ShopItem_t
{
    bool Enabled;
    uint32_t Price;
    IItem* Resource;
};

struct ItemActionInfo
{
    IItem* Resource;
    uint32_t Count;
    int32_t Location;

    ItemActionInfo(IItem* Resource, uint32_t Count)
        : Resource(Resource)
        , Count(Count)
        , Location(0)
    {}

    ItemActionInfo(IItem* Resource, uint32_t Count, int32_t Location)
        : Resource(Resource)
        , Count(Count)
        , Location(Location)
    {}
};

struct State_t
{
    std::string MyName;
    uint32_t MyId;
    std::string TradeName;
    TradeState TradeState;
    uint16_t TradeSync;
    std::chrono::steady_clock::time_point TradeDelay;
    bool BazaarOpen;
    bool ShopActive;
    uint16_t ShopIndex;
    std::string ShopName;
    std::map<uint8_t, ShopItem_t> ShopStock;

    State_t()
        : MyName("Unknown")
        , MyId(0)
        , TradeState(TradeState::TradeClosed)
        , TradeSync(0)
        , TradeDelay(std::chrono::steady_clock::now())
        , BazaarOpen(false)
        , ShopActive(false)
        , ShopIndex(false)
        , ShopName("Unknown")
        , ShopStock(std::map<uint8_t, ShopItem_t>())
    {}
};

struct ItemData_t
{
    uint8_t Container;
    IItem* Resource;
    Ashita::FFXI::item_t* Item;

    ItemData_t(int Container, IItem* Resource, Ashita::FFXI::item_t* Item)
        : Container(Container)
        , Resource(Resource)
        , Item(Item)
    {}
};

struct cistringcmp
{
    bool operator()(const string& lhs, const string& rhs) const
    {
        return (_stricmp(lhs.c_str(), rhs.c_str()) < 0);
    }
};
struct CommandHelp
{
    string command;
    string description;
};
#endif