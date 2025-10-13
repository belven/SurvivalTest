#pragma once
#include "CoreMinimal.h"
#include "BaseBuilding/BuildingPart.h"
#include "GameFramework/PlayerController.h"
#include "Blueprint/UserWidget.h"
#include "Components/TimelineComponent.h"
#include "Items/ItemStructs.h"
#include "Items/Weapon.h"
#include "BasePlayerController.generated.h"

class UEquipmentSwapTask;
class UReloadTask;
class UProjectileWeapon;
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnUseItem);

class UInventoryUI;
class ABaseCharacter;
class UTimelineComponent;
class UHUDUI;

UENUM(BlueprintType)
enum class EOneDirection : uint8
{
	North,
	NorthEast,
	NorthWest,
	East,
	West,
	South,
	SouthEast,
	SouthWest,
	End
};

UCLASS()
class SURVIVALTEST_API ABasePlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	ABasePlayerController();

	UFUNCTION()
	void TimelineCallback();

	UFUNCTION()
	void TimelineFinishedCallback();

	UFUNCTION()
		void ContainersUpdated();

	UFUNCTION()
		void EnemyHit(ABaseCharacter* enemy);

	virtual void PlayerTick(float DeltaTime) override;
	virtual void OnPossess(APawn* aPawn) override;

	UFUNCTION()
	void OutOfAmmo();

	UFUNCTION()
	void ReloadComplete();

	UFUNCTION()
		void WeaponEquipped(UWeapon* oldWeapon);

	UFUNCTION()
	void Reload();

	UFUNCTION()
	void Sprint();

	UFUNCTION()
	void Build();
	void CreateBuildingPart(ABuildingPart* bp, EOneDirection direction);
	FVector MoveVectorByDirection(const FVector& centerLocation, const FRotator& currentRotation, float distance, EOneDirection direction);

	void OnPrimaryActionReleased();
	void OnPrimaryWeapon();
	void OnSecondaryWeapon();
	void OnSidearm();
	void EquipWeaponAtSlot(int32 slot, EGearType type);
	void LeanRight();
	void LeanLeft();
	void LeanCenter();

	virtual void SetupInputComponent() override;
	void ShowCursor();
	void OnPrimaryAction();
	void MoveForward(float Val);
	void MoveRight(float Val);
	void OpenInventory();
	void CloseInventory();
	void LoadInventories();

	ABaseCharacter* GetBaseCharacter() const { return baseCharacter; }
	void SetBaseCharacter(ABaseCharacter* inBaseCharacter) { this->baseCharacter = inBaseCharacter; }

	UPROPERTY()
	FOnUseItem OnUseItem;

	virtual void BeginPlay() override;

private:
	float RotateValue;
	float CurveFloatValue;
	float TimelineValue;
	int32 leanDirection = 0;
	bool performAction;
	bool useEquipment;
	bool isReloading;

	UPROPERTY()
	UReloadTask* reloadTask;

	UPROPERTY()
	UEquipmentSwapTask* equipmentSwapTask;

	UFUNCTION()
	void CharacterDied(ABaseCharacter* deadCharacter);

	UPROPERTY()
	UProjectileWeapon* rangedWeapon;

	//UPROPERTY()
	//UTimelineComponent* leanTimeline;

	FTimeline leanTimeline;

	UPROPERTY()
	UCurveFloat* leanCurve;

	UPROPERTY()
	TSubclassOf<UUserWidget> inventoryWidgetClass;

	UPROPERTY()
		TSubclassOf<UUserWidget> mainHUDClass;

	UPROPERTY()
	UHUDUI* mainHUD;

	UPROPERTY()
	UInventoryUI* inventoryWidget;

	UPROPERTY()
	ABaseCharacter* baseCharacter;
};
