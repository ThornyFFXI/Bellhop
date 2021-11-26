#include "Bellhop.h"

void Bellhop::Sort(vector<string> Args, int ArgCount, CommandHelp HelpText)
{
    if (ArgCount < 3)
    {
        PrintHelpText(HelpText, false);
        return;
    }

    int Container = GetContainerIndexFromName(Args[2].c_str());
    if (Container == -1)
    {
        OutputHelper::Outputf(Ashita::LogLevel::Error, "Invalid container specified.  You input: %s", Args[2].c_str());
        return;
    }

    pk_SortInventory packet;
    packet.Container = Container;
    m_AshitaCore->GetPacketManager()->AddOutgoingPacket(0x3A, sizeof(pk_SortInventory), (uint8_t*)&packet);
    OutputHelper::Outputf(Ashita::LogLevel::Info, "Sorting $H%s$R.", ContainerNames[Container]);
}