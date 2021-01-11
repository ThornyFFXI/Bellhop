#include "Bellhop.h"

void Bellhop::Bazaar(vector<string> Args, int ArgCount, CommandHelp HelpText)
{
    if (ArgCount < 4)
    {
        PrintHelpText(HelpText, false);
        return;
    }

    if (!mState.BazaarOpen)
    {
        pOutput->error("Bazaar menu must be open.");
        return;
    }

    int32_t price = atoi(Args[3].c_str());
    if ((price < 0) || (price > 99999999))
    {
        pOutput->error_f("Price must be between 0g(unbazaar) and 99,999,999g.  You input: %d", price);
        return;    
    }

    std::list<ItemData_t> Items = GetMatchingItems(Args[2], 0);
    if (Items.size() == 0)
    {
        pOutput->error_f("No matching items found.  Term: %s", Args[2].c_str());
        return;
    }

    Items.sort([](const ItemData_t a, const ItemData_t b)
    {
        //If only one item has a full stack, prefer full stack (keep in mind multiple items may match query, so we prefer a full stack over quantity if any exist).
        if ((a.Item->Count == a.Resource->StackSize) != (b.Item->Count == b.Resource->StackSize))
        {
            return (a.Item->Count == a.Resource->StackSize);
        }

        //Otherwise, take the bigger count.
        return (a.Item->Count > b.Item->Count);
    });

    for (std::list<ItemData_t>::iterator iter = Items.begin(); iter != Items.end(); iter++)
    {
        //Skip EX item.
        if ((iter->Resource->Flags & 0x4000) == 0)
            continue;

        //Skip augmented item. (Resource will not be set as non-bazaarable as it is bazaarable until augmented.)
        if ((iter->Item->Extra[0] == 2) || (iter->Item->Extra[0] == 3))
            continue;

        //Skip equipped item.
        if (iter->Item->Flags == 5)
            continue;

        //Send bazaar packet.
        pk_SetBazaar packet;
        packet.Price        = price;
        packet.Index        = iter->Item->Index;
        m_AshitaCore->GetPacketManager()->AddOutgoingPacket(0x10A, sizeof(pk_SetBazaar), (uint8_t*)(&packet));


        if (price == 0)
        {
            if (iter->Item->Count == 1)
                pOutput->message_f("Unbazaaring a $H%s$R.", iter->Resource->LogNameSingular[0]);
            else
                pOutput->message_f("Unbazaaring $H%d %s$R.", iter->Item->Count, iter->Resource->LogNamePlural[0]);
        }
        else if (iter->Item->Count == 1)
            pOutput->message_f("Bazaaring a $H%s$R for $H%d$Rg.", iter->Resource->LogNameSingular[0], price);
        else
            pOutput->message_f("Bazaaring $H%d %s$R for $H%d$Rg.", iter->Item->Count, iter->Resource->LogNamePlural[0], price);
        return;
    }

    pOutput->error_f("No matching items found.  Term: %s", Args[2].c_str());
    return;
}
void Bellhop::BazaarAll(vector<string> Args, int ArgCount, CommandHelp HelpText)
{
    if (ArgCount < 4)
    {
        PrintHelpText(HelpText, false);
        return;
    }

    if (!mState.BazaarOpen)
    {
        pOutput->error("Bazaar menu must be open.");
        return;
    }

    int32_t price = atoi(Args[3].c_str());
    if ((price < 0) || (price > 99999999))
    {
        pOutput->error_f("Price must be between 0g(unbazaar) and 99,999,999g.  You input: %d", price);
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
        //Skip EX item.
        if (iter->Resource->Flags & 0x4000)
            continue;

        //Skip augmented item. (Resource will not be set as non-bazaarable as it is bazaarable until augmented.)
        if ((iter->Item->Extra[0] == 2) || (iter->Item->Extra[0] == 3))
            continue;

        //Skip equipped item.
        if (iter->Item->Flags == 5)
            continue;

        //Send bazaar packet.
        pk_SetBazaar packet;
        packet.Price        = price;
        packet.Index        = iter->Item->Index;
        m_AshitaCore->GetPacketManager()->AddOutgoingPacket(0x10A, sizeof(pk_SetBazaar), (uint8_t*)(&packet));

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
        if (price == 0)
            pOutput->message_f("Unbazaaring $H%u matching items.", resultCount);
        else
            pOutput->message_f("Bazaaring $H%u$R matching items for $H%d$Rg.", resultCount, price);    
    }
    else
    {
        for (std::list<ItemActionInfo>::iterator iter = results.begin(); iter != results.end(); iter++)
        {
            if (price == 0)
            {
                if (iter->Count == 1)
                    pOutput->message_f("Unbazaaring a $H%s$R.", iter->Resource->LogNameSingular[0]);
                else
                    pOutput->message_f("Unbazaaring $H%u %s$R.", iter->Count, iter->Resource->LogNamePlural[0]);
            }
            else if (iter->Count == 1)
                pOutput->message_f("Bazaaring a $H%s$R for $H%d$Rg.", iter->Resource->LogNameSingular[0], price);
            else
                pOutput->message_f("Bazaaring $H%u %s$R for $H%d$Rg.", iter->Count, iter->Resource->LogNamePlural[0], price);
        }
    }
}