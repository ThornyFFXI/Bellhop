#include "Bellhop.h"

void Bellhop::Get(vector<string> Args, int ArgCount, CommandHelp HelpText)
{
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

    int Container = -1;
    if (ArgCount > 4)
    {
        Container = GetContainerIndexFromName(Args[4].c_str());
        if (Container == -1)
        {
            OutputHelper::Outputf(Ashita::LogLevel::Error, "Invalid container specified.  You input: %s", Args[4].c_str());
            return;
        }
    }

    int invSpaceAvailable = GetAvailableSpace(0);
    if (invSpaceAvailable == 0)
    {
        OutputHelper::Output(Ashita::LogLevel::Error, "No available inventory space.");
        return;
    }

    std::list<ItemData_t> items;
    if (Container == -1)
        items = GetMatchingItemsNoInventory(Args[2]);
    else
        items = GetMatchingItems(Args[2], Container);

    
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

        //Skip furniture that is currently placed.
        if (((iter->Resource->Type == 10) || (iter->Resource->Type == 11) || (iter->Resource->Type == 12) || (iter->Resource->Type == 14)) && (iter->Item->Extra[1] & 0x40))
            continue;

        pk_MoveItem packet;
        packet.FromStorage = iter->Container;
        packet.Quantity    = min(Quantity - resultCount, iter->Item->Count);
        packet.FromIndex   = iter->Item->Index;
        m_AshitaCore->GetPacketManager()->AddOutgoingPacket(0x029, sizeof(pk_MoveItem), (uint8_t*)&packet);

        StoreResult(&results, ItemActionInfo(iter->Resource, packet.Quantity, packet.FromStorage));
        resultCount += packet.Quantity;

        invSpaceAvailable--;
        if ((invSpaceAvailable < 1) || (resultCount >= Quantity))
            break;
    }

    if (resultCount == 0)
    {
        OutputHelper::Outputf(Ashita::LogLevel::Error, "No matching items found.  Term: %s", Args[2].c_str());
        return;
    }
    else if ((results.size() > 1) && (mConfig.GetShortOutput()))
    {
        OutputHelper::Outputf(Ashita::LogLevel::Info, "Retrieving $H%u$R matching items.", resultCount);
    }
    else
    {
        for (std::list<ItemActionInfo>::iterator iter = results.begin(); iter != results.end(); iter++)
        {
            if (iter->Count == 1)
                OutputHelper::Outputf(Ashita::LogLevel::Info, "Retrieving a $H%s$R from $H%s$R.", iter->Resource->LogNameSingular[0], ContainerNames[iter->Location]);
            else
                OutputHelper::Outputf(Ashita::LogLevel::Info, "Retrieving $H%u %s$R from $H%s$R.", iter->Count, iter->Resource->LogNamePlural[0], ContainerNames[iter->Location]);
        }
    }
}
void Bellhop::Gets(vector<string> Args, int ArgCount, CommandHelp HelpText)
{
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

    int invSpaceAvailable = GetAvailableSpace(0);
    if (invSpaceAvailable == 0)
    {
        OutputHelper::Output(Ashita::LogLevel::Error, "No available inventory space.");
        return;
    }

    std::list<ItemData_t> items;
    if (Container == -1)
        items = GetMatchingItemsNoInventory(Args[2]);
    else
        items = GetMatchingItems(Args[2], Container);

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

        //Skip furniture that is currently placed.
        if (((iter->Resource->Type == 10) || (iter->Resource->Type == 11) || (iter->Resource->Type == 12) || (iter->Resource->Type == 14)) && (iter->Item->Extra[1] & 0x40))
            continue;

        pk_MoveItem packet;
        packet.FromStorage = iter->Container;
        packet.Quantity    = iter->Item->Count;
        packet.FromIndex   = iter->Item->Index;
        m_AshitaCore->GetPacketManager()->AddOutgoingPacket(0x029, sizeof(pk_MoveItem), (uint8_t*)&packet);

        if (packet.Quantity == 1)
            OutputHelper::Outputf(Ashita::LogLevel::Info, "Retrieving a $H%s$R from $H%s$R.", iter->Resource->LogNameSingular[0], ContainerNames[iter->Container]);
        else
            OutputHelper::Outputf(Ashita::LogLevel::Info, "Retrieving $H%u %s$R from $H%s$R.", packet.Quantity, iter->Resource->LogNamePlural[0], ContainerNames[iter->Container]);
        return;
    }

    OutputHelper::Outputf(Ashita::LogLevel::Error, "No matching items found.  Term: %s", Args[2].c_str());
}
void Bellhop::GetAll(vector<string> Args, int ArgCount, CommandHelp HelpText)
{
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

    int invSpaceAvailable = GetAvailableSpace(0);
    if (invSpaceAvailable == 0)
    {
        OutputHelper::Output(Ashita::LogLevel::Error, "No available inventory space.");
        return;
    }

    std::list<ItemData_t> items;
    if (Container == -1)
        items = GetMatchingItemsNoInventory(Args[2]);
    else
        items = GetMatchingItems(Args[2], Container);

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

        //Skip furniture that is currently placed.
        if (((iter->Resource->Type == 10) || (iter->Resource->Type == 11) || (iter->Resource->Type == 12) || (iter->Resource->Type == 14)) && (iter->Item->Extra[1] & 0x40))
            continue;

        pk_MoveItem packet;
        packet.FromStorage = iter->Container;
        packet.Quantity    = iter->Item->Count;
        packet.FromIndex   = iter->Item->Index;
        m_AshitaCore->GetPacketManager()->AddOutgoingPacket(0x029, sizeof(pk_MoveItem), (uint8_t*)&packet);

        StoreResult(&results, ItemActionInfo(iter->Resource, packet.Quantity, packet.FromStorage));
        resultCount += packet.Quantity;

        invSpaceAvailable--;
        if (invSpaceAvailable < 1)
            break;
    }

    if (resultCount == 0)
    {
        OutputHelper::Outputf(Ashita::LogLevel::Error, "No matching items found.  Term: %s", Args[2].c_str());
        return;
    }
    else if ((results.size() > 1) && (mConfig.GetShortOutput()))
    {
        OutputHelper::Outputf(Ashita::LogLevel::Info, "Retrieving $H%u$R matching items.", resultCount);
    }
    else
    {
        for (std::list<ItemActionInfo>::iterator iter = results.begin(); iter != results.end(); iter++)
        {
            if (iter->Count == 1)
                OutputHelper::Outputf(Ashita::LogLevel::Info, "Retrieving a $H%s$R from $H%s$R.", iter->Resource->LogNameSingular[0], ContainerNames[iter->Location]);
            else
                OutputHelper::Outputf(Ashita::LogLevel::Info, "Retrieving $H%u %s$R from $H%s$R.", iter->Count, iter->Resource->LogNamePlural[0], ContainerNames[iter->Location]);
        }
    }
}