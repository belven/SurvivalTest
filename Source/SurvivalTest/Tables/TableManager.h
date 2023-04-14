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

	void LoadTableData();
	void LoadTableFromFile(UCSVTable* table);
	TArray<FString> CleanData(TArray<FString> strings);

private:
	UPROPERTY()
	UItemDataTable* ItemData;

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
		UMissionTable* missionTable;

	UPROPERTY()
		UMissionLoadoutTable* missionLoadoutTable;
};
