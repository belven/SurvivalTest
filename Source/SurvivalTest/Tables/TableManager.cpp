#include "TableManager.h"
#include "ArmourDataTable.h"
#include "ContainerTableData.h"
#include "CSVTable.h"
#include "Items/ItemDataTable.h"
#include "LoadoutTableData.h"
#include "Weapons/MeleeWeaponDataTable.h"
#include "Weapons/ProjectileWeaponDataTable.h"
#include "Weapons/RangedWeaponDataTable.h"
#include "Weapons/WeaponDataTable.h"
#include "ConsumableTableData.h"
#include "Items/InstanceItemDataTable.h"
#include "Items/WeaponInstanceTable.h"
#include "Mission/MissionItemTable.h"
#include "Mission/MissionLoadoutTable.h"
#include "Mission/MissionTable.h"

void UTableManager::LoadTableData()
{
	LoadTableFromFile(GetItemDataTable());
	LoadTableFromFile(GetWeaponDataTable());
	LoadTableFromFile(GetRangedWeaponData());
	LoadTableFromFile(GetMeleeWeaponData());
	LoadTableFromFile(GetProjectileWeaponData());
	LoadTableFromFile(GetArmourDataTable());
	LoadTableFromFile(GetLoadoutTableData());
	LoadTableFromFile(GetContainerData());
	LoadTableFromFile(GetConsumableData());
	LoadTableFromFile(GetMissionTable());
	LoadTableFromFile(GetMissionLoadoutTable());
	LoadTableFromFile(GetMissionItemTable());
}

void UTableManager::LoadTableFromFile(UCSVTable* table)
{
	const FString path = table->GetPath();

	if (FPaths::FileExists(path))
	{
		TArray<TArray<FString>> parsedCSV;
		FString FileContent;

		FFileHelper::LoadFileToString(FileContent, *path);

		const TCHAR* Terminators[] = { L"\r", L"\n" };
		const TCHAR* CSVDelimiters[] = { TEXT(","), TEXT("\t") };

		TArray<FString> CSVLines;
		FileContent.ParseIntoArray(CSVLines, Terminators, 2);

		TArray<FString> temp_array;
		for (int i = 0; i < CSVLines.Num(); i++)
		{
			temp_array.Empty();
			CSVLines[i].ParseIntoArray(temp_array, CSVDelimiters, 2);
			parsedCSV.Add(CleanData(temp_array));
		}

		table->LoadData(parsedCSV);
	}
}

FInstanceWeaponData UTableManager::GetInstanceWeaponDataByInstanceItemID(int32 instanceItemID)
{
	int32 instanceContainerDataID = 0;
	TMap<int32, FInstanceWeaponData> instancedWeapons = GetWeaponInstanceTable()->GetData();

	for (auto& iwd : instancedWeapons)
	{
		if (iwd.Value.instanceItemID == instanceItemID)
		{
			return iwd.Value;
		}
	}

	return {};
}


TArray<FInstanceItemData> UTableManager::GetInstancedItemsForContainer(int32 instanceContainerID)
{
	TArray<FInstanceItemData> data;
	for (auto& iid : GetInstanceItemDataTable()->GetData())
	{
		if (iid.Value.containerInstanceID == instanceContainerID)
		{
			data.Add(iid.Value);
		}
	}
	return data;
}

FItemData UTableManager::GetItemData(int32 itemID)
{
	TArray<FItemData> itemData = GetItemDataTable()->GetData();
	for (const FItemData id : itemData)
	{
		if (id.ID == itemID)
		{
			return id;
		}
	}
	return {};
}

FWeaponData UTableManager::GetWeaponData(int32 itemID)
{
	TArray<FWeaponData> weaponData = GetWeaponDataTable()->GetData();
	for (const FWeaponData wd : weaponData)
	{
		if (wd.itemID == itemID)
		{
			return wd;
		}
	}
	return {};
}

FConsumableData UTableManager::GetConsumableData(int32 itemID)
{
	for (const auto& cd : GetConsumableData()->GetData())
	{
		if (cd.Value.itemID == itemID)
		{
			return cd.Value;
		}
	}
	return {};
}

FMeleeWeaponData UTableManager::GetMeleeWeaponData(int32 weaponID)
{
	for (const FMeleeWeaponData wd : GetMeleeWeaponData()->GetData())
	{
		if (wd.weaponID == weaponID)
		{
			return wd;
		}
	}
	return {};
}

FRangedWeaponData UTableManager::GetRangedWeaponData(int32 weaponID)
{
	for (const FRangedWeaponData wd : GetRangedWeaponData()->GetData())
	{
		if (wd.weaponID == weaponID)
		{
			return wd;
		}
	}
	return {};
}

FProjectileWeaponData UTableManager::GetProjectileWeaponData(int32 rangedWeaponID)
{
	for (const FProjectileWeaponData wd : GetProjectileWeaponData()->GetData())
	{
		if (wd.rangedWeaponID == rangedWeaponID)
		{
			return wd;
		}
	}
	return {};
}

FInstanceArmourData UTableManager::GetInstancedArmourByContainerID(int32 inContainerInstanceID)
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

FString UTableManager::GetContainerInstanceName(int32 containerID)
{
	return GetInstancedContainers().FindChecked(containerID).name;
}

FItemData UTableManager::GetItemDataByName(const FString& inString)
{
	TArray<FItemData> itemData = GetItemDataTable()->GetData();
	for (const FItemData id : itemData)
	{
		if (id.name.Equals(inString))
		{
			return id;
		}
	}
	return {};
}


FArmourData UTableManager::GetArmourData(int32 armourID)
{
	for (const FArmourData ad : GetArmourDataTable()->GetData())
	{
		if (ad.ID == armourID)
		{
			return ad;
		}
	}
	return {};
}

FContainerData UTableManager::GetContainerDataName(FString containerName)
{
	for (auto& cd : GetContainerData()->GetData())
	{
		if (cd.Value.name.Equals(containerName))
			return cd.Value;
	}
	return {};
}

FContainerData UTableManager::GetContainerDataByID(int32 containerID)
{
	return GetContainerData()->GetData().FindChecked(containerID);
}

FArmourData UTableManager::GetArmourDataByItemID(int32 itemID)
{
	for (const FArmourData ad : GetArmourDataTable()->GetData())
	{
		if (ad.itemID == itemID)
		{
			return ad;
		}
	}
	return {};
}

FLoadoutData UTableManager::GetLoadoutData(FString loadoutName)
{
	for (const FLoadoutData ld : GetLoadoutTableData()->GetData())
	{
		if (ld.name.Equals(loadoutName))
		{
			return ld;
		}
	}
	return {};
}

EGearType UTableManager::GetGearTypeForItem(int32 itemID)
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
void UTableManager::AddUpdateData(const FInstanceItemData& inData)
{
	GetInstanceItemDataTable()->GetData().Add(inData.ID, inData);
}

FInstanceArmourData UTableManager::GetInstanceArmourDataByInstanceItemID(int32 InstanceItemID)
{
	for (auto& iad : GetInstancedArmour())
	{
		if (iad.Value.instancedItemDataID == InstanceItemID)
			return iad.Value;
	}
	return {};
}

/**
 * Due to the data being structs, the only way to actually update information in the Map of data, is by adding the copy of the struct back in the array, as if it was new
 *
 * @param inData The data we're adding or updating, it will match on ID of the data
 *
 */
void UTableManager::AddUpdateData(const FInstanceArmourData& inData)
{
	GetInstancedArmour().Add(inData.ID, inData);
}

/**
 * Due to the data being structs, the only way to actually update information in the Map of data, is by adding the copy of the struct back in the array, as if it was new
 *
 * @param inData The data we're adding or updating, it will match on ID of the data
 *
 */
void UTableManager::AddUpdateData(const FInstanceWeaponData& inData)
{
	GetWeaponInstanceTable()->GetData().Add(inData.ID, inData);
}


void UTableManager::SaveTableToFile(UCSVTable* table)
{
	FFileHelper::SaveStringArrayToFile(table->GetDataStrings(), *table->GetPath());
}

TArray<FString> UTableManager::CleanData(TArray<FString> strings)
{
	TArray<FString> newStrings;
	for (FString s : strings)
	{
		newStrings.Add(s.TrimQuotes());
	}
	return newStrings;
}

TArray<int32> UTableManager::GetItemsForMissionType(EMissionType type)
{
	TArray<int32> itemTypes;

	for (FMissionItemData mid : GetMissionItemTable()->GetData())
	{
		if (mid.type == type)
			itemTypes.Add(mid.itemID);
	}

	return itemTypes;
}

#pragma region Getters

UItemDataTable* UTableManager::GetItemDataTable()
{
	if (ItemData == nullptr) { ItemData = NewObject<UItemDataTable>(); }
	return ItemData;
}

UWeaponDataTable* UTableManager::GetWeaponDataTable()
{
	if (WeaponData == nullptr) { WeaponData = NewObject<UWeaponDataTable>(); }
	return WeaponData;
}

URangedWeaponDataTable* UTableManager::GetRangedWeaponData()
{
	if (rangedWeaponData == nullptr) { rangedWeaponData = NewObject<URangedWeaponDataTable>(); }
	return rangedWeaponData;
}

UProjectileWeaponDataTable* UTableManager::GetProjectileWeaponData()
{
	if (projectileWeaponData == nullptr) { projectileWeaponData = NewObject<UProjectileWeaponDataTable>(); }
	return projectileWeaponData;
}

UMeleeWeaponDataTable* UTableManager::GetMeleeWeaponData()
{
	if (meleeWeaponData == nullptr) { meleeWeaponData = NewObject<UMeleeWeaponDataTable>(); }
	return meleeWeaponData;
}

UArmourDataTable* UTableManager::GetArmourDataTable()
{
	if (armourDataTable == nullptr) { armourDataTable = NewObject<UArmourDataTable>(); }
	return armourDataTable;
}

ULoadoutTableData* UTableManager::GetLoadoutTableData()
{
	if (loadoutTableData == nullptr) { loadoutTableData = NewObject<ULoadoutTableData>(); }
	return loadoutTableData;
}

UContainerTableData* UTableManager::GetContainerData()
{
	if (containerData == nullptr) { containerData = NewObject<UContainerTableData>(); }
	return containerData;
}

UConsumableTableData* UTableManager::GetConsumableData()
{
	if (consumableData == nullptr) { consumableData = NewObject<UConsumableTableData>(); }
	return consumableData;
}

UMissionTable* UTableManager::GetMissionTable()
{
	if (missionTable == nullptr) { missionTable = NewObject<UMissionTable>(); }
	return missionTable;
}

UMissionLoadoutTable* UTableManager::GetMissionLoadoutTable()
{
	if (missionLoadoutTable == nullptr) { missionLoadoutTable = NewObject<UMissionLoadoutTable>(); }
	return missionLoadoutTable;
}

UInstanceItemDataTable* UTableManager::GetInstanceItemDataTable()
{
	if (instanceItemDataTable == nullptr) { instanceItemDataTable = NewObject<UInstanceItemDataTable>(); }
	return instanceItemDataTable;
}

UMissionItemTable* UTableManager::GetMissionItemTable()
{
	if (missionItemTable == nullptr) { missionItemTable = NewObject<UMissionItemTable>(); }
	return missionItemTable;
}

UWeaponInstanceTable* UTableManager::GetWeaponInstanceTable()
{
	if (weaponInstances == nullptr) { weaponInstances = NewObject<UWeaponInstanceTable>(); }
	return weaponInstances;	
}

#pragma endregion Getters
