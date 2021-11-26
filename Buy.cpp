#include "Bellhop.h"

void Bellhop::Buy(vector<string> Args, int ArgCount, CommandHelp HelpText)
{
    if (!mState.ShopActive)
    {
        OutputHelper::Output(Ashita::LogLevel::Error, "You do not currently have a shop active.");
        return;
    }

    if (ArgCount < 3)
    {
        PrintHelpText(HelpText, false);
        return;
    }

    int quantity = 1;
    if (ArgCount > 3)
        quantity = atoi(Args[3].c_str());

    if ((quantity < 1) || (quantity > 7920))
    {
        OutputHelper::Outputf(Ashita::LogLevel::Error, "Quantity must be between 1 and 7920.  You input: %d", quantity);
    }

    ShopItem_t matchData = {0};
    uint8_t matchSlot    = GetShopItem(Args[2], &matchData);
    if (matchData.Resource == NULL)
    {
        OutputHelper::Outputf(Ashita::LogLevel::Error, "No matching items found.  Term: %s", Args[2].c_str());
        return;
    }

    int spaceLimit = GetAvailableSpace(0) * matchData.Resource->StackSize;
    if (spaceLimit == 0)
    {
        OutputHelper::Output(Ashita::LogLevel::Error, "You have no open space.");
        return;
    }

    int gilLimit    = m_AshitaCore->GetMemoryManager()->GetInventory()->GetContainerItem(0, 0)->Count / matchData.Price;
    if (gilLimit == 0)
    {
        OutputHelper::Output(Ashita::LogLevel::Error, "You do not have enough gil.");
        return;
    }

    int boughtAmount = 0;
    int buyAmount = min(spaceLimit, min(gilLimit, quantity));

    while (boughtAmount < buyAmount)
    {
        pk_BuyFromNPC packet;
        packet.Index          = matchSlot;
        packet.Count         = min(buyAmount - boughtAmount, matchData.Resource->StackSize);
        m_AshitaCore->GetPacketManager()->AddOutgoingPacket(0x83, sizeof(packet), (uint8_t*)&packet);
        boughtAmount += packet.Count;
    }

    if (boughtAmount == 1)
        OutputHelper::Outputf(Ashita::LogLevel::Info, "Buying a $H%s$R for $H%ug$R.", matchData.Resource->LogNameSingular[0], matchData.Price);
    else
        OutputHelper::Outputf(Ashita::LogLevel::Info, "Buying $H%d %s$R for $H%ug$R.", boughtAmount, matchData.Resource->LogNamePlural[0], matchData.Price * boughtAmount);
}
void Bellhop::Buys(vector<string> Args, int ArgCount, CommandHelp HelpText)
{
    if (!mState.ShopActive)
    {
        OutputHelper::Output(Ashita::LogLevel::Error, "You do not currently have a shop active.");
        return;
    }

    if (ArgCount < 3)
    {
        PrintHelpText(HelpText, false);
        return;
    }

    ShopItem_t matchData = {0};
    uint8_t matchSlot    = GetShopItem(Args[2], &matchData);
    if (matchData.Resource == NULL)
    {
        OutputHelper::Outputf(Ashita::LogLevel::Error, "No matching items found.  Term: %s", Args[2].c_str());
        return;
    }

    if (GetAvailableSpace(0) == 0)
    {
        OutputHelper::Output(Ashita::LogLevel::Error, "You have no open space.");
        return;
    }

    int gilLimit   = m_AshitaCore->GetMemoryManager()->GetInventory()->GetContainerItem(0, 0)->Count / matchData.Price;
    if (gilLimit == 0)
    {
        OutputHelper::Output(Ashita::LogLevel::Error, "You do not have enough gil.");
        return;
    }

    int boughtAmount = 0;
    int buyAmount    = min(gilLimit, matchData.Resource->StackSize);

    while (boughtAmount < buyAmount)
    {
        pk_BuyFromNPC packet;
        packet.Index = matchSlot;
        packet.Count         = min(buyAmount - boughtAmount, matchData.Resource->StackSize);
        m_AshitaCore->GetPacketManager()->AddOutgoingPacket(0x83, sizeof(packet), (uint8_t*)&packet);
        boughtAmount += packet.Count;
    }

    if (boughtAmount == 1)
        OutputHelper::Outputf(Ashita::LogLevel::Info, "Buying a $H%s$R for $H%ug$R.", matchData.Resource->LogNameSingular[0], matchData.Price);
    else
        OutputHelper::Outputf(Ashita::LogLevel::Info, "Buying $H%d %s$R for $H%ug$R.", boughtAmount, matchData.Resource->LogNamePlural[0], matchData.Price * boughtAmount);
}
void Bellhop::BuyAll(vector<string> Args, int ArgCount, CommandHelp HelpText)
{
    if (!mState.ShopActive)
    {
        OutputHelper::Output(Ashita::LogLevel::Error, "You do not currently have a shop active.");
        return;
    }

    if (ArgCount < 3)
    {
        PrintHelpText(HelpText, false);
        return;
    }

    ShopItem_t matchData = {0};
    uint8_t matchSlot = GetShopItem(Args[2], &matchData);
    if (matchData.Resource == NULL)
    {
        OutputHelper::Outputf(Ashita::LogLevel::Error, "No matching items found.  Term: %s", Args[2].c_str());
        return;
    }

    int spaceLimit = GetAvailableSpace(0) * matchData.Resource->StackSize;
    if (spaceLimit == 0)
    {
        OutputHelper::Output(Ashita::LogLevel::Error, "You have no open space.");
        return;
    }

    int gilLimit = m_AshitaCore->GetMemoryManager()->GetInventory()->GetContainerItem(0, 0)->Count / matchData.Price;
    if (gilLimit == 0)
    {
        OutputHelper::Output(Ashita::LogLevel::Error, "You do not have enough gil.");
        return;
    }

    int boughtAmount = 0;
    int buyAmount    = min(spaceLimit, gilLimit);

    while (boughtAmount < buyAmount)
    {
        pk_BuyFromNPC packet;
        packet.Index = matchSlot;
        packet.Count         = min(buyAmount - boughtAmount, matchData.Resource->StackSize);
        m_AshitaCore->GetPacketManager()->AddOutgoingPacket(0x83, sizeof(packet), (uint8_t*)&packet);
        boughtAmount += packet.Count;
    }

    if (boughtAmount == 1)
        OutputHelper::Outputf(Ashita::LogLevel::Info, "Buying a $H%s$R for $H%ug$R.", matchData.Resource->LogNameSingular[0], matchData.Price);
    else
        OutputHelper::Outputf(Ashita::LogLevel::Info, "Buying $H%d %s$R for $H%ug$R.", boughtAmount, matchData.Resource->LogNamePlural[0], matchData.Price * boughtAmount);
}