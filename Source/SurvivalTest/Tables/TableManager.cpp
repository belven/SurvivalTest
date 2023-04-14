#include "TableManager.h"
#include "ArmourDataTable.h"
#include "ContainerTableData.h"
#include "CSVTable.h"
#include "ItemDataTable.h"
#include "LoadoutTableData.h"
#include "MeleeWeaponDataTable.h"
#include "ProjectileWeaponDataTable.h"
#include "RangedWeaponDataTable.h"
#include "WeaponDataTable.h"
#include "ConsumableTableData.h"

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

TArray<FString> UTableManager::CleanData(TArray<FString> strings)
{
	TArray<FString> newStrings;
	for (FString s : strings)
	{
		newStrings.Add(s.TrimQuotes());
	}
	return newStrings;
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

#pragma endregion Getters