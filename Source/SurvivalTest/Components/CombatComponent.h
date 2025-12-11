// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "SurvivalTest/Items/ItemStructs.h"
#include "CombatComponent.generated.h"


class UWeapon;
class UReloadTask;
class UProjectileWeapon;
class UEquipmentSwapTask;
class UBaseGameInstance;
class ABaseCharacter;


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class SURVIVALTEST_API UCombatComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UCombatComponent();

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	static UCombatComponent* CreateCombatComponent(AController* controller, ABaseCharacter* character);

	UFUNCTION()
	void OnUIStateChanged(bool state);

	UFUNCTION()
	void OutOfAmmo();

	UFUNCTION()
	void ReloadComplete();

	UFUNCTION()
	void WeaponEquipped(UWeapon* oldWeapon);

	UFUNCTION()
	void Reload();

	UFUNCTION()
	void OnPrimaryActionReleased();

	UFUNCTION()
	void OnPrimaryWeapon();

	UFUNCTION()
	void OnSecondaryWeapon();

	UFUNCTION()
	void OnSidearm();

	UFUNCTION()
	void OnPrimaryAction();
	void EquipWeaponAtSlot(int32 slot, EGearType type);

	ABaseCharacter* GetBaseCharacter() const { return baseCharacter; }
	void SetBaseCharacter(ABaseCharacter* inBaseCharacter) { this->baseCharacter = inBaseCharacter; }

	bool IsUIOpen() const
	{
		return uiState;
	}

	void SetUIState(bool inUIState)
	{
		uiState = inUIState;
	}

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

private:
	UPROPERTY()
	UEquipmentSwapTask* equipmentSwapTask;

	UPROPERTY()
	UReloadTask* reloadTask;

	bool uiState = false;
	float RotateValue;
	bool performAction;
	bool useEquipment;
	bool isReloading;

	UPROPERTY()
	UProjectileWeapon* rangedWeapon;

	UPROPERTY()
	ABaseCharacter* baseCharacter;

	UPROPERTY()
	UBaseGameInstance* baseGameInstance;

	UPROPERTY()
	TObjectPtr<APlayerCameraManager> PlayerCameraManager;
};
