#pragma once
#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "SurvivalTest/Items/ItemStructs.h"
#include "CombatComponent.generated.h"

class ABasePlayerController;
class ABaseAIController;
class UWeapon;
class UReloadTask;
class UProjectileWeapon;
class UEquipmentSwapTask;
class UBaseGameInstance;
class ABaseCharacter;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnWeaponEquipped, UWeapon*, oldWeapon, UWeapon*, newWeapon);

DECLARE_LOG_CATEGORY_EXTERN(CombatComponentLog, Warning, All)

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class SURVIVALTEST_API UCombatComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UCombatComponent();

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	static UCombatComponent* CreateCombatComponent(ABaseAIController* controller, ABaseCharacter* character);
	static UCombatComponent* CreateCombatComponent(ABasePlayerController* controller, ABaseCharacter* character);

	UFUNCTION()
	void OnUIStateChanged(bool state);

	UFUNCTION()
	void OutOfAmmo();

	UFUNCTION()
	void WeaponReady();

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

	UPROPERTY()
	FOnWeaponEquipped OnWeaponEquipped;

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

private:
	static UCombatComponent* CreateCombatComponent_INTERNAL(AController* controller, ABaseCharacter* character);

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
