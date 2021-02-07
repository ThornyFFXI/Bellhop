#include "Bellhop.h"

void Bellhop::TradeToPlayer(vector<string> Args, int ArgCount, CommandHelp HelpText)
{
    if (ArgCount < 3)
    {
        PrintHelpText(HelpText, false);
        return;
    }

    if ((mSettings.EnforceTradeWindow) && ((mState.TradeState != TradeState::TradeOpen) && (mState.TradeState != TradeState::TradeConfirmedByOther)))
    {
        pOutput->error("You must have trade open to use this command.");
        return;
    }

    std::list<pk_AddItemToPlayerTrade> pendingPackets;
    std::list<int> indicesUsed;
    uint32_t totalCount = 0;

    for (int x = 2; x < ArgCount; x += 2)
    {
        int quantity = 1;
        if ((x + 1) < ArgCount)
            quantity = atoi(Args[x + 1].c_str());
        if ((quantity < 1) || (quantity > 792))
        {
            pOutput->error_f("Quantity must be between 1 and 792.  You input quantity of $H%s$R for the item $H%s$R.", Args[x + 1].c_str(), Args[x].c_str());
            return;
        }

        std::list<ItemData_t> items = GetMatchingItems(Args[x], 0);
        items.sort([quantity](const ItemData_t a, const ItemData_t b) {
            //If we have an exact match, that always wins.
            if ((a.Item->Count == quantity) != (b.Item->Count == quantity))
                return (a.Item->Count == quantity);

            //When dealing with less than a stack, pick the cleanest moves to maintain inventory better.
            if ((a.Resource->StackSize == b.Resource->StackSize) && (quantity < a.Resource->StackSize))
            {
                //If only one of them is enough of the item, prefer that one.
                if ((a.Item->Count > quantity) != (b.Item->Count > quantity))
                {
                    return (a.Item->Count > b.Item->Count);
                }

                //If both have enough, prefer the smaller partial stack.
                if (a.Item->Count > quantity)
                    return (a.Item->Count < b.Item->Count);

                //If neither have enough, proceed to higher quantity check.
            }

            //Sort by highest quantity of item.
            return (a.Item->Count > b.Item->Count);
        });

        int itemsFound = 0;
        while ((items.size() > 0) && (itemsFound < quantity) && (pendingPackets.size() < 8))
        {
            std::list<ItemData_t>::iterator iter = items.begin();

            //Skip EX item.
            if (iter->Resource->Flags & 0x4000)
            {
                items.erase(iter);
                continue;
            }

            //Skip equipped gear, bazaared item, equipped linkshell.
            if ((iter->Item->Flags == 5) || (iter->Item->Flags == 19) || (iter->Item->Flags == 25))
            {
                items.erase(iter);
                continue;
            }

            //Since we make a pass for each pair of arguments, make sure we don't reuse indices.
            if (std::find(indicesUsed.begin(), indicesUsed.end(), iter->Item->Index) != indicesUsed.end())
            {
                items.erase(iter);
                continue;
            }

            pk_AddItemToPlayerTrade packet;
            packet.Id       = iter->Item->Id;
            packet.Index    = iter->Item->Index;
            packet.Quantity = min(quantity - itemsFound, iter->Item->Count);
            packet.Slot     = pendingPackets.size() + 1;

            itemsFound += packet.Quantity;
            totalCount += packet.Quantity;
            indicesUsed.push_back(iter->Item->Index);

            pendingPackets.push_back(packet);
            items.erase(iter);
        }

        if (itemsFound < quantity)
        {
            if (pendingPackets.size() == 8)
            {
                pOutput->error("Requested trade could not be fit in 8 trade slots.");
                return;
            }
            else
            {
                pOutput->error_f("Only found $H%d/%d$R items matching the term $H%s$R.", itemsFound, quantity, Args[x].c_str());
                return;
            }
        }
    }

    for (std::list<pk_AddItemToPlayerTrade>::iterator iter = pendingPackets.begin(); iter != pendingPackets.end(); iter++)
    {
        pk_AddItemToPlayerTrade packet = *iter;
        m_AshitaCore->GetPacketManager()->AddOutgoingPacket(0x34, sizeof(pk_AddItemToPlayerTrade), (uint8_t*)&packet);
    }

    pk_TradePlayer packet;
    packet.Type      = 2;
    m_AshitaCore->GetPacketManager()->AddOutgoingPacket(0x33, 12, (uint8_t*)(&packet));
    pOutput->message_f("Traded $H%d$R matching items to $H%s$R.", totalCount, mState.TradeName.c_str());
}
void Bellhop::TradeAllToPlayer(vector<string> Args, int ArgCount, CommandHelp HelpText)
{
    if (ArgCount < 3)
    {
        PrintHelpText(HelpText, false);
        return;
    }

    if ((mSettings.EnforceTradeWindow) && ((mState.TradeState != TradeState::TradeOpen) && (mState.TradeState != TradeState::TradeConfirmedByOther)))
    {
        pOutput->error("You must have trade open to use this command.");
        return;
    }

    std::list<ItemData_t> items;
    for (int x = 2; x < ArgCount; x++)
    {
        items.splice(items.end(), GetMatchingItems(Args[x], 0));
    }
    if (items.size() == 0)
    {
        pOutput->error("No matching items found.");
        return;
    }

    items.sort([](const ItemData_t a, const ItemData_t b) {
        //Sort by highest quantity of item.
        return (a.Item->Count > b.Item->Count);
    });

    uint32_t tradeCount = 0;
    uint32_t slotCount  = 0;
    while ((items.size() > 0) && (slotCount < 8))
    {
        std::list<ItemData_t>::iterator iter = items.begin();

        //Skip EX item.
        if (iter->Resource->Flags & 0x4000)
        {
            items.erase(iter);
            continue;
        }

        //Skip equipped gear, bazaared item, equipped linkshell.
        if ((iter->Item->Flags == 5) || (iter->Item->Flags == 19) || (iter->Item->Flags == 25))
        {
            items.erase(iter);
            continue;
        }

        pk_AddItemToPlayerTrade packet;
        packet.Id = iter->Item->Id;
        packet.Index = iter->Item->Index;
        packet.Quantity = iter->Item->Count;
        slotCount++; //Not zero indexed, 0 slot reserved for gil..
        tradeCount += packet.Quantity;
        packet.Slot     = slotCount;
        m_AshitaCore->GetPacketManager()->AddOutgoingPacket(0x34, sizeof(pk_AddItemToPlayerTrade), (uint8_t*)&packet);
        items.erase(iter);
    }

    if (tradeCount == 0)
    {
        pOutput->error("No matching items could be traded.");
        return;
    }

    pk_TradePlayer packet;
    packet.Type       = 2;
    m_AshitaCore->GetPacketManager()->AddOutgoingPacket(0x33, 12, (uint8_t*)(&packet));
    pOutput->message_f("Traded $H%d$R matching items to $H%s$R.", tradeCount, mState.TradeName.c_str());
}