#include "Bellhop.h"

void Bellhop::Sell(vector<string> Args, int ArgCount, CommandHelp HelpText)
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

    int Quantity = 1;
    if (ArgCount > 3)
    {
        Quantity = atoi(Args[3].c_str());
        if ((Quantity < 1) || (Quantity > 7920))
        {
            OutputHelper::Outputf(Ashita::LogLevel::Error, "Quantity must be between 1 and 7920.  You entered: %s", Args[3].c_str());
            return;
        }
    }

    std::list<ItemData_t> items = GetMatchingItems(Args[2], 0);
    if (items.size() == 0)
    {
        OutputHelper::Outputf(Ashita::LogLevel::Error, "No matching items found.  Term: %s", Args[2].c_str());
        return;
    }

    items.sort([Quantity](const ItemData_t a, const ItemData_t b) {
        //If we have an exact match, that always wins.
        if ((a.Item->Count == Quantity) != (b.Item->Count == Quantity))
            return (a.Item->Count == Quantity);

        //When dealing with less than a stack, pick the cleanest moves to maintain inventory better.
        if ((a.Resource->StackSize == b.Resource->StackSize) && (Quantity < a.Resource->StackSize))
        {
            //If only one of them is enough of the item, prefer that one.
            if ((a.Item->Count > Quantity) != (b.Item->Count > Quantity))
            {
                return (a.Item->Count > b.Item->Count);
            }

            //If both have enough, prefer the smaller partial stack.
            if (a.Item->Count > Quantity)
                return (a.Item->Count < b.Item->Count);

            //If neither have enough, proceed to higher quantity check.
        }

        //Sort by highest quantity of item.
        return (a.Item->Count > b.Item->Count);
    });

    std::list<ItemActionInfo> results;
    uint32_t resultCount = 0;
    for (std::list<ItemData_t>::iterator iter = items.begin(); iter != items.end(); iter++)
    {
        //Skip gil
        if (iter->Item->Id == 65535)
            continue;

        //Skip equipped gear, bazaared item, equipped linkshell.
        if ((iter->Item->Flags == 5) || (iter->Item->Flags == 19) || (iter->Item->Flags == 25))
            continue;

        //Skip non-sellable item
        if (iter->Resource->Flags & 0x1000)
            continue;

        pk_NpcSellRequest packet;
        packet.Index = iter->Item->Index;
        packet.Id    = iter->Item->Id;
        packet.Quantity = min(Quantity - resultCount, iter->Item->Count);
        m_AshitaCore->GetPacketManager()->AddOutgoingPacket(0x84, sizeof(pk_NpcSellRequest), (uint8_t*)&packet);

        pk_NpcSellConfirm packet2;
        m_AshitaCore->GetPacketManager()->AddOutgoingPacket(0x85, sizeof(pk_NpcSellConfirm), (uint8_t*)&packet2);

        StoreResult(&results, ItemActionInfo(iter->Resource, packet.Quantity));
        resultCount += packet.Quantity;

        if (resultCount >= Quantity)
            break;
    }

    if (resultCount == 0)
    {
        OutputHelper::Output(Ashita::LogLevel::Error, "None of the matching items can be sold.");
        return;
    }
    else if ((results.size() > 1) && (mConfig.GetShortOutput()))
    {
        OutputHelper::Outputf(Ashita::LogLevel::Info, "Selling $H%u matching items$R to $H%s$R.", resultCount, mState.ShopName.c_str());
    }
    else
    {
        for (std::list<ItemActionInfo>::iterator iter = results.begin(); iter != results.end(); iter++)
        {
            if (iter->Count == 1)
                OutputHelper::Outputf(Ashita::LogLevel::Info, "Selling a $H%s$R to $H%s$R.", iter->Resource->LogNameSingular[0], mState.ShopName.c_str());
            else
                OutputHelper::Outputf(Ashita::LogLevel::Info, "Selling $H%u %s$R to $H%s$R.", iter->Count, iter->Resource->LogNamePlural[0], mState.ShopName.c_str());
        }
    }
}
void Bellhop::Sells(vector<string> Args, int ArgCount, CommandHelp HelpText)
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

    std::list<ItemData_t> items = GetMatchingItems(Args[2], 0);
    if (items.size() == 0)
    {
        OutputHelper::Outputf(Ashita::LogLevel::Error, "No matching items found.  Term: %s", Args[2].c_str());
        return;
    }

    items.sort([](const ItemData_t a, const ItemData_t b) {
        //Sort by highest quantity of item.
        return (a.Item->Count > b.Item->Count);
    });

    for (std::list<ItemData_t>::iterator iter = items.begin(); iter != items.end(); iter++)
    {
        //Skip gil
        if (iter->Item->Id == 65535)
            continue;

        //Skip equipped gear, bazaared item, equipped linkshell.
        if ((iter->Item->Flags == 5) || (iter->Item->Flags == 19) || (iter->Item->Flags == 25))
            continue;

        //Skip non-sellable item.
        if (iter->Resource->Flags & 0x1000)
            continue;
        
        pk_NpcSellRequest packet;
        packet.Index    = iter->Item->Index;
        packet.Id       = iter->Item->Id;
        packet.Quantity = iter->Item->Count;
        m_AshitaCore->GetPacketManager()->AddOutgoingPacket(0x84, sizeof(pk_NpcSellRequest), (uint8_t*)&packet);

        pk_NpcSellConfirm packet2;
        m_AshitaCore->GetPacketManager()->AddOutgoingPacket(0x85, sizeof(pk_NpcSellConfirm), (uint8_t*)&packet2);

        if (packet.Quantity == 1)
            OutputHelper::Outputf(Ashita::LogLevel::Info, "Selling a $H%s$R to $H%s$R.", iter->Resource->LogNameSingular[0], mState.ShopName.c_str());
        else
            OutputHelper::Outputf(Ashita::LogLevel::Info, "Selling $H%u %s$R to $H%s$R.", packet.Quantity, iter->Resource->LogNamePlural[0], mState.ShopName.c_str());
        return;
    }

    OutputHelper::Output(Ashita::LogLevel::Error, "None of the matching items can be sold.");
}
void Bellhop::SellAll(vector<string> Args, int ArgCount, CommandHelp HelpText)
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

    int Container = -1;
    if (ArgCount > 3)
    {
        Container = GetContainerIndexFromName(Args[3].c_str());
        if (Container == -1)
        {
            OutputHelper::Outputf(Ashita::LogLevel::Error, "Invalid container specified.  You input: %s", Args[3].c_str());
            return;
        }
    }

    std::list<ItemData_t> items = GetMatchingItems(Args[2], 0);
    if (items.size() == 0)
    {
        OutputHelper::Outputf(Ashita::LogLevel::Error, "No matching items found.  Term: %s", Args[2].c_str());
        return;
    }

    items.sort([](const ItemData_t a, const ItemData_t b) {
        //Sort by highest quantity of item.
        return (a.Item->Count > b.Item->Count);
    });

    std::list<ItemActionInfo> results;
    uint32_t resultCount = 0;
    for (std::list<ItemData_t>::iterator iter = items.begin(); iter != items.end(); iter++)
    {
        //Skip gil
        if (iter->Item->Id == 65535)
            continue;

        //Skip equipped gear, bazaared item, equipped linkshell.
        if ((iter->Item->Flags == 5) || (iter->Item->Flags == 19) || (iter->Item->Flags == 25))
            continue;

        //Skip non-sellable item.
        if (iter->Resource->Flags & 0x1000)
            continue;

        pk_NpcSellRequest packet;
        packet.Index    = iter->Item->Index;
        packet.Id       = iter->Item->Id;
        packet.Quantity = iter->Item->Count;
        m_AshitaCore->GetPacketManager()->AddOutgoingPacket(0x84, sizeof(pk_NpcSellRequest), (uint8_t*)&packet);

        pk_NpcSellConfirm packet2;
        m_AshitaCore->GetPacketManager()->AddOutgoingPacket(0x85, sizeof(pk_NpcSellConfirm), (uint8_t*)&packet2);

        StoreResult(&results, ItemActionInfo(iter->Resource, packet.Quantity));
        resultCount += packet.Quantity;
    }

    if (resultCount == 0)
    {
        OutputHelper::Output(Ashita::LogLevel::Error, "None of the matching items can be sold.");
        return;
    }
    else if ((results.size() > 1) && (mConfig.GetShortOutput()))
    {
        OutputHelper::Outputf(Ashita::LogLevel::Info, "Selling $H%u matching items$R to $H%s$R.", resultCount, mState.ShopName.c_str());
    }
    else
    {
        for (std::list<ItemActionInfo>::iterator iter = results.begin(); iter != results.end(); iter++)
        {
            if (iter->Count == 1)
                OutputHelper::Outputf(Ashita::LogLevel::Info, "Selling a $H%s$R to $H%s$R.", iter->Resource->LogNameSingular[0], mState.ShopName.c_str());
            else
                OutputHelper::Outputf(Ashita::LogLevel::Info, "Selling $H%u %s$R to $H%s$R.", iter->Count, iter->Resource->LogNamePlural[0], mState.ShopName.c_str());
        }
    }
}