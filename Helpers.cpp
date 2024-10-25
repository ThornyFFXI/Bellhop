#include "Bellhop.h"

std::list<int> gEquipBags =
{8, 10, 11, 12, 13, 14, 15, 16};

std::list<ItemData_t> Bellhop::GetMatchingItemsNoInventory(std::string Parameter)
{
    uint8_t Containers[CONTAINER_MAX] = {0};
    CheckContainers(Containers);

    std::list<ItemData_t> ItemList;

    if (Parameter.substr(0, 5) == "TYPE:")
    {
        auto itemTypes                                              = mConfig.GetItemTypes();
        ItemTypeMap::iterator iter = itemTypes.find(Parameter.substr(5));
        if (iter != itemTypes.end())
        {
            for (int x = 1; x < CONTAINER_MAX; x++)
            {
                if (!Containers[x])
                    continue;
                GetMatchingItemsByType(iter->second, x, &ItemList);
            }
        }
        return ItemList;
    }

    uint16_t ItemId = StringToUint16OrZero(Parameter);
    if (ItemId != 0)
    {
        IItem* pResource = m_AshitaCore->GetResourceManager()->GetItemById(ItemId);
        if ((pResource) && (strlen(pResource->Name[0]) > 0))
        {
            for (int x = 1; x < CONTAINER_MAX; x++)
            {
                if (!Containers[x])
                    continue;
                GetMatchingItemsByResource(pResource, x, &ItemList);
            }
        }
    }
    else
    {
        for (int x = 1; x < CONTAINER_MAX; x++)
        {
            if (!Containers[x])
                continue;
            GetMatchingItemsByWildcard(Parameter, x, &ItemList);
        }
    }
    return ItemList;
}
std::list<ItemData_t> Bellhop::GetMatchingItems(std::string Parameter, int Container)
{
    std::list<ItemData_t> ItemList;

    if (_stricmp(Parameter.c_str(), "gil") == 0)
    {
        Ashita::FFXI::item_t* pItem = m_AshitaCore->GetMemoryManager()->GetInventory()->GetContainerItem(0, 0);
        if ((Container == 0) && (pItem->Count > 0))
        {
            ItemList.push_back(ItemData_t(0, m_AshitaCore->GetResourceManager()->GetItemById(65535), pItem));
        }
        return ItemList;
    }

    if (Parameter.substr(0, 5) == "TYPE:")
    {
        auto itemTypes                                            = mConfig.GetItemTypes();
        ItemTypeMap::iterator iter = itemTypes.find(Parameter.substr(5));
        if (iter != itemTypes.end())
        {
            GetMatchingItemsByType(iter->second, Container, &ItemList);
        }
        return ItemList;
    }

    uint16_t ItemId = StringToUint16OrZero(Parameter);
    if (ItemId != 0)
    {
        IItem* pResource = m_AshitaCore->GetResourceManager()->GetItemById(ItemId);
        if ((pResource) && (strlen(pResource->Name[0]) > 0))
        {
            GetMatchingItemsByResource(pResource, Container, &ItemList);
        }
    }
    else
    {
        GetMatchingItemsByWildcard(Parameter, Container, &ItemList);
    }
    return ItemList;
}
int Bellhop::GetMatchingItemsByWildcard(std::string Parameter, int Container, std::list<ItemData_t>* Output)
{
    int count = 0;
    IInventory* pInv = m_AshitaCore->GetMemoryManager()->GetInventory();
    int containerMax = pInv->GetContainerCountMax(Container);

    for (int x = 1; x <= containerMax; x++)
    {
        Ashita::FFXI::item_t* pItem = pInv->GetContainerItem(Container, x);
        if ((pItem->Id == 0) || (pItem->Count == 0))
            continue;

        IItem* pResource = m_AshitaCore->GetResourceManager()->GetItemById(pItem->Id);
        if ((!pResource) || (strlen(pResource->Name[0]) < 1))
            continue;

        if (BellhopConfig::CheckWildcardMatch(Parameter.c_str(), pResource->Name[0]))
        {
            Output->push_back(ItemData_t(Container, pResource, pItem));
        }
    }

    return count;
}
int Bellhop::GetMatchingItemsByResource(IItem* pResource, int Container, std::list<ItemData_t>* Output)
{
    int count        = 0;
    IInventory* pInv = m_AshitaCore->GetMemoryManager()->GetInventory();
    int containerMax = pInv->GetContainerCountMax(Container);

    for (int x = 1; x <= containerMax; x++)
    {
        Ashita::FFXI::item_t* pItem = pInv->GetContainerItem(Container, x);
        if ((pItem->Id != pResource->Id) || (pItem->Count == 0))
            continue;

        Output->push_back(ItemData_t(Container, pResource, pItem));
    }

    return count;
}
int Bellhop::GetMatchingItemsByType(std::vector<uint16_t> Ids, int Container, std::list<ItemData_t>* Output)
{
    int count        = 0;
    IInventory* pInv = m_AshitaCore->GetMemoryManager()->GetInventory();
    int containerMax = pInv->GetContainerCountMax(Container);

    for (int x = 1; x <= containerMax; x++)
    {
        Ashita::FFXI::item_t* pItem = pInv->GetContainerItem(Container, x);
        if ((pItem->Id == 0) || (pItem->Count == 0))
            continue;

        IItem* pResource = m_AshitaCore->GetResourceManager()->GetItemById(pItem->Id);
        if ((!pResource) || (strlen(pResource->Name[0]) < 1))
            continue;

        if (std::find(Ids.begin(), Ids.end(), pItem->Id) != Ids.end())
        {
            Output->push_back(ItemData_t(Container, pResource, pItem));
        }
    }

    return count;
}
uint8_t Bellhop::GetShopItem(std::string Parameter, ShopItem_t* buffer)
{
    for (std::map<uint8_t, ShopItem_t>::iterator iter = mState.ShopStock.begin(); iter != mState.ShopStock.end(); iter++)
    {
        if ((iter->second.Enabled) && (CheckItemMatch(Parameter, iter->second.Resource)))
        {
            *buffer = iter->second;
            return iter->first;
        }
    }
    return 0;
}
uint16_t Bellhop::StringToUint16OrZero(std::string Parameter)
{
    for (int x = 0; x < Parameter.length(); x++)
    {
        if (!isdigit(Parameter[x]))
            return 0;
    }
    return (uint16_t)atoi(Parameter.c_str());
}
bool Bellhop::CheckItemMatch(std::string Parameter, IItem* pResource)
{
    if (Parameter.substr(0, 5) == "TYPE:")
    {
        auto itemTypes                                            = mConfig.GetItemTypes();
        ItemTypeMap::iterator iter = itemTypes.find(Parameter.substr(5));
        if (iter != itemTypes.end())
        {
            for (std::vector<uint16_t>::iterator itemIter = iter->second.begin(); itemIter != iter->second.end(); itemIter++)
            {
                if (*itemIter == pResource->Id)
                    return true;
            
            }
        }
        return false;
    }

    uint16_t ItemId = StringToUint16OrZero(Parameter);
    if (ItemId != 0)
    {
        return (ItemId == pResource->Id);
    }
    else
    {
        return BellhopConfig::CheckWildcardMatch(Parameter.c_str(), pResource->Name[0]);
    }
}
void Bellhop::CheckContainers(uint8_t* buffer)
{
    bool atNomad = false;
    bool inMog   = false;

    DWORD zonepointer = Read32(pZoneFlags, 0);
    if (zonepointer != 0)
    {
        if (Read32(zonepointer, pZoneOffset) & 0x100)
        {
            inMog = true;
        }
    }

    for (int x = 0; x < 1024; x++)
    {
        if ((m_AshitaCore->GetMemoryManager()->GetEntity()->GetRawEntity(x)) && (m_AshitaCore->GetMemoryManager()->GetEntity()->GetRenderFlags0(x) & 0x200))
        {
            if ((m_AshitaCore->GetMemoryManager()->GetEntity()->GetDistance(x) < 36) && (strcmp(m_AshitaCore->GetMemoryManager()->GetEntity()->GetName(x), "Nomad Moogle") == 0))
            {
                atNomad = true;
            }
        }
    }

    DWORD Memloc = Read32(pWardrobe, 0);
    Memloc       = Read32(Memloc, 0);
    uint8_t flags = Read8(Memloc, 0xB4);

    buffer[0]  = true;                                               //Always have inventory.
    buffer[1]  = (atNomad || inMog);                                 //Safe
    buffer[2]  = (inMog || (atNomad && mConfig.GetNomadStorage())); //Storage
    buffer[3]  = false;                                              //Never have temp items.
    buffer[4]  = (atNomad || inMog);                                 //Locker
    buffer[5]  = ((Read8(Memloc, 0xB4) & 0x01) != 0);                //Satchel
    buffer[6]  = true;                                               //Sack
    buffer[7]  = true;                                               //Case
    buffer[8]  = true;                                               //Wardrobe
    buffer[9]  = (atNomad || inMog);                                 //Safe2
    buffer[10] = true;                                               //Wardrobe2
    buffer[11] = ((flags & 0x04) != 0);                //Wardrobe3
    buffer[12] = ((flags & 0x08) != 0);                //Wardrobe4
    buffer[13] = ((flags & 0x10) != 0);                //Wardrobe5
    buffer[14] = ((flags & 0x20) != 0);                //Wardrobe6
    buffer[15] = ((flags & 0x40) != 0);                //Wardrobe7
    buffer[16] = ((flags & 0x80) != 0);                //Wardrobe8

    auto enabledbags = mConfig.GetEnabledBags();
    for (auto iter = enabledbags.begin(); iter != enabledbags.end(); iter++)
    {
        buffer[*iter] = true;
    }
    
    auto disabledbags = mConfig.GetDisabledBags();
    for (auto iter = disabledbags.begin(); iter != disabledbags.end(); iter++)
    {
        buffer[*iter] = false;
    }
}
int Bellhop::GetAvailableSpace(int Container)
{
    if ((Container < 0) || (Container >= CONTAINER_MAX))
        return 0;
    return (m_AshitaCore->GetMemoryManager()->GetInventory()->GetContainerCountMax(Container) - m_AshitaCore->GetMemoryManager()->GetInventory()->GetContainerCount(Container));
}
AutoTradeSetting Bellhop::GetTradeResponse(std::string Name)
{
    auto blacklist = mConfig.GetBlacklist();
    for (auto iter = blacklist.begin(); iter != blacklist.end(); iter++)
    {
        if (*iter == Name)
            return AutoTradeSetting::Deny;
    }

    auto whitelist = mConfig.GetWhitelist();
    for (auto iter = whitelist.begin(); iter != whitelist.end(); iter++)
    {
        if (*iter == Name)
            return AutoTradeSetting::Accept;
    }

    return mConfig.GetAutoTradeSetting();
}
void Bellhop::PrintHelpText(CommandHelp help, bool Description)
{
    OutputHelper::Outputf(Ashita::LogLevel::Info, "$H%s", help.command.c_str());
    if (Description)
        OutputHelper::Output(Ashita::LogLevel::Info, help.description);
}
bool Bellhop::AddToList(std::list<string>* List, std::string Name)
{
    for (std::list<string>::iterator iter = List->begin(); iter != List->end(); iter++)
    {
        if (*iter == Name)
        {
            return false;
        }
    }
    List->push_back(Name);
    return true;
}
bool Bellhop::RemoveFromList(std::list<string>* List, std::string Name)
{
    bool foundName = false;
    for (std::list<string>::iterator iter = List->begin(); iter != List->end();)
    {
        if (*iter == Name)
        {
            foundName = true;
            iter      = List->erase(iter);
        }
        else
            iter++;
    }
    return foundName;
}
int Bellhop::GetContainerIndexFromName(const char* ContainerName)
{
    for (int x = 0; x < CONTAINER_MAX; x++)
    {
        if (_stricmp(ContainerNames[x], ContainerName) == 0)
            return x;
    }

    return -1;
}
bool Bellhop::CanStoreItem(int Container, ItemData_t item)
{
    if ((Container == 2) //No furniture in storage
        && ((item.Resource->Type == 10) || (item.Resource->Type == 11) || (item.Resource->Type == 12) || (item.Resource->Type == 14)))
    {
        return false;
    }

    if ((std::find(gEquipBags.begin(), gEquipBags.end(), Container) != gEquipBags.end()) && (!(item.Resource->Flags & 0x800)))
        return false;

    return true;
}
void Bellhop::StoreResult(std::list<ItemActionInfo>* List, ItemActionInfo Entry)
{
    for (std::list<ItemActionInfo>::iterator iter = List->begin(); iter != List->end(); iter++)
    {
        if ((iter->Resource->Id == Entry.Resource->Id) && (iter->Location == Entry.Location))
        {
            iter->Count += Entry.Count;
            return;
        }
    }
    List->push_back(Entry);
}