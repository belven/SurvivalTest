#pragma once
#include "CoreMinimal.h"
#include "CSVTable.h"
#include "TableManager.generated.h"

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

	void LoadTableData();
	void LoadTableFromFile(UCSVTable* table);
	void RemoveContainerData(int32 containerInstanceID);
	FInstanceBoxData GetInstanceBoxDataByContainerInstance(int32 BoxInstanceID);
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
	FContainerData GetContainerDataName(FString containerName);
	FContainerData GetContainerDataByID(int32 containerID);
	FArmourData GetArmourDataByItemID(int32 itemID);
	FLoadoutData GetLoadoutData(FString loadoutName);
	EGearType GetGearTypeForItem(int32 itemID);
	FConsumableData GetConsumableData(int32 itemID);
	FInstanceWeaponData GetInstanceWeaponDataByInstanceItemID(int32 instanceItemID);
	FInstanceArmourData GetInstancedArmourByContainerID(int32 inContainerInstanceID);
	FString GetContainerInstanceName(int32 containerID);

	TMap<int32, FInstanceContainerData>& GetInstancedContainers() { return instancedContainers; }
	TMap<int32, FInstanceArmourData>& GetInstancedArmour() { return armourInstances; }
	TMap<int32, FInstanceBoxData>& GetInstancedBoxes() { return boxContainers; }
	FItemData GetItemDataByName(const FString& inString);

private:
	TMap<int32, FInstanceContainerData> instancedContainers;
	TMap<int32, FInstanceArmourData> armourInstances;
	TMap<int32, FInstanceBoxData> boxContainers;

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
};
