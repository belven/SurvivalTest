#pragma once
#include "CoreMinimal.h"
#include "CSVTable.h"
#include "Items/InstanceItemDataTable.h"
#include "SurvivalTest/Items/ItemStructs.h"
#include "SurvivalTest/Missions/MissionStructs.h"
#include "TableManager.generated.h"

class UInProgressCraftingTable;
class UCraftingDeviceRecipesTable;
struct FInstanceItemData;
struct FInstanceWeaponData;
struct FInstanceArmourData;
enum class EMissionType : uint8;
class UProjectileWeaponDataTable;
class UMeleeWeaponDataTable;
class URangedWeaponDataTable;
class UWeaponDataTable;
class UItemDataTable;
class UArmourDataTable;
class UArmourResistanceDataTable;
class ULoadoutTableData;
class UContainerTableData;
class UConsumableTableData;
class UMissionTable;
class UMissionLoadoutTable;
class UInstanceItemDataTable;
class UMissionItemTable;
class UWeaponInstanceTable;
class UContainerItemTableData;
class UMissionContainerTableData;
class URecipeInputOutputDataTable;
class UInputOutputDataTable;
class URecipeTable;
class UCraftingDeviceTable;
class UInstanceCraftingDeviceTable;

UCLASS()
class SURVIVALTEST_API UTableManager : public UObject
{
	GENERATED_BODY()

public:
	UItemDataTable* GetItemDataTable();
	UWeaponDataTable* GetWeaponDataTable();
	URangedWeaponDataTable* GetRangedWeaponData();
	UProjectileWeaponDataTable* GetProjectileWeaponData();
	UMeleeWeaponDataTable* GetMeleeWeaponData();
	UArmourDataTable* GetArmourDataTable();
	ULoadoutTableData* GetLoadoutTableData();
	UContainerTableData* GetContainerData();
	UConsumableTableData* GetConsumableData();
	UMissionTable* GetMissionTable();
	UMissionLoadoutTable* GetMissionLoadoutTable();
	UInstanceItemDataTable* GetInstanceItemDataTable();
	UMissionItemTable* GetMissionItemTable();
	UWeaponInstanceTable* GetWeaponInstanceTable();
	UContainerItemTableData* GetContainerItemDataTable();
	UMissionContainerTableData* GetMissionContainerTableData();

	URecipeInputOutputDataTable* GetRecipeInputOutputDataTableData();
	UInputOutputDataTable* GetInputOutputDataTableData();
	URecipeTable* GetRecipeTableData();
	UCraftingDeviceTable* GetCraftingDeviceTableData();
	UCraftingDeviceRecipesTable* GetCraftingDeviceRecipesTable();
	UInProgressCraftingTable* GetInProgressCraftingTable();
	UInstanceCraftingDeviceTable* GetInstanceCraftingDeviceTable();

	int32 GetNextInstanceItemDataID();
	int32 GetNextInstanceLootBoxDataID();
	int32 GetNextInstanceArmourDataID();
	FInstanceWeaponData CreateNewInstanceWeaponData(int32 instanceItemID, const FProjectileWeaponData& pwd);
	int32 GetNextInstanceContainerDataID();
	int32 GetNextInstanceWeaponDataID();

	void LoadTableData();
	void LoadTableFromFile(UCSVTable* table);
	void RemoveContainerData(int32 containerInstanceID);


	FInstanceItemData GetInstanceItemDataByID(int32 instanceItemID) { return GetInstanceItemDataTable()->GetInstanceItemDataByID(instanceItemID); }

	FInstanceLootBoxData GetInstanceBoxDataByContainerInstance(int32 containerInstanceID);
	FInstanceContainerData GetInstanceContainerData(int32 containerInstanceID);
	void SaveTableToFile(UCSVTable* table);
	TArray<FString> CleanData(TArray<FString> strings);

	TArray<int32> GetItemsForMissionType(EMissionType type);

	void AddUpdateData(const FInstanceArmourData& inData);
	void AddUpdateData(const FInstanceWeaponData& inData);
	void AddUpdateData(const FInstanceItemData& inData);
	TArray<FInstanceItemData> GetInstancedItemsForContainer(int32 instanceContainerID);
	FInstanceArmourData GetInstanceArmourDataByInstanceItemID(int32 InstanceItemID);

	FItemData GetItemData(int32 itemID);
	FWeaponData GetWeaponData(int32 itemID);
	FMeleeWeaponData GetMeleeWeaponData(int32 weaponID);
	FRangedWeaponData GetRangedWeaponData(int32 weaponID);
	FProjectileWeaponData GetProjectileWeaponData(int32 rangedWeaponID);
	FArmourData GetArmourData(int32 armourID);
	FContainerData GetContainerDataName(const FString& containerName);
	FContainerData GetContainerDataByID(int32 containerID);
	FArmourData GetArmourDataByItemID(int32 itemID);
	FLoadoutData GetLoadoutData(const FString& loadoutName);
	EGearType GetGearTypeForItem(int32 itemID);
	FConsumableData GetConsumableData(int32 itemID);
	FInstanceWeaponData GetInstanceWeaponDataByInstanceItemID(int32 instanceItemID);
	FInstanceArmourData GetInstancedArmourByContainerID(int32 inContainerInstanceID);
	FString GetContainerInstanceName(int32 containerID);
	TArray<int32> GetContainerItems(int32 containerID);
	TArray<FMissionContainerData> GetMissionContainers(EMissionType type);

	TMap<int32, FInstanceContainerData>& GetInstancedContainers() { return instancedContainers; }
	TMap<int32, FInstanceArmourData>& GetInstancedArmour() { return armourInstances; }
	TMap<int32, FInstanceLootBoxData>& GetInstancedLootBoxes() { return instancedLootboxes; }
	FItemData GetItemDataByName(const FString& inString);

	UFUNCTION(BlueprintCallable, Category = "Recipe")
	TArray<FFullRecipe> GetRecipes(int32 craftingDeviceID);

private:
	TMap<int32, FInstanceContainerData> instancedContainers;
	TMap<int32, FInstanceArmourData> armourInstances;
	TMap<int32, FInstanceLootBoxData> instancedLootboxes;
	TMap<int32, FFullRecipe> recipes;
	TMap<int32, TArray<FFullRecipe>> craftingDeviceFullRecipes;

	FCriticalSection InstanceItemIDLock;
	int32 LastInstanceItemID = -1;

	FCriticalSection InstanceContainerDataLock;
	int32 LastInstanceContainerDataID = -1;

	FCriticalSection InstanceWeaponDataLock;
	int32 LastInstanceWeaponDataID = -1;

	FCriticalSection InstanceArmourDataLock;
	int32 LastInstanceArmourDataID = -1;

	FCriticalSection InstanceLootBoxDataIDLock;
	int32 LastInstanceLootBoxDataID = -1;

	UPROPERTY()
	UItemDataTable* ItemData;

	UPROPERTY()
	UInstanceItemDataTable* instanceItemDataTable;

	UPROPERTY()
	UWeaponDataTable* WeaponData;

	UPROPERTY()
	URangedWeaponDataTable* rangedWeaponData;

	UPROPERTY()
	UMeleeWeaponDataTable* meleeWeaponData;

	UPROPERTY()
	UProjectileWeaponDataTable* projectileWeaponData;

	UPROPERTY()
	UArmourDataTable* armourDataTable;

	UPROPERTY()
	ULoadoutTableData* loadoutTableData;

	UPROPERTY()
	UContainerTableData* containerData;

	UPROPERTY()
	UConsumableTableData* consumableData;

	UPROPERTY()
	UMissionItemTable* missionItemTable;

	UPROPERTY()
	UMissionTable* missionTable;

	UPROPERTY()
	UMissionLoadoutTable* missionLoadoutTable;

	UPROPERTY()
	UWeaponInstanceTable* weaponInstances;

	UPROPERTY()
	UContainerItemTableData* containerItemDataTable;

	UPROPERTY()
	UMissionContainerTableData* missionContainerTableData;

	UPROPERTY()
	URecipeInputOutputDataTable* recipeInputOutputDataTable;

	UPROPERTY()
	UInputOutputDataTable* inputOutputDataTable;

	UPROPERTY()
	URecipeTable* recipeTable;

	UPROPERTY()
	UCraftingDeviceTable* CraftingDeviceTable;

	UPROPERTY()
	UCraftingDeviceRecipesTable* CraftingDeviceRecipesTable;

	UPROPERTY()
	UInProgressCraftingTable* InProgressCraftingTable;

	UPROPERTY()
	UInstanceCraftingDeviceTable* InstanceCraftingDeviceTable;
};
