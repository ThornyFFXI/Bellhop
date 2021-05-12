#include "Bellhop.h"

void Bellhop::TradeToNpc(vector<string> Args, int ArgCount, CommandHelp HelpText)
{
    if (ArgCount < 3)
    {
        PrintHelpText(HelpText, false);
        return;
    }

    uint16_t targetIndex = m_AshitaCore->GetMemoryManager()->GetTarget()->GetTargetIndex(m_AshitaCore->GetMemoryManager()->GetTarget()->GetIsSubTargetActive());
    if ((m_AshitaCore->GetMemoryManager()->GetEntity()->GetRawEntity(targetIndex) == NULL) || (targetIndex >= 0x400) || ((m_AshitaCore->GetMemoryManager()->GetEntity()->GetRenderFlags0(targetIndex) & 0x200) == 0))
    {
        pOutput->error("You must have a valid NPC targeted.");
        return;
    }

    if (m_AshitaCore->GetMemoryManager()->GetEntity()->GetDistance(targetIndex) > 35.9f)
    {
        pOutput->error("NPC is too far away.");
        return;
    }

    pk_TradeToNpc packet;
    packet.TargetIndex = targetIndex;
    packet.TargetId    = m_AshitaCore->GetMemoryManager()->GetEntity()->GetServerId(targetIndex);

    std::list<int> indicesUsed;
    uint32_t totalCount = 0;

    for (int x = 2; x < ArgCount; x += 2)
    {
        int quantity                = 1;
        if ((x + 1) < ArgCount)
            quantity = atoi(Args[x + 1].c_str());

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
        while ((items.size() > 0) && (itemsFound < quantity) && (packet.TotalItems < 8))
        {
            std::list<ItemData_t>::iterator iter = items.begin();

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

            packet.ItemIndex[packet.TotalItems] = iter->Item->Index;
            packet.ItemQuantity[packet.TotalItems] =  min(quantity - itemsFound, iter->Item->Count);

            itemsFound += packet.ItemQuantity[packet.TotalItems];
            totalCount += packet.ItemQuantity[packet.TotalItems];

            packet.TotalItems++;
            indicesUsed.push_back(iter->Item->Index);
            items.erase(iter);
        }

        if (itemsFound < quantity)
        {
            if (packet.TotalItems == 8)
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

    packet.ArrangeGil();
    m_AshitaCore->GetPacketManager()->AddOutgoingPacket(0x36, sizeof(pk_TradeToNpc), (uint8_t*)&packet);
    pOutput->message_f("Traded $H%d$R matching items to $H%s$R.", totalCount, m_AshitaCore->GetMemoryManager()->GetEntity()->GetName(packet.TargetIndex));
}
void Bellhop::TradeAllToNpc(vector<string> Args, int ArgCount, CommandHelp HelpText)
{
    if (ArgCount < 3)
    {
        PrintHelpText(HelpText, false);
        return;
    }

    uint16_t targetIndex     = m_AshitaCore->GetMemoryManager()->GetTarget()->GetTargetIndex(m_AshitaCore->GetMemoryManager()->GetTarget()->GetIsSubTargetActive());
    if ((m_AshitaCore->GetMemoryManager()->GetEntity()->GetRawEntity(targetIndex) == NULL) || (targetIndex >= 0x400) || ((m_AshitaCore->GetMemoryManager()->GetEntity()->GetRenderFlags0(targetIndex) & 0x200) == 0))
    {
        pOutput->error("You must have a valid NPC targeted.");
        return;
    }

    if (m_AshitaCore->GetMemoryManager()->GetEntity()->GetDistance(targetIndex) > 35.9f)
    {
        pOutput->error("NPC is too far away.");
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

    pk_TradeToNpc packet;
    packet.TargetIndex  = targetIndex;
    packet.TargetId     = m_AshitaCore->GetMemoryManager()->GetEntity()->GetServerId(targetIndex);
    uint32_t tradeCount = 0;
    while ((items.size() > 0) && (packet.TotalItems < 8))
    {
        std::list<ItemData_t>::iterator iter = items.begin();

        //Skip equipped gear, bazaared item, equipped linkshell.
        if ((iter->Item->Flags == 5) || (iter->Item->Flags == 19) || (iter->Item->Flags == 25))
        {
            items.erase(iter);
            continue;
        }

        packet.ItemIndex[packet.TotalItems] = iter->Item->Index;
        packet.ItemQuantity[packet.TotalItems] = iter->Item->Count;
        tradeCount += packet.ItemQuantity[packet.TotalItems];
        packet.TotalItems++;
        items.erase(iter);
    }

    if (tradeCount == 0)
    {
        pOutput->error("No matching items could be traded.");
        return;
    }

    packet.ArrangeGil();
    m_AshitaCore->GetPacketManager()->AddOutgoingPacket(0x36, sizeof(pk_TradeToNpc), (uint8_t*)&packet);
    pOutput->message_f("Traded $H%d$R matching items to $H%s$R.", tradeCount, m_AshitaCore->GetMemoryManager()->GetEntity()->GetName(packet.TargetIndex));
}