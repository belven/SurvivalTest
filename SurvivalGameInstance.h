// Fill out your copyright notice in the Description page of Project Settings.
#pragma once
#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "Items/ItemStructs.h"
#include "Kismet/GameplayStatics.h"
#include "SurvivalGameInstance.generated.h"

#define GameInstance(world) Cast<USurvivalGameInstance>(UGameplayStatics::GetGameInstance(world))

class UProjectileWeaponDataTable;
class UMeleeWeaponDataTable;
class URangedWeaponDataTable;
class UWeaponDataTable;
class UItemDataTable;
class UArmourCSVDataTable;
class UArmourResistanceDataTable;
class ULoadoutTableData;
class UFactionManager;
class URPGEventManager;

UCLASS()
class SURVIVALTEST_API USurvivalGameInstance : public UGameInstance
{
	GENERATED_BODY()
public:
	USurvivalGameInstance();
	
	UFUNCTION(BlueprintCallable)
		void LoadTableFromFile(UCSVTable* table);

	void LoadTableData();

	UFactionManager* GetFactionManager();

	FItemData GetItemData(int32 itemID);

	FWeaponData GetWeaponData(int32 itemID);
	FMeleeWeaponData GetMeleeWeaponData(int32 weaponID);
	FRangedWeaponData GetRangedWeaponData(int32 weaponID);
	FProjectileWeaponData GetProjectileWeaponData(int32 rangedWeaponID);

	FArmourData GetArmourData(int32 itemID);

	FLoadoutData GetLoadoutData(int32 entityID);

	virtual void Init() override;
	URPGEventManager* GetEventManager();

	UItemDataTable* GetItemDataTable();
	UWeaponDataTable* GetWeaponDataTable();
	URangedWeaponDataTable* GetRangedWeaponData();
	UProjectileWeaponDataTable* GetProjectileWeaponData();
	UMeleeWeaponDataTable* GetMeleeWeaponData();
	UArmourCSVDataTable* GetArmourDataTable();
	ULoadoutTableData* GetLoadoutTableData();

	static TArray<FString> CleanData(TArray<FString> strings);

	//TArray<APatrolPath*> paths;
private:
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
		UArmourCSVDataTable* armourDataTable;
	
	UPROPERTY()
		ULoadoutTableData* loadoutTableData;	
};