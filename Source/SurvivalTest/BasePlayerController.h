#pragma once
#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "Blueprint/UserWidget.h"
#include "Components/TimelineComponent.h"
#include "Items/ItemStructs.h"
#include "BasePlayerController.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnUseItem);

class UInventoryUI;
class ABaseCharacter;
class UTimelineComponent;
class UHUDUI;

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

	virtual void PlayerTick(float DeltaTime) override;
	virtual void OnPossess(APawn* aPawn) override;

	UFUNCTION()
	void Reload();

	bool HasAmmoForWeapon();
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
	bool performAction;
	
	virtual void BeginPlay() override;

private:
	float RotateValue;
	float CurveFloatValue;
	float TimelineValue;
	int32 leanDirection = 0;

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
