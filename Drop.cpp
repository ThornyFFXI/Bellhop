#include "Bellhop.h"

void Bellhop::Drop(vector<string> Args, int ArgCount, CommandHelp HelpText)
{
    if (ArgCount < 3)
    {
        PrintHelpText(HelpText, false);
        return;
    }

    std::list<ItemData_t> Items = GetMatchingItems(Args[2], 0);
    if (Items.size() == 0)
    {
        pOutput->error_f("No matching items found.  Term: %s", Args[2].c_str());
        return;
    }

    Items.sort([](const ItemData_t a, const ItemData_t b) {
        //If only one item has a full stack, prefer full stack (keep in mind multiple item types may match query, so we prefer a full stack over quantity if any exist).
        if ((a.Item->Count == a.Resource->StackSize) != (b.Item->Count == b.Resource->StackSize))
        {
            return (a.Item->Count == a.Resource->StackSize);
        }

        //Otherwise, take the bigger count.
        return (a.Item->Count > b.Item->Count);
    });

    for (std::list<ItemData_t>::iterator iter = Items.begin(); iter != Items.end(); iter++)
    {
        //Skip gil
        if (iter->Item->Id == 65535)
            continue;

        //Skip equipped gear, bazaared item, equipped linkshell.
        if ((iter->Item->Flags == 5) || (iter->Item->Flags == 19) || (iter->Item->Flags == 25))
            continue;

        pk_DropItem packet;
        packet.Quantity    = iter->Item->Count;
        packet.Index       = iter->Item->Index;
        m_AshitaCore->GetPacketManager()->AddOutgoingPacket(0x28, sizeof(pk_DropItem), (uint8_t*)&packet);

        if (iter->Item->Count == 1)
            pOutput->message_f("Dropping a $H%s$R.", iter->Resource->LogNameSingular[0]);
        else
            pOutput->message_f("Dropping $H%d %s$R.", iter->Item->Count, iter->Resource->LogNamePlural[0]);
        return;
    }

    pOutput->error_f("No matching items found.  Term: %s", Args[2].c_str());
    return;
}
void Bellhop::DropAll(vector<string> Args, int ArgCount, CommandHelp HelpText)
{
    if (ArgCount < 3)
    {
        PrintHelpText(HelpText, false);
        return;
    }

    std::list<ItemData_t> Items = GetMatchingItems(Args[2], 0);
    if (Items.size() == 0)
    {
        pOutput->error_f("No matching items found.  Term: %s", Args[2].c_str());
        return;
    }

    std::list<ItemActionInfo> results;
    uint32_t resultCount = 0;

    for (std::list<ItemData_t>::iterator iter = Items.begin(); iter != Items.end(); iter++)
    {
        //Skip equipped gear, bazaared item, equipped linkshell.
        if ((iter->Item->Flags == 5) || (iter->Item->Flags == 19) || (iter->Item->Flags == 25))
            continue;

        pk_DropItem packet;
        packet.Quantity = iter->Item->Count;
        packet.Index    = iter->Item->Index;
        m_AshitaCore->GetPacketManager()->AddOutgoingPacket(0x28, sizeof(pk_DropItem), (uint8_t*)&packet);

        StoreResult(&results, ItemActionInfo(iter->Resource, iter->Item->Count));
        resultCount += iter->Item->Count;
    }

    if (resultCount == 0)
    {
        pOutput->error_f("No matching items found.  Term: %s", Args[2].c_str());
        return;
    }

    if ((results.size() > 1) && (mSettings.ShortOutput))
    {
        pOutput->message_f("Dropping $H%u$R matching items.", resultCount);
    }
    else
    {
        for (std::list<ItemActionInfo>::iterator iter = results.begin(); iter != results.end(); iter++)
        {
            if (iter->Count == 1)
                pOutput->message_f("Dropping a $H%s$R.", iter->Resource->LogNameSingular[0]);
            else
                pOutput->message_f("Dropping $H%u %s$R.", iter->Count, iter->Resource->LogNamePlural[0]);
        }
    }
}