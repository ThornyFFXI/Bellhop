#ifndef __ASHITA_Bellhop_Packets_H_INCLUDED__
#define __ASHITA_Bellhop_Packets_H_INCLUDED__

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif
#pragma pack(1)

struct pk_TradePlayer
{
    uint32_t Header;
    uint32_t Type;
    uint16_t TradeCount;
    uint16_t Unknown;

    pk_TradePlayer()
        : Header(0)
        , Type(0)
        , TradeCount(0)
        , Unknown(0)
    {}
};

struct pk_SetBazaar
{
    uint32_t Header;
    uint32_t Index;
    uint32_t Price;
    
    pk_SetBazaar()
        : Header(0)
        , Index(0)
        , Price(0)
    {}
};

struct pk_BuyFromNpc
{
    uint32_t Header;
    uint32_t Count;
    uint16_t Unknown0000;
    uint32_t Slot;
    uint16_t Unknown0001;

    pk_BuyFromNpc()
        : Header(0)
        , Count(0)
        , Unknown0000(0)
        , Slot(0)
        , Unknown0001(0)
    {}
};

struct pk_DropItem
{
    uint32_t Header;
    uint32_t Quantity;
    uint8_t Location;
    uint8_t Index;
    uint16_t Unknown0000;

    pk_DropItem()
        : Header(0)
        , Quantity(0)
        , Location(0)
        , Index(0)
        , Unknown0000(0)
    {}
};

struct pk_MoveItem
{
    uint32_t Header;
    uint32_t Quantity;
    uint8_t FromStorage;
    uint8_t ToStorage;
    uint8_t FromIndex;
    uint8_t ToIndex;

    pk_MoveItem()
        : Header(0)
        , Quantity(0)
        , FromStorage(0)
        , ToStorage(0)
        , FromIndex(0)
        , ToIndex(81)
    {}
};

struct pk_SortInventory
{
    uint32_t Header;
    uint32_t Container;

    pk_SortInventory()
        : Header(0)
        , Container(0)
    {}
};

struct pk_NpcSellRequest
{
    uint32_t Header;
    uint32_t Quantity;
    uint16_t Id;
    uint8_t Index;
    uint8_t Unknown0000;

    pk_NpcSellRequest()
        : Header(0)
        , Quantity(0)
        , Id(0)
        , Index(0)
        , Unknown0000(0)
    {}
};
struct pk_NpcSellConfirm
{
    uint32_t Header;
    uint32_t Unknown;

    pk_NpcSellConfirm()
        : Header(0)
        , Unknown(1)
    {}
};

struct pk_TradeToNpc
{
    uint32_t Header;
    uint32_t TargetId;
    int32_t ItemQuantity[10];
    uint8_t ItemIndex[10];
    uint16_t TargetIndex;
    uint32_t TotalItems;

    pk_TradeToNpc()
        : Header(0)
        , TargetId(0)
        , TargetIndex(0)
        , TotalItems(0)
    {
        for (int x = 0; x < 10; x++)
        {
            ItemQuantity[x] = 0;
            ItemIndex[x]    = 0;
        }
    }
};

struct pk_AddItemToPlayerTrade
{
    uint32_t Header;
    uint32_t Quantity;
    uint16_t Id;
    uint8_t Index;
    uint8_t Slot;

    pk_AddItemToPlayerTrade()
        : Header(0)
        , Quantity(0)
        , Id(0)
        , Index(0)
        , Slot(0)
    {}
};
#endif