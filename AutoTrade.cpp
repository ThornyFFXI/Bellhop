#include "Bellhop.h"

void Bellhop::HandleAutoTrade()
{
    if (mState.TradeDelay > std::chrono::steady_clock::now())
        return;

    if (mState.TradeState == TradeState::IncomingOffer)
    {
        AutoTradeSetting response = GetTradeResponse(mState.TradeName.c_str());
        if (response == AutoTradeSetting::Accept)
        {
            //Send trade accept
            pk_TradePlayer packet;
            m_AshitaCore->GetPacketManager()->AddOutgoingPacket(0x33, 12, (uint8_t*)(&packet));
            pOutput->message_f("Sending trade accept packet. [$H%s$R]", mState.TradeName.c_str());

            //Set repeat time
            mState.TradeDelay = std::chrono::steady_clock::now() + std::chrono::milliseconds(mSettings.RetryDelay);
        }
        else if (response == AutoTradeSetting::Deny)
        {
            //Send trade deny
            pk_TradePlayer packet;
            packet.Type          = 1;
            m_AshitaCore->GetPacketManager()->AddOutgoingPacket(0x33, 12, (uint8_t*)(&packet));
            pOutput->message_f("Sending trade deny packet. [$H%s$R]", mState.TradeName.c_str());

            //Set repeat time
            mState.TradeDelay = std::chrono::steady_clock::now() + std::chrono::milliseconds(mSettings.RetryDelay);
        }
    }

    if (mState.TradeState == TradeState::TradeConfirmedByOther)
    {
        AutoTradeSetting response = GetTradeResponse(mState.TradeName.c_str());
        if (response == AutoTradeSetting::Accept)
        {
            //Send trade confirm
            pk_TradePlayer packet;
            packet.Type          = 2;
            packet.TradeCount    = mState.TradeSync;
            m_AshitaCore->GetPacketManager()->AddOutgoingPacket(0x33, 12, (uint8_t*)(&packet));
            pOutput->message_f("Sending trade confirm packet. [$H%s$R]", mState.TradeName.c_str());

            //Set repeat time
            mState.TradeDelay = std::chrono::steady_clock::now() + std::chrono::milliseconds(mSettings.RetryDelay);
        }
        else if (response == AutoTradeSetting::Deny)
        {
            //Send trade cancel
            pk_TradePlayer packet;
            packet.Type          = 1;
            m_AshitaCore->GetPacketManager()->AddOutgoingPacket(0x33, 12, (uint8_t*)(&packet));
            pOutput->message_f("Sending trade cancel packet. [$H%s$R]", mState.TradeName.c_str());

            //Set repeat time
            mState.TradeDelay = std::chrono::steady_clock::now() + std::chrono::milliseconds(mSettings.RetryDelay);
        }
    }
}