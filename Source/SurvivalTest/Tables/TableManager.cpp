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
#include "ContainerItemTableData.h"
#include "Items/InputOutputDataTable.h"
#include "Items/RecipeInputOutputDataTable.h"
#include "Items/RecipeTable.h"
#include "Crafting/CraftingDeviceRecipesTable.h"
#include "Crafting/CraftingDeviceTable.h"
#include "Crafting/InProgressCraftingTable.h"
#include "Crafting/InstanceCraftingDeviceTable.h"
#include "Mission/MissionContainerTableData.h"
#include "SurvivalTest/HelperFunctions.h"

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
	LoadTableFromFile(GetContainerItemDataTable());
	LoadTableFromFile(GetMissionContainerTableData());
	LoadTableFromFile(GetRecipeInputOutputDataTableData());
	LoadTableFromFile(GetInputOutputDataTableData());
	LoadTableFromFile(GetRecipeTableData());
	LoadTableFromFile(GetCraftingDeviceRecipesTable());
	LoadTableFromFile(GetCraftingDeviceTableData());
	LoadTableFromFile(GetInProgressCraftingTable());
	LoadTableFromFile(GetInstanceCraftingDeviceTable());
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

void UTableManager::RemoveContainerData(int32 containerInstanceID)
{
	TArray<FInstanceItemData> items = GetInstancedItemsForContainer(containerInstanceID);
	FInstanceContainerData icd = GetInstanceContainerData(containerInstanceID);

	if (icd.type == EContainerType::Box)
	{
		int32 boxID = GetInstanceBoxDataByContainerInstance(containerInstanceID).ID;
		GetInstancedBoxes().FindAndRemoveChecked(boxID);

		// TODO make box container CSVTable
	}
	else if (icd.type == EContainerType::Player)
	{
		int32 playerID = 0;
		// TODO make player container data FInstancePlayerInventory
	}
	else if (icd.type == EContainerType::Armour)
	{
		FInstanceArmourData armour = GetInstancedArmourByContainerID(icd.ID);
		GetInstancedBoxes().FindAndRemoveChecked(armour.ID);
	}

	for (FInstanceItemData iid : items)
	{
		GetInstanceItemDataTable()->GetData().FindAndRemoveChecked(iid.ID);
	}

	instancedContainers.FindAndRemoveChecked(icd.ID);
}

FInstanceBoxData UTableManager::GetInstanceBoxDataByContainerInstance(int32 containerInstanceID)
{
	TArray<FInstanceBoxData> instancedBoxesFound;
	boxContainers.GenerateValueArray(instancedBoxesFound);

	for (const FInstanceBoxData ibd : instancedBoxesFound)
	{
		if (ibd.containerInstanceID == containerInstanceID)
		{
			return ibd;
		}
	}
	return {};
}


FInstanceContainerData UTableManager::GetInstanceContainerData(int32 containerInstanceID)
{
	TArray<FInstanceContainerData> instancedContainersFound;
	instancedContainers.GenerateValueArray(instancedContainersFound);

	for (const FInstanceContainerData icd : instancedContainersFound)
	{
		if (icd.ID == containerInstanceID)
		{
			return icd;
		}
	}
	return {};
}

FInstanceWeaponData UTableManager::GetInstanceWeaponDataByInstanceItemID(int32 instanceItemID)
{
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

TArray<FFullRecipe>& UTableManager::GetRecipes()
{
	if (recipes.IsEmpty()) {
		for (FRecipeData recipeData : GetRecipeTableData()->GetData())
		{
			FFullRecipe fullRecipe;
			fullRecipe.recipe = recipeData;

			// ReSharper disable once IdentifierTypo
			for (FRecipeInputOutputData riod : GetRecipeInputOutputDataTableData()->GetData())
			{
				if (riod.recipeID == recipeData.ID)
				{
					for (FInputOutputData iod : GetInputOutputDataTableData()->GetData()) {
						if (iod.ID == riod.inputOutputDataID) {
							switch (iod.inputOrOutput)
							{
							case EInputOrOutput::Input:
								fullRecipe.inputs.Add(iod);
								break;
							case EInputOrOutput::Output:
								fullRecipe.outputs.Add(iod);
								break;
							case EInputOrOutput::End:
								break;
							}
						}
					}
				}
			}

			recipes.Add(fullRecipe);
		}
	}

	return recipes;
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

FContainerData UTableManager::GetContainerDataName(const FString& containerName)
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

FLoadoutData UTableManager::GetLoadoutData(const FString& loadoutName)
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

TArray<int32> UTableManager::GetContainerItems(int32 containerID)
{
	TArray<int32> items;

	for (FContainerItemData data : containerItemDataTable->GetData())
	{
		if (data.containerID == containerID)
		{
			items.Add(data.itemID);
		}
	}
	return items;
}

TArray<FMissionContainerData> UTableManager::GetMissionContainers(EMissionType type)
{
	TArray<FMissionContainerData> missionContainers;

	for (FMissionContainerData data : GetMissionContainerTableData()->GetData())
	{
		if (data.type == type)
		{
			missionContainers.Add(data);
		}
	}
	return missionContainers;
}

#pragma region Getters

UItemDataTable* UTableManager::GetItemDataTable()
{
	return UHelperFunctions::GetValue(ItemData, this);
}

UWeaponDataTable* UTableManager::GetWeaponDataTable()
{
	return UHelperFunctions::GetValue(WeaponData, this);
}

URangedWeaponDataTable* UTableManager::GetRangedWeaponData()
{
	return UHelperFunctions::GetValue(rangedWeaponData, this);
}

UProjectileWeaponDataTable* UTableManager::GetProjectileWeaponData()
{
	return UHelperFunctions::GetValue(projectileWeaponData, this);
}

UMeleeWeaponDataTable* UTableManager::GetMeleeWeaponData()
{
	return UHelperFunctions::GetValue(meleeWeaponData, this);
}

UArmourDataTable* UTableManager::GetArmourDataTable()
{
	return UHelperFunctions::GetValue(armourDataTable, this);
}

ULoadoutTableData* UTableManager::GetLoadoutTableData()
{
	return UHelperFunctions::GetValue(loadoutTableData, this);
}

UContainerTableData* UTableManager::GetContainerData()
{
	return UHelperFunctions::GetValue(containerData, this);
}

UConsumableTableData* UTableManager::GetConsumableData()
{
	return UHelperFunctions::GetValue(consumableData, this);
}

UMissionTable* UTableManager::GetMissionTable()
{
	return UHelperFunctions::GetValue(missionTable, this);
}

UMissionLoadoutTable* UTableManager::GetMissionLoadoutTable()
{
	return UHelperFunctions::GetValue(missionLoadoutTable, this);
}

UInstanceItemDataTable* UTableManager::GetInstanceItemDataTable()
{
	return UHelperFunctions::GetValue(instanceItemDataTable, this);
}

UMissionItemTable* UTableManager::GetMissionItemTable()
{
	return UHelperFunctions::GetValue(missionItemTable, this);
}

UWeaponInstanceTable* UTableManager::GetWeaponInstanceTable()
{
	return UHelperFunctions::GetValue(weaponInstances, this);
}

UContainerItemTableData* UTableManager::GetContainerItemDataTable()
{
	return UHelperFunctions::GetValue(containerItemDataTable, this);
}

UMissionContainerTableData* UTableManager::GetMissionContainerTableData()
{
	return UHelperFunctions::GetValue(missionContainerTableData, this);
}

URecipeInputOutputDataTable* UTableManager::GetRecipeInputOutputDataTableData()
{
	return UHelperFunctions::GetValue(recipeInputOutputDataTable, this);
}

UInputOutputDataTable* UTableManager::GetInputOutputDataTableData()
{
	return UHelperFunctions::GetValue(inputOutputDataTable, this);
}

URecipeTable* UTableManager::GetRecipeTableData()
{
	return UHelperFunctions::GetValue(recipeTable, this);
}

UCraftingDeviceTable* UTableManager::GetCraftingDeviceTableData()
{
	return UHelperFunctions::GetValue(CraftingDeviceTable, this);
}

UCraftingDeviceRecipesTable* UTableManager::GetCraftingDeviceRecipesTable()
{
	return UHelperFunctions::GetValue(CraftingDeviceRecipesTable, this);
}

UInProgressCraftingTable* UTableManager::GetInProgressCraftingTable()
{
	return UHelperFunctions::GetValue(InProgressCraftingTable, this);
}

UInstanceCraftingDeviceTable* UTableManager::GetInstanceCraftingDeviceTable()
{
	return UHelperFunctions::GetValue(InstanceCraftingDeviceTable, this);
}

#pragma endregion Getters
