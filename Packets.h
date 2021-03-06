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

//0x83
struct pk_BuyFromNPC
{
    const uint32_t Header = 0x00000000;
    uint8_t Count;
    const uint8_t Padding[5] = {0};
    uint8_t Index;
    const uint8_t Padding2[5] = {0};

    pk_BuyFromNPC()
        : Count(0)
        , Index(0)
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
        , ToIndex(82)
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

    //Legitimate client always puts gil first in NPC trades.
    void ArrangeGil()
    {
        for (int x = 1; x < TotalItems; x++)
        {
            if (ItemIndex[x] == 0)
            {
                int gilQuantity = ItemQuantity[x];
                ItemIndex[x]    = ItemIndex[0];
                ItemQuantity[x] = ItemQuantity[0];
                ItemIndex[0]    = 0;
                ItemQuantity[0] = gilQuantity;
            }
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
