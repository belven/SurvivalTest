#include "SurvivalGameInstance.h"

USurvivalGameInstance::USurvivalGameInstance()
{
    items.Add(FItemData(GetNextItemID(), "Test", EItemType::Consumable, 10));
}

FItemData USurvivalGameInstance::GetItemByID(int32 itemID)
{
    FItemData data;
    data.ID = UItemStructs::InvalidInt;

    for(FItemData id : items)
    {
        if (id.ID == itemID)
            return id;
    }

    // ReSharper disable once CppSomeObjectMembersMightNotBeInitialized
    return data;
}

int32 USurvivalGameInstance::GetNextItemID()
{
    if(items.Num() > 0)
    {
        return items[items.Num() - 1].ID + 1;
    }

    return 0;
}
