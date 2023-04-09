#include "BaseGameInstance.h"

#include "FactionManager.h"
#include "Events/RPGEventManager.h"
#include "Items/ItemContainer.h"
#include "Tables/ArmourDataTable.h"
#include "Tables/ContainerTableData.h"
#include "Tables/CSVTable.h"
#include "Tables/ItemDataTable.h"
#include "Tables/LoadoutTableData.h"
#include "Tables/MeleeWeaponDataTable.h"
#include "Tables/ProjectileWeaponDataTable.h"
#include "Tables/RangedWeaponDataTable.h"
#include "Tables/WeaponDataTable.h"
#include "Missions/PatrolPath.h"

UBaseGameInstance::UBaseGameInstance()
{
}

void UBaseGameInstance::LoadTableFromFile(UCSVTable* table)
{
	const FString path = table->GetPath();

	if (FPaths::FileExists(path))
	{
		TArray<TArray<FString>> parsedCSV;
		FString FileContent;

		FFileHelper::LoadFileToString(FileContent, *path);

		const TCHAR* Terminators[] = {L"\r", L"\n"};
		const TCHAR* CSVDelimiters[] = {TEXT(","), TEXT("\t")};

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

TArray<FString> UBaseGameInstance::CleanData(TArray<FString> strings)
{
	TArray<FString> newStrings;
	for (FString s : strings)
	{
		newStrings.Add(s.TrimQuotes());
	}
	return newStrings;
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
	TArray<FInstanceItemData> items;
	TArray<FInstanceItemData> itemsFound;

	GetInstancedItems().GenerateValueArray(items);

	for (FInstanceItemData iid : items)
	{
		if (iid.containerInstanceID == instanceContainerID)
		{
			itemsFound.Add(iid);
		}
	}
	return itemsFound;
}

void UBaseGameInstance::LoadTableData()
{
	LoadTableFromFile(GetItemDataTable());
	LoadTableFromFile(GetWeaponDataTable());
	LoadTableFromFile(GetRangedWeaponData());
	LoadTableFromFile(GetMeleeWeaponData());
	LoadTableFromFile(GetProjectileWeaponData());
	LoadTableFromFile(GetArmourDataTable());
	LoadTableFromFile(GetLoadoutTableData());
	LoadTableFromFile(GetContainerData());
}

UFactionManager* UBaseGameInstance::GetFactionManager()
{
	if (factionManager == nullptr)
	{
		factionManager = NewObject<UFactionManager>();
	}

	return factionManager;
}


FItemData UBaseGameInstance::GetItemData(int32 itemID)
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

FWeaponData UBaseGameInstance::GetWeaponData(int32 itemID)
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

FMeleeWeaponData UBaseGameInstance::GetMeleeWeaponData(int32 weaponID)
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

FRangedWeaponData UBaseGameInstance::GetRangedWeaponData(int32 weaponID)
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

FProjectileWeaponData UBaseGameInstance::GetProjectileWeaponData(int32 rangedWeaponID)
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
	for (const FArmourData ad : GetArmourDataTable()->GetData())
	{
		if (ad.ID == armourID)
		{
			return ad;
		}
	}
	return {};
}

FContainerData UBaseGameInstance::GetContainerDataByID(int32 containerID)
{
	return GetContainerData()->GetData().FindChecked(containerID);
}

FInstanceItemData UBaseGameInstance::CreateNewInstanceItem(int32 itemID, int32 amount, int32 slot, int32 containerInstanceID)
{
	FInstanceItemData data;
	data.ID = GetNextInstanceItemDataID();
	data.amount = amount;
	data.containerInstanceID = containerInstanceID;
	data.itemID = itemID;
	data.slot = slot;
	GetInstancedItems().Add(data.ID, data);
	return data;
}

FArmourData UBaseGameInstance::GetArmourDataByItemID(int32 itemID)
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

FLoadoutData UBaseGameInstance::GetLoadoutData(int32 entityID)
{
	for (const FLoadoutData ld : GetLoadoutTableData()->GetData())
	{
		if (ld.entityID == entityID)
		{
			return ld;
		}
	}
	return {};
}

TArray<FInstanceItemData> UBaseGameInstance::GetInventoryItems(int32 instanceContainerID)
{
	TArray<FInstanceItemData> data;
	for (TTuple<int32, FInstanceItemData>& iid : instanceItems)
	{
		if (iid.Value.containerInstanceID == instanceContainerID)
		{
			data.Add(iid.Value);
		}
	}
	return data;
}

void UBaseGameInstance::Init()
{
	Super::Init();
	LoadTableData();
}

URPGEventManager* UBaseGameInstance::GetEventManager()
{
	if (eventManager == nullptr)
	{
		eventManager = NewObject<URPGEventManager>();
	}

	return eventManager;
}

UItemDataTable* UBaseGameInstance::GetItemDataTable()
{
	if (ItemData == nullptr)
	{
		ItemData = NewObject<UItemDataTable>();
	}

	return ItemData;
}

UWeaponDataTable* UBaseGameInstance::GetWeaponDataTable()
{
	if (WeaponData == nullptr)
	{
		WeaponData = NewObject<UWeaponDataTable>();
	}

	return WeaponData;
}

URangedWeaponDataTable* UBaseGameInstance::GetRangedWeaponData()
{
	if (rangedWeaponData == nullptr)
	{
		rangedWeaponData = NewObject<URangedWeaponDataTable>();
	}

	return rangedWeaponData;
}

UProjectileWeaponDataTable* UBaseGameInstance::GetProjectileWeaponData()
{
	if (projectileWeaponData == nullptr)
	{
		projectileWeaponData = NewObject<UProjectileWeaponDataTable>();
	}

	return projectileWeaponData;
}

UMeleeWeaponDataTable* UBaseGameInstance::GetMeleeWeaponData()
{
	if (meleeWeaponData == nullptr)
	{
		meleeWeaponData = NewObject<UMeleeWeaponDataTable>();
	}

	return meleeWeaponData;
}

UArmourDataTable* UBaseGameInstance::GetArmourDataTable()
{
	if (armourDataTable == nullptr)
	{
		armourDataTable = NewObject<UArmourDataTable>();
	}

	return armourDataTable;
}

ULoadoutTableData* UBaseGameInstance::GetLoadoutTableData()
{
	if (loadoutTableData == nullptr)
	{
		loadoutTableData = NewObject<ULoadoutTableData>();
	}

	return loadoutTableData;
}

UContainerTableData* UBaseGameInstance::GetContainerData()
{
	if (containerData == nullptr)
	{
		containerData = NewObject<UContainerTableData>();
	}

	return containerData;
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
