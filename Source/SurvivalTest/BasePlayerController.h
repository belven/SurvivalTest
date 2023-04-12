#pragma once
#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "Blueprint/UserWidget.h"
#include "Items/ItemStructs.h"
#include "BasePlayerController.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnUseItem);

class UInventoryUI;
class ABaseCharacter;

UCLASS()
class SURVIVALTEST_API ABasePlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	UFUNCTION()
	void ContainersUpdated();

	ABasePlayerController();
	virtual void PlayerTick(float DeltaTime) override;
	virtual void OnPossess(APawn* aPawn) override;
	void OnPrimaryActionReleased();
	void OnPrimaryWeapon();
	void OnSecondaryWeapon();
	void OnSidearm();
	void EquipWeaponAtSlot(int32 slot, EGearType type);
	virtual void SetupInputComponent() override;
	void ShowCursor();
	void OnPrimaryAction();
	void MoveForward(float Val);
	void MoveRight(float Val);
	void LoadInventories();

	ABaseCharacter* GetBaseCharacter() const { return baseCharacter; }
	void SetBaseCharacter(ABaseCharacter* inBaseCharacter) { this->baseCharacter = inBaseCharacter; }

	UPROPERTY()
	FOnUseItem OnUseItem;
	bool performAction;

protected:
	virtual void BeginPlay() override;

private:
	UPROPERTY()
	TSubclassOf<UUserWidget> inventoryWidgetClass;

	UPROPERTY()
	UInventoryUI* inventoryWidget;

	UPROPERTY()
	ABaseCharacter* baseCharacter;
};
