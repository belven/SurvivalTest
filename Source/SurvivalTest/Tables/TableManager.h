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
	void SaveTableToFile(UCSVTable* table);
	TArray<FString> CleanData(TArray<FString> strings);

	TArray<int32> GetItemsForMissionType(EMissionType type);

private:
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
