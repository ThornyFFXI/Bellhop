#include "Bellhop.h"

bool Bellhop::HandleIncomingPacket(uint16_t id, uint32_t size, const uint8_t* data, uint8_t* modified, uint32_t sizeChunk, const uint8_t* dataChunk, bool injected, bool blocked)
{
    UNREFERENCED_PARAMETER(modified);
    UNREFERENCED_PARAMETER(sizeChunk);
    UNREFERENCED_PARAMETER(dataChunk);
    UNREFERENCED_PARAMETER(injected);
    UNREFERENCED_PARAMETER(blocked);

    if (id == 0x0A)
        HandleIncomingPacket_00A(size, data);
    else if (id == 0x0B)
        HandleIncomingPacket_00B(size, data);
    else if (id == 0x21)
        HandleIncomingPacket_021(size, data);
    else if (id == 0x22)
        HandleIncomingPacket_022(size, data);
    else if (id == 0x23)
        HandleIncomingPacket_023(size, data);
    else if (id == 0x3C)
        HandleIncomingPacket_03C(size, data);

    return false;
}
bool Bellhop::HandleOutgoingPacket(uint16_t id, uint32_t size, const uint8_t* data, uint8_t* modified, uint32_t sizeChunk, const uint8_t* dataChunk, bool injected, bool blocked)
{
    UNREFERENCED_PARAMETER(modified);
    UNREFERENCED_PARAMETER(sizeChunk);
    UNREFERENCED_PARAMETER(dataChunk);
    UNREFERENCED_PARAMETER(injected);
    UNREFERENCED_PARAMETER(blocked);

    if (id == 0x15)
        HandleOutgoingPacket_015(size, data);
    else if (id == 0x1A)
        HandleOutgoingPacket_01A(size, data);
    else if (id == 0x109)
        HandleOutgoingPacket_109(size, data);
    else if (id == 0x10B)
        HandleOutgoingPacket_10B(size, data);

    return false;
}
void Bellhop::HandleIncomingPacket_00A(uint32_t size, const uint8_t* data)
{
    //Server told us we're zoning in, so record char name and reset states.
    std::string currentName((const char*)data + 0x84);
    if (currentName != mState.MyName)
    {
        mState.MyName = currentName;
        LoadSettings();
    }
    mState.MyId = Read32(data, 4);
    mState.TradeState = TradeState::TradeClosed;
    mState.BazaarOpen = false;
    mState.ShopActive = false;
    mState.ShopStock.clear();
}
void Bellhop::HandleIncomingPacket_00B(uint32_t size, const uint8_t* data)
{
    //Server told us we're zoning out, so reset states.
    mState.TradeState = TradeState::TradeClosed;
    mState.BazaarOpen = false;
    mState.ShopActive = false;
    mState.ShopStock.clear();
}
void Bellhop::HandleIncomingPacket_021(uint32_t size, const uint8_t* data)
{
    if (mState.TradeState != TradeState::IncomingOffer)
    {
        mState.TradeState = TradeState::IncomingOffer;
        mState.TradeDelay = std::chrono::steady_clock::now();

        uint16_t playerIndex = Read16(data, 0x08);
        if (m_AshitaCore->GetMemoryManager()->GetEntity()->GetRawEntity(playerIndex))
            mState.TradeName = std::string(m_AshitaCore->GetMemoryManager()->GetEntity()->GetName(playerIndex));
        else
            mState.TradeName = "";
    }
}
void Bellhop::HandleIncomingPacket_022(uint32_t size, const uint8_t* data)
{
    uint8_t type = Read8(data, 8);
    uint16_t playerIndex = Read16(data, 0x0C);

    if (type == 0)
    {
        if (mState.TradeState != TradeState::TradeOpen)
        {        
            mState.TradeState = TradeState::TradeOpen;
            mState.TradeDelay = std::chrono::steady_clock::now();

            if (m_AshitaCore->GetMemoryManager()->GetEntity()->GetRawEntity(playerIndex))
                mState.TradeName = std::string(m_AshitaCore->GetMemoryManager()->GetEntity()->GetName(playerIndex));
            else
                mState.TradeName = "";
        }
    }

    else if (type == 1)
    {
        mState.TradeState = TradeState::TradeClosed;
    }

    else if (type == 2)
    {
        if (mState.TradeState != TradeState::TradeConfirmedByOther)
        {
            mState.TradeState = TradeState::TradeConfirmedByOther;
            mState.TradeDelay = std::chrono::steady_clock::now();

            if (m_AshitaCore->GetMemoryManager()->GetEntity()->GetRawEntity(playerIndex))
                mState.TradeName = std::string(m_AshitaCore->GetMemoryManager()->GetEntity()->GetName(playerIndex));
            else
                mState.TradeName = "";
        }
    }

    else if (type == 9)
    {
        mState.TradeState = TradeState::TradeClosed;
    }
}
void Bellhop::HandleIncomingPacket_023(uint32_t size, const uint8_t* data)
{
    mState.TradeState = TradeState::TradeOpen;
    mState.TradeSync  = Read16(data, 0x08);
}
void Bellhop::HandleIncomingPacket_03C(uint32_t size, const uint8_t* data)
{
    if (!mState.ShopActive)
    {
        mState.ShopStock.clear();
        mState.ShopActive = true;
        mState.ShopName   = m_AshitaCore->GetMemoryManager()->GetEntity()->GetName(mState.ShopIndex);
        pOutput->message_f("Entered shop.  [$H%s$R]", mState.ShopName.c_str());
    }

    for (int x = 8; (x + 7) < size; x += 12)
    {
        uint16_t slot = Read16(data, x + 6);
        ShopItem_t entry;
        entry.Enabled = true;
        entry.Price   = Read32(data, x);
        entry.Resource = m_AshitaCore->GetResourceManager()->GetItemById(Read16(data, x + 4));

        if (!mSettings.IgnoreCraftSkill)
        {
            uint16_t craftSkill = Read16(data, x + 8);
            if (craftSkill != 0)
            {
                uint16_t compareSkill = Read16(data, x + 10);
                if (compareSkill != 0)
                {
                    /*
                    The NPC seems to return values in hundreds.  200 = recruit, 300 = initiate, 400 = novice, 500 = apprentice, 600 = journeyman, 700 = craftsman.
                    Since the client uses 0-indexed rank, we modify this value to match  (0=amateur,1=recruit,2=initiate,3=novice,4=apprentice,5=journeyman,6=craftsman.
                    If the item has a skill of 0, we don't need to do this because it's already 0.
                    */
                    compareSkill = (compareSkill / 100) - 1;
                }
                uint16_t mySkill = m_AshitaCore->GetMemoryManager()->GetPlayer()->GetCraftSkill(craftSkill - 48).GetRank();
                if (mySkill < compareSkill)
                    entry.Enabled = false;
            }
        }

        mState.ShopStock[slot] = entry;    
    }
}
void Bellhop::HandleOutgoingPacket_015(uint32_t size, const uint8_t* data)
{
    //We handle autotrade on outgoing 0x15, because it's the time when we have the most accurate data(anything we inject is IMMEDIATELY going out).
    HandleAutoTrade();

    //We track shop data here because it's when we tell the server we moved, until the 0x15 goes out the server has no idea where we are so it doesn't matter.
    if (mState.ShopActive)
    {
        if ((m_AshitaCore->GetMemoryManager()->GetEntity()->GetRawEntity(mState.ShopIndex) == NULL) || ((m_AshitaCore->GetMemoryManager()->GetEntity()->GetRenderFlags0(mState.ShopIndex) & 0x200) == 0))
        {
            mState.ShopActive = false;
            pOutput->message_f("Left shop.  [$H%s$R]", mState.ShopName.c_str());
            mState.ShopStock.clear();
            return;
        }

        double Distance = pow(ReadFloat(data, 0x04) - m_AshitaCore->GetMemoryManager()->GetEntity()->GetLocalPositionX(mState.ShopIndex), 2) + pow(ReadFloat(data, 0x0C) - m_AshitaCore->GetMemoryManager()->GetEntity()->GetLocalPositionY(mState.ShopIndex), 2);
        if (Distance >= 36.0f)
        {
            mState.ShopActive = false;
            pOutput->message_f("Left shop.  [$H%s$R]", mState.ShopName.c_str());
            mState.ShopStock.clear();
            return;
        }
    }
}
void Bellhop::HandleOutgoingPacket_01A(uint32_t size, const uint8_t* data)
{
    if (Read32(data, 0x0A) == 0) //Talk packet
    {
        //We clear shop when we talk to someone else, because that tells the server we are no longer in the shop.
        if (mState.ShopActive)
        {
            mState.ShopActive = false;
            pOutput->message_f("Left shop.  [$H%s$R]", mState.ShopName.c_str());
            mState.ShopStock.clear();
        }

        //And we save the index of the last NPC we talked to, so if a shop menu opens we know who spawned it.
        mState.ShopIndex = Read16(data, 0x08);
    }
}
void Bellhop::HandleOutgoingPacket_109(uint32_t size, const uint8_t* data)
{
    mState.BazaarOpen = false;
}
void Bellhop::HandleOutgoingPacket_10B(uint32_t size, const uint8_t* data)
{
    mState.BazaarOpen = true;
}