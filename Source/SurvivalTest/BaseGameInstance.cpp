#include "BaseGameInstance.h"
#include "FactionManager.h"
#include "Events/RPGEventManager.h"
#include "Items/ItemContainer.h"
#include "Tables/ArmourDataTable.h"
#include "Tables/ConsumableTableData.h"
#include "Tables/ContainerTableData.h"
#include "Tables/Items/ItemDataTable.h"
#include "Tables/LoadoutTableData.h"
#include "Tables/Weapons/MeleeWeaponDataTable.h"
#include "Tables/Weapons/ProjectileWeaponDataTable.h"
#include "Tables/Weapons/RangedWeaponDataTable.h"
#include "Tables/TableManager.h"
#include "Tables/Weapons/WeaponDataTable.h"

#define mTable() GetTableManager()

void UBaseGameInstance::Init()
{
	Super::Init();
	mTable()->LoadTableData();
}

void UBaseGameInstance::Shutdown()
{
	mTable()->SaveTableToFile(mTable()->GetInstanceItemDataTable());
	Super::Shutdown();
}

int32 UBaseGameInstance::GetNextInstanceItemDataID()
{
	int32 instanceItemDataID = 0;
	if (GetInstancedItems().Num() > 0)
	{
		instanceItemDataID = GetInstancedItems()[GetInstancedItems().Num() - 1].ID + 1;
	}
	return instanceItemDataID;
}

int32 UBaseGameInstance::GetNextBoxID()
{
	int32 boxID = 0;
	if (GetInstancedBoxes().Num() > 0)
	{
		boxID = GetInstancedBoxes()[GetInstancedBoxes().Num() - 1].boxID + 1;
	}
	return boxID;
}

int32 UBaseGameInstance::GetNextInstanceBoxDataID()
{
	int32 instanceBoxDataID = 0;
	if (GetInstancedBoxes().Num() > 0)
	{
		instanceBoxDataID = GetInstancedBoxes()[GetInstancedBoxes().Num() - 1].ID + 1;
	}
	return instanceBoxDataID;
}

int32 UBaseGameInstance::GetNextInstanceArmourDataID()
{
	int32 instanceArmourDataID = 0;
	if (GetInstancedArmour().Num() > 0)
	{
		instanceArmourDataID = GetInstancedArmour().FindChecked(GetInstancedArmour().Num() - 1).ID + 1;
	}
	return instanceArmourDataID;
}

int32 UBaseGameInstance::GetNextInstanceContainerDataID()
{
	int32 instanceContainerDataID = 0;
	if (GetInstancedContainers().Num() > 0)
	{
		instanceContainerDataID = GetInstancedContainers()[GetInstancedContainers().Num() - 1].ID + 1;
	}
	return instanceContainerDataID;
}

TArray<FInstanceItemData> UBaseGameInstance::GetInstancedItemsForContainer(int32 instanceContainerID)
{
	TArray<FInstanceItemData> data;
	for (TTuple<int32, FInstanceItemData>& iid : GetInstancedItems())
	{
		if (iid.Value.containerInstanceID == instanceContainerID)
		{
			data.Add(iid.Value);
		}
	}
	return data;
}

FItemData UBaseGameInstance::GetItemData(int32 itemID)
{
	TArray<FItemData> itemData = mTable()->GetItemDataTable()->GetData();
	for (const FItemData id : itemData)
	{
		if (id.ID == itemID)
		{
			return id;
		}
	}
	return {};
}

FWeaponData UBaseGameInstance::GetWeaponData(int32 itemID)
{
	TArray<FWeaponData> weaponData = mTable()->GetWeaponDataTable()->GetData();
	for (const FWeaponData wd : weaponData)
	{
		if (wd.itemID == itemID)
		{
			return wd;
		}
	}
	return {};
}

FConsumableData UBaseGameInstance::GetConsumableData(int32 itemID)
{
	for (const auto& cd : mTable()->GetConsumableData()->GetData())
	{
		if (cd.Value.itemID == itemID)
		{
			return cd.Value;
		}
	}
	return {};
}

FMeleeWeaponData UBaseGameInstance::GetMeleeWeaponData(int32 weaponID)
{
	for (const FMeleeWeaponData wd : mTable()->GetMeleeWeaponData()->GetData())
	{
		if (wd.weaponID == weaponID)
		{
			return wd;
		}
	}
	return {};
}

FRangedWeaponData UBaseGameInstance::GetRangedWeaponData(int32 weaponID)
{
	for (const FRangedWeaponData wd : mTable()->GetRangedWeaponData()->GetData())
	{
		if (wd.weaponID == weaponID)
		{
			return wd;
		}
	}
	return {};
}

FProjectileWeaponData UBaseGameInstance::GetProjectileWeaponData(int32 rangedWeaponID)
{
	for (const FProjectileWeaponData wd : mTable()->GetProjectileWeaponData()->GetData())
	{
		if (wd.rangedWeaponID == rangedWeaponID)
		{
			return wd;
		}
	}
	return {};
}

FInstanceArmourData UBaseGameInstance::GetInstancedArmourByContainerID(int32 inContainerInstanceID)
{
	for (auto& ad : GetInstancedArmour())
	{
		if (ad.Value.containerInstanceID == inContainerInstanceID)
		{
			return ad.Value;
		}
	}
	return {};
}

FString UBaseGameInstance::GetContainerInstanceName(int32 containerID)
{
	return GetInstancedContainers().FindChecked(containerID).name;
}


FArmourData UBaseGameInstance::GetArmourData(int32 armourID)
{
	for (const FArmourData ad : mTable()->GetArmourDataTable()->GetData())
	{
		if (ad.ID == armourID)
		{
			return ad;
		}
	}
	return {};
}

FContainerData UBaseGameInstance::GetContainerDataName(FString containerName)
{
	for (auto& cd : mTable()->GetContainerData()->GetData())
	{
		if (cd.Value.name.Equals(containerName))
			return cd.Value;
	}
	return {};
}

FContainerData UBaseGameInstance::GetContainerDataByID(int32 containerID)
{
	return mTable()->GetContainerData()->GetData().FindChecked(containerID);
}

FArmourData UBaseGameInstance::GetArmourDataByItemID(int32 itemID)
{
	for (const FArmourData ad : mTable()->GetArmourDataTable()->GetData())
	{
		if (ad.itemID == itemID)
		{
			return ad;
		}
	}
	return {};
}

FLoadoutData UBaseGameInstance::GetLoadoutData(FString loadoutName)
{
	for (const FLoadoutData ld : mTable()->GetLoadoutTableData()->GetData())
	{
		if (ld.name.Equals( loadoutName))
		{
			return ld;
		}
	}
	return {};
}

EGearType UBaseGameInstance::GetGearTypeForItem(int32 itemID)
{
	FItemData id = GetItemData(itemID);
	if (id.type == EItemType::Armour)
	{
		FArmourData ad = GetArmourDataByItemID(itemID);
		return ad.slot;
	}
	else if (id.type == EItemType::Weapon)
	{
		FWeaponData wd = GetWeaponData(itemID);
		return wd.gearType;
	}

	return EGearType::End;
}

/**
 * Due to the data being structs, the only way to actually update information in the Map of data, is by adding the copy of the struct back in the array, as if it was new
 *
 * @param inData The data we're adding or updating, it will match on ID of the data
 *
 */
void UBaseGameInstance::AddUpdateData(const FInstanceArmourData& inData)
{
	GetInstancedArmour().Add(inData.ID, inData);
}


/**
 * Due to the data being structs, the only way to actually update information in the Map of data, is by adding the copy of the struct back in the array, as if it was new
 *
 * @param inData The data we're adding or updating, it will match on ID of the data
 *
 */
void UBaseGameInstance::AddUpdateData(const FInstanceItemData& inData)
{
	GetInstancedItems().Add(inData.ID, inData);
}

FInstanceArmourData UBaseGameInstance::GetInstanceArmourDataByInstanceItemID(int32 InstanceItemID)
{
	for (auto& iad : GetInstancedArmour())
	{
		if (iad.Value.instancedItemDataID == InstanceItemID)
			return iad.Value;
	}
	return {};
}

#pragma region Getters
UFactionManager* UBaseGameInstance::GetFactionManager()
{
	if (factionManager == nullptr) { factionManager = NewObject<UFactionManager>(); }
	return factionManager;
}

URPGEventManager* UBaseGameInstance::GetEventManager()
{
	if (eventManager == nullptr) { eventManager = NewObject<URPGEventManager>(); }
	return eventManager;
}

UTableManager* UBaseGameInstance::GetTableManager()
{
	if (tableManager == nullptr) { tableManager = NewObject<UTableManager>(); }
	return tableManager;
}

#pragma endregion Getters