// Fill out your copyright notice in the Description page of Project Settings.
#pragma once
#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "Items/ItemStructs.h"
#include "Kismet/GameplayStatics.h"
#include "Tables/Items/InstanceItemDataTable.h"
#include "Tables/TableManager.h"
#include "BaseGameInstance.generated.h"

#define GameInstance(world) Cast<UBaseGameInstance>(UGameplayStatics::GetGameInstance(world))
#define mGameInstance() Cast<UBaseGameInstance>(UGameplayStatics::GetGameInstance(GetWorld()))

class UTableManager;
class UFactionManager;
class URPGEventManager;
class UContainerTableData;
class APatrolPath;
class AMainGrid;
class UConsumableTableData;

UCLASS()
class SURVIVALTEST_API UBaseGameInstance : public UGameInstance
{
	GENERATED_BODY()
public:
	virtual void Init() override;

	UFUNCTION(BlueprintCallable)
		FItemData GetItemData(int32 itemID);

	void AddUpdateData(const FInstanceArmourData& inData);
	void AddUpdateData(const FInstanceItemData& inData);

	TArray<FInstanceItemData> GetInstancedItemsForContainer(int32 instanceContainerID);
	FInstanceArmourData GetInstanceArmourDataByInstanceItemID(int32 InstanceItemID);

	FWeaponData GetWeaponData(int32 itemID);
	FMeleeWeaponData GetMeleeWeaponData(int32 weaponID);
	FRangedWeaponData GetRangedWeaponData(int32 weaponID);
	FProjectileWeaponData GetProjectileWeaponData(int32 rangedWeaponID);
	FArmourData GetArmourData(int32 armourID);
	FContainerData GetContainerDataName(FString containerName);
	FContainerData GetContainerDataByID(int32 containerID);	
	FArmourData GetArmourDataByItemID(int32 itemID);
	FLoadoutData GetLoadoutData(FString loadoutName);
	FConsumableData GetConsumableData(int32 itemID);
	
	EGearType GetGearTypeForItem(int32 itemID);
	FInstanceArmourData GetInstancedArmourByContainerID(int32 inContainerInstanceID);
	FString GetContainerInstanceName(int32 containerID);
	
	int32 GetNextInstanceItemDataID();
	int32 GetNextBoxID();
	int32 GetNextInstanceBoxDataID();
	int32 GetNextInstanceArmourDataID();
	int32 GetNextInstanceContainerDataID();

	TMap<int32, FInstanceItemData>& GetInstancedItems() { return tableManager->GetInstanceItemDataTable()->GetData(); }
	TMap<int32, FInstanceContainerData>& GetInstancedContainers() { return instancedContainers; }
	TMap<int32, FInstanceArmourData>& GetInstancedArmour() { return armourInstances; }
	TMap<int32, FInstanceBoxData>& GetInstancedBoxes() { return boxContainers; }

	TArray<APatrolPath*> paths;

	UPROPERTY()
	AMainGrid* grid;

	URPGEventManager* GetEventManager();
	UTableManager* GetTableManager();
	UFactionManager* GetFactionManager();
private:

	// TODO Make these use data tables!
	TMap<int32, FInstanceContainerData> instancedContainers;
	TMap<int32, FInstanceArmourData> armourInstances;
	TMap<int32, FInstanceBoxData> boxContainers;

	UPROPERTY()
		URPGEventManager* eventManager;

	UPROPERTY()
		UFactionManager* factionManager;

	UPROPERTY()
		UTableManager* tableManager;
	};