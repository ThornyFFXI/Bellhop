#include "Bellhop.h"

void Bellhop::Put(vector<string> Args, int ArgCount, CommandHelp HelpText)
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
            pOutput->error_f("Quantity must be between 1 and 7920.  You entered: %s", Args[3].c_str());
            return;
        }
    }

    int Container = -1;
    if (ArgCount > 4)
    {
        Container = GetContainerIndexFromName(Args[4].c_str());
        if (Container == -1)
        {
            pOutput->error_f("Invalid container specified.  You input: %s", Args[4].c_str());
            return;
        }
    }

    //Populate a list of which bags have space in them and are currently available.
    uint8_t space[CONTAINER_MAX];
    CheckContainers(space);
    bool spaceFound = false;
    for (int x = 0; x < CONTAINER_MAX; x++)
    {
        if (space[x])
        {
            if ((Container == -1) || (Container == x))
                space[x] = GetAvailableSpace(x);
            else
                space[x] = 0;
            if (space[x])
                spaceFound = true;
        }
    }
    if (!spaceFound)
    {
        pOutput->error("No available space to store items.");
        return;
    }

    
    std::list<ItemData_t> items = GetMatchingItems(Args[2], 0);
    if (items.size() == 0)
    {
        pOutput->error_f("No matching items found.  Term: %s", Args[2].c_str());
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

        pk_MoveItem packet;
        for (int x = 0; x < 13; x++)
        {
            if ((space[x] > 0) && (CanStoreItem(x, *iter)))
            {
                packet.ToStorage = x;
                space[x]--;            
            }
        }

        //Skip item that has no place to be stored.
        if (packet.ToStorage == 0)
            continue;

        packet.FromStorage = 0;
        packet.Quantity    = min(Quantity - resultCount, iter->Item->Count);
        packet.FromIndex   = iter->Item->Index;
        m_AshitaCore->GetPacketManager()->AddOutgoingPacket(0x029, sizeof(pk_MoveItem), (uint8_t*)&packet);

        StoreResult(&results, ItemActionInfo(iter->Resource, packet.Quantity, packet.ToStorage));
        resultCount += packet.Quantity;

        if (resultCount >= Quantity)
            break;
    }

    if (resultCount == 0)
    {
        pOutput->error("None of the available storage containers can hold a matching item.");
        return;
    }
    else if ((results.size() > 1) && (mSettings.ShortOutput))
    {
        pOutput->message_f("Storing $H%u matching items$R.", resultCount);
    }
    else
    {
        for (std::list<ItemActionInfo>::iterator iter = results.begin(); iter != results.end(); iter++)
        {
            if (iter->Count == 1)
                pOutput->message_f("Storing a $H%s$R in $H%s$R.", iter->Resource->LogNameSingular[0], ContainerNames[iter->Location]);
            else
                pOutput->message_f("Storing $H%u %s$R in $H%s$R.", iter->Count, iter->Resource->LogNamePlural[0], ContainerNames[iter->Location]);
        }
    }
}
void Bellhop::Puts(vector<string> Args, int ArgCount, CommandHelp HelpText)
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
            pOutput->error_f("Invalid container specified.  You input: %s", Args[3].c_str());
            return;
        }
    }

    //Populate a list of which bags have space in them and are currently available.
    uint8_t space[CONTAINER_MAX];
    CheckContainers(space);
    bool spaceFound = false;
    for (int x = 0; x < CONTAINER_MAX; x++)
    {
        if (space[x])
        {
            if ((Container == -1) || (Container == x))
                space[x] = GetAvailableSpace(x);
            else
                space[x] = 0;
            if (space[x])
                spaceFound = true;
        }
    }
    if (!spaceFound)
    {
        pOutput->error("No available space to store items.");
        return;
    }

    std::list<ItemData_t> items = GetMatchingItems(Args[2], 0);
    if (items.size() == 0)
    {
        pOutput->error_f("No matching items found.  Term: %s", Args[2].c_str());
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

        pk_MoveItem packet;
        for (int x = 0; x < 13; x++)
        {
            if ((space[x] > 0) && (CanStoreItem(x, *iter)))
            {
                packet.ToStorage = x;
                space[x]--;
            }
        }

        //Skip item that has no place to be stored.
        if (packet.ToStorage == 0)
            continue;

        packet.Quantity    = iter->Item->Count;
        packet.FromIndex   = iter->Item->Index;
        m_AshitaCore->GetPacketManager()->AddOutgoingPacket(0x029, sizeof(pk_MoveItem), (uint8_t*)&packet);

        if (packet.Quantity == 1)
            pOutput->message_f("Storing a $H%s$R in $H%s$R.", iter->Resource->LogNameSingular[0], ContainerNames[packet.ToStorage]);
        else
            pOutput->message_f("Storing $H%u %s$R in $H%s$R.", packet.Quantity, iter->Resource->LogNamePlural[0], ContainerNames[packet.ToStorage]);
        return;
    }
    
    pOutput->error("None of the available storage containers can hold a matching item.");
}
void Bellhop::PutAll(vector<string> Args, int ArgCount, CommandHelp HelpText)
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
            pOutput->error_f("Invalid container specified.  You input: %s", Args[3].c_str());
            return;
        }
    }

    //Populate a list of which bags have space in them and are currently available.
    uint8_t space[CONTAINER_MAX];
    CheckContainers(space);
    bool spaceFound = false;
    for (int x = 0; x < CONTAINER_MAX; x++)
    {
        if (space[x])
        {
            if ((Container == -1) || (Container == x))
                space[x] = GetAvailableSpace(x);
            else
                space[x] = 0;
            if (space[x])
                spaceFound = true;
        }
    }
    if (!spaceFound)
    {
        pOutput->error("No available space to store items.");
        return;
    }

    std::list<ItemData_t> items = GetMatchingItems(Args[2], 0);
    if (items.size() == 0)
    {
        pOutput->error_f("No matching items found.  Term: %s", Args[2].c_str());
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

        pk_MoveItem packet;
        for (int x = 0; x < 13; x++)
        {
            if ((space[x] > 0) && (CanStoreItem(x, *iter)))
            {
                packet.ToStorage = x;
                space[x]--;
            }
        }

        //Skip item that has no place to be stored.
        if (packet.ToStorage == 0)
            continue;

        packet.FromStorage = 0;
        packet.Quantity    = iter->Item->Count;
        packet.FromIndex   = iter->Item->Index;
        m_AshitaCore->GetPacketManager()->AddOutgoingPacket(0x029, sizeof(pk_MoveItem), (uint8_t*)&packet);

        StoreResult(&results, ItemActionInfo(iter->Resource, packet.Quantity, packet.ToStorage));
        resultCount += packet.Quantity;
    }

    if (resultCount == 0)
    {
        pOutput->error("None of the available storage containers can hold a matching item.");
        return;
    }
    else if ((results.size() > 1) && (mSettings.ShortOutput))
    {
        pOutput->message_f("Storing $H%u$R matching items.", resultCount);
    }
    else
    {
        for (std::list<ItemActionInfo>::iterator iter = results.begin(); iter != results.end(); iter++)
        {
            if (iter->Count == 1)
                pOutput->message_f("Storing a $H%s$R in $H%s$R.", iter->Resource->LogNameSingular[0], ContainerNames[iter->Location]);
            else
                pOutput->message_f("Storing $H%u %s$R in $H%s$R.", iter->Count, iter->Resource->LogNamePlural[0], ContainerNames[iter->Location]);
        }
    }
}