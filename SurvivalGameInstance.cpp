#include "SurvivalGameInstance.h"

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

USurvivalGameInstance::USurvivalGameInstance()
{

}

void USurvivalGameInstance::LoadTableFromFile(UCSVTable* table)
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
		for (int i = 0; i < CSVLines.Num(); i++) {
			temp_array.Empty();
			CSVLines[i].ParseIntoArray(temp_array, CSVDelimiters, 2);
			parsedCSV.Add(CleanData(temp_array));
		}

		table->LoadData(parsedCSV);
	}
}

TArray<FString> USurvivalGameInstance::CleanData(TArray<FString> strings)
{
	TArray<FString> newStrings;
	for (FString s : strings)
	{
		newStrings.Add(s.TrimQuotes());
	}
	return newStrings;
}

int32 USurvivalGameInstance::GetNextInstanceItemDataID()
{
	int32 instanceItemDataID = 0;
	if (GetInstancedItems().Num() > 0)
	{
		instanceItemDataID = GetInstancedItems()[GetInstancedItems().Num() - 1].ID + 1;
	}
	return instanceItemDataID;
}

int32 USurvivalGameInstance::GetNextInstanceBoxDataID()
{
	int32 instanceBoxDataID = 0;
	if (GetInstancedBoxes().Num() > 0)
	{
		instanceBoxDataID = GetInstancedBoxes()[GetInstancedBoxes().Num() - 1].ID + 1;
	}
	return instanceBoxDataID;
}

int32 USurvivalGameInstance::GetNextInstanceContainerDataID()
{
	int32 instanceContainerDataID = 0;
	if (GetInstancedContainers().Num() > 0)
	{
		instanceContainerDataID = GetInstancedContainers()[GetInstancedContainers().Num() - 1].ID + 1;
	}
	return instanceContainerDataID;
}

void USurvivalGameInstance::LoadTableData()
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

UFactionManager* USurvivalGameInstance::GetFactionManager()
{
	if (factionManager == NULL)
	{
		factionManager = NewObject<UFactionManager>();
	}

	return factionManager;
}


FItemData USurvivalGameInstance::GetItemData(int32 itemID)
{
	TArray<FItemData> itemData = GetItemDataTable()->GetData();
	for (const FItemData id : itemData)
	{
		if (id.ID == itemID)
			return id;
	}
	return {};
}

FWeaponData USurvivalGameInstance::GetWeaponData(int32 itemID)
{
	TArray<FWeaponData> weaponData = GetWeaponDataTable()->GetData();
	for (const FWeaponData wd : weaponData)
	{
		if (wd.itemID == itemID)
			return wd;
	}
	return {};
}

FMeleeWeaponData USurvivalGameInstance::GetMeleeWeaponData(int32 weaponID)
{
	for (const FMeleeWeaponData wd : GetMeleeWeaponData()->GetData())
	{
		if (wd.weaponID == weaponID)
			return wd;
	}
	return {};
}

FRangedWeaponData USurvivalGameInstance::GetRangedWeaponData(int32 weaponID)
{
	for (const FRangedWeaponData wd : GetRangedWeaponData()->GetData())
	{
		if (wd.weaponID == weaponID)
			return wd;
	}
	return {};
}

FProjectileWeaponData USurvivalGameInstance::GetProjectileWeaponData(int32 rangedWeaponID)
{
	for (const FProjectileWeaponData wd : GetProjectileWeaponData()->GetData())
	{
		if (wd.rangedWeaponID == rangedWeaponID)
			return wd;
	}
	return {};
}


FArmourData USurvivalGameInstance::GetArmourData(int32 armourID)
{
	for (const FArmourData ad : GetArmourDataTable()->GetData())
	{
		if (ad.ID == armourID)
			return ad;
	}
	return {};
}

FContainerData USurvivalGameInstance::GetContainerDataByID(int32 containerID)
{
	return GetContainerData()->GetData().FindChecked(containerID);
}

FArmourData USurvivalGameInstance::GetArmourDataByItemID(int32 itemID)
{
	for (const FArmourData ad : GetArmourDataTable()->GetData())
	{
		if (ad.itemID == itemID)
			return ad;
	}
	return {};
}

FLoadoutData USurvivalGameInstance::GetLoadoutData(int32 entityID)
{
	for (const FLoadoutData ld : GetLoadoutTableData()->GetData())
	{
		if (ld.entityID == entityID)
			return ld;
	}
	return {};
}

TArray<FInstanceItemData> USurvivalGameInstance::GetInventoryItems(int32 instanceContainerID)
{
	TArray<FInstanceItemData> data;
	for(TTuple<int32, FInstanceItemData>& iid : instanceItems)
	{
		if (iid.Value.containerInstanceID == instanceContainerID)
			data.Add(iid.Value);
	}
	return data;
}

void USurvivalGameInstance::Init()
{
	Super::Init();
	LoadTableData();
}

URPGEventManager* USurvivalGameInstance::GetEventManager()
{
	if (eventManager == NULL) 
	{
		eventManager = NewObject<URPGEventManager>();
	}

	return eventManager;
}

UItemDataTable* USurvivalGameInstance::GetItemDataTable()
{
	if (ItemData == NULL)
	{
		ItemData = NewObject<UItemDataTable>();
	}

	return ItemData;
}

UWeaponDataTable* USurvivalGameInstance::GetWeaponDataTable()
{
	if (WeaponData == NULL)
	{
		WeaponData = NewObject<UWeaponDataTable>();
	}

	return WeaponData;
}

URangedWeaponDataTable* USurvivalGameInstance::GetRangedWeaponData()
{
	if (rangedWeaponData == NULL)
	{
		rangedWeaponData = NewObject<URangedWeaponDataTable>();
	}

	return rangedWeaponData;
}

UProjectileWeaponDataTable* USurvivalGameInstance::GetProjectileWeaponData()
{
	if (projectileWeaponData == NULL)
	{
		projectileWeaponData = NewObject<UProjectileWeaponDataTable>();
	}

	return projectileWeaponData;
}

UMeleeWeaponDataTable* USurvivalGameInstance::GetMeleeWeaponData()
{
	if (meleeWeaponData == NULL)
	{
		meleeWeaponData = NewObject<UMeleeWeaponDataTable>();
	}

	return meleeWeaponData;
}

UArmourDataTable* USurvivalGameInstance::GetArmourDataTable()
{
	if (armourDataTable == NULL)
	{
		armourDataTable = NewObject<UArmourDataTable>();
	}

	return armourDataTable;
}

ULoadoutTableData* USurvivalGameInstance::GetLoadoutTableData()
{
	if (loadoutTableData == NULL)
	{
		loadoutTableData = NewObject<ULoadoutTableData>();
	}

	return loadoutTableData;
}

UContainerTableData* USurvivalGameInstance::GetContainerData()
{
	if (containerData == NULL)
	{
		containerData = NewObject<UContainerTableData>();
	}

	return containerData;
}
