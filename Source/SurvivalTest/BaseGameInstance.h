// Fill out your copyright notice in the Description page of Project Settings.
#pragma once
#include "CoreMinimal.h"
#include "MainLight.h"
#include "Engine/GameInstance.h"
#include "Items/ItemStructs.h"
#include "Kismet/GameplayStatics.h"
#include "Tables/Items/InstanceItemDataTable.h"
#include "Tables/TableManager.h"
#include "BaseGameInstance.generated.h"

#define GameInstance(world) Cast<UBaseGameInstance>(UGameplayStatics::GetGameInstance(world))
#define mGameInstance() Cast<UBaseGameInstance>(UGameplayStatics::GetGameInstance(GetWorld()))
#define mTable() GetTableManager()

class UTableManager;
class UFactionManager;
class URPGEventManager;
class UContainerTableData;
class APatrolPath;
class AMainGrid;
class UConsumableTableData;
class UMissionManager;

UCLASS()
class SURVIVALTEST_API UBaseGameInstance : public UGameInstance
{
	GENERATED_BODY()

public:
	virtual void Init() override;
	virtual void Shutdown() override;
	
	UFUNCTION(BlueprintCallable)
	FItemData GetItemData(int32 itemID) { return mTable()->GetItemData(itemID); }

	void AddUpdateData(const FInstanceArmourData& inData) { mTable()->AddUpdateData(inData); }
	void AddUpdateData(const FInstanceWeaponData& inData) { mTable()->AddUpdateData(inData); }
	void AddUpdateData(const FInstanceItemData& inData) { mTable()->AddUpdateData(inData); }
	TArray<FInstanceItemData> GetInstancedItemsForContainer(int32 instanceContainerID) { return mTable()->GetInstancedItemsForContainer(instanceContainerID); }
	FInstanceArmourData GetInstanceArmourDataByInstanceItemID(int32 InstanceItemID) { return  mTable()->GetInstanceArmourDataByInstanceItemID(InstanceItemID); }

	FWeaponData GetWeaponData(int32 itemID) { return mTable()->GetWeaponData(itemID); }
	FMeleeWeaponData GetMeleeWeaponData(int32 weaponID) { return mTable()->GetMeleeWeaponData(weaponID); }
	FRangedWeaponData GetRangedWeaponData(int32 weaponID) { return mTable()->GetRangedWeaponData(weaponID); }
	FProjectileWeaponData GetProjectileWeaponData(int32 rangedWeaponID) { return mTable()->GetProjectileWeaponData(rangedWeaponID); }
	FArmourData GetArmourData(int32 armourID) { return mTable()->GetArmourData(armourID); }
	FContainerData GetContainerDataName(FString containerName) { return mTable()->GetContainerDataName(containerName); }
	FContainerData GetContainerDataByID(int32 containerID) { return mTable()->GetContainerDataByID(containerID); }
	FArmourData GetArmourDataByItemID(int32 itemID) { return mTable()->GetArmourDataByItemID(itemID); }
	FLoadoutData GetLoadoutData(FString loadoutName) { return mTable()->GetLoadoutData(loadoutName); }
	FConsumableData GetConsumableData(int32 itemID) { return mTable()->GetConsumableData(itemID); }
	FInstanceWeaponData GetInstanceWeaponDataByInstanceItemID(int32 instanceItemID) { return mTable()->GetInstanceWeaponDataByInstanceItemID(instanceItemID); }
	FInstanceArmourData GetInstancedArmourByContainerID(int32 inContainerInstanceID) { return mTable()->GetInstancedArmourByContainerID(inContainerInstanceID); }
	FString GetContainerInstanceName(int32 containerID) { return mTable()->GetContainerInstanceName(containerID); }

	EGearType GetGearTypeForItem(int32 itemID) { return mTable()->GetGearTypeForItem(itemID); }

	int32 GetNextInstanceItemDataID();
	int32 GetNextBoxID();
	int32 GetNextInstanceBoxDataID();
	int32 GetNextInstanceArmourDataID();
	int32 GetNextInstanceContainerDataID();
	int32 GetNextInstanceWeaponDataID();

	TMap<int32, FInstanceItemData>& GetInstancedItems() { return mTable()->GetInstanceItemDataTable()->GetData(); }
	TMap<int32, FInstanceContainerData>& GetInstancedContainers() { return mTable()->GetInstancedContainers(); }
	TMap<int32, FInstanceArmourData>& GetInstancedArmour() { return mTable()->GetInstancedArmour(); }
	TMap<int32, FInstanceBoxData>& GetInstancedBoxes() { return mTable()->GetInstancedBoxes(); }
	void SetMainLight(AMainLight* inMainLight) { mainLight = inMainLight; }

	AMainLight* GetMainLight() const { return mainLight; }

	TArray<APatrolPath*> paths;

	UPROPERTY()
	AMainGrid* grid;

	URPGEventManager* GetEventManager();
	UTableManager* GetTableManager();
	UFactionManager* GetFactionManager();
	UMissionManager* GetMissionManager();

private:

	UPROPERTY()
	UMissionManager* missionManager;

	UPROPERTY()
	AMainLight* mainLight;

	UPROPERTY()
	URPGEventManager* eventManager;

	UPROPERTY()
	UFactionManager* factionManager;

	UPROPERTY()
	UTableManager* tableManager;
};
