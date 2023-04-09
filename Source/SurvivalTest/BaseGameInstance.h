// Fill out your copyright notice in the Description page of Project Settings.
#pragma once
#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "Items/ItemStructs.h"
#include "Kismet/GameplayStatics.h"
#include "BaseGameInstance.generated.h"

#define GameInstance(world) Cast<UBaseGameInstance>(UGameplayStatics::GetGameInstance(world))

class UProjectileWeaponDataTable;
class UMeleeWeaponDataTable;
class URangedWeaponDataTable;
class UWeaponDataTable;
class UItemDataTable;
class UArmourDataTable;
class UArmourResistanceDataTable;
class ULoadoutTableData;
class UFactionManager;
class URPGEventManager;
class UContainerTableData;
class APatrolPath;

UCLASS()
class SURVIVALTEST_API UBaseGameInstance : public UGameInstance
{
	GENERATED_BODY()
public:
	UBaseGameInstance();

	UFUNCTION(BlueprintCallable)
		void LoadTableFromFile(UCSVTable* table);

	void LoadTableData();

	UFactionManager* GetFactionManager();

	UFUNCTION(BlueprintCallable)
		FItemData GetItemData(int32 itemID);

	FWeaponData GetWeaponData(int32 itemID);
	FMeleeWeaponData GetMeleeWeaponData(int32 weaponID);
	FRangedWeaponData GetRangedWeaponData(int32 weaponID);
	FProjectileWeaponData GetProjectileWeaponData(int32 rangedWeaponID);
	FArmourData GetArmourData(int32 armourID);
	FContainerData GetContainerDataByID(int32 containerID);

	FInstanceItemData CreateNewInstanceItem(int32 itemID, int32 amount, int32 slot, int32 containerInstanceID);

	FArmourData GetArmourDataByItemID(int32 itemID);

	FLoadoutData GetLoadoutData(int32 entityID);

	TArray<FInstanceItemData> GetInventoryItems(int32 instanceContainerID);

	virtual void Init() override;
	URPGEventManager* GetEventManager();

	UItemDataTable* GetItemDataTable();
	UWeaponDataTable* GetWeaponDataTable();
	URangedWeaponDataTable* GetRangedWeaponData();
	UProjectileWeaponDataTable* GetProjectileWeaponData();
	UMeleeWeaponDataTable* GetMeleeWeaponData();
	UArmourDataTable* GetArmourDataTable();
	ULoadoutTableData* GetLoadoutTableData();
	UContainerTableData* GetContainerData();
	EGearType GetGearTypeForItem(int32 itemID);
	void AddUpdateData(const FInstanceArmourData& inData);
	void AddUpdateData(const FInstanceItemData& inData);
	FInstanceArmourData GetInstancedArmourByContainerID(int32 inContainerInstanceID);
	FString GetContainerInstanceName(int32 containerID);

	static TArray<FString> CleanData(TArray<FString> strings);

	int32 GetNextInstanceItemDataID();
	int32 GetNextInstanceBoxDataID();
	int32 GetNextInstanceArmourDataID();

	int32 GetNextInstanceContainerDataID();
	TArray<FInstanceItemData> GetInstancedItemsForContainer(int32 instanceContainerID);

	TMap<int32, FInstanceItemData>& GetInstancedItems() { return instanceItems; }
	TMap<int32, FInstanceContainerData>& GetInstancedContainers() { return instancedContainers; }
	TMap<int32, FInstanceArmourData>& GetInstancedArmour() { return armourInstances; }
	TMap<int32, FInstanceBoxData>& GetInstancedBoxes() { return boxContainers; }

	TArray<APatrolPath*> paths;
private:
	TMap<int32, FInstanceContainerData> instancedContainers;
	TMap<int32, FInstanceArmourData> armourInstances;
	TMap<int32, FInstanceBoxData> boxContainers;
	TMap<int32, FInstanceItemData> instanceItems;

	UPROPERTY()
		URPGEventManager* eventManager;

	UPROPERTY()
		UFactionManager* factionManager;

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
};