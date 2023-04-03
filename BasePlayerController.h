#pragma once
#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "Blueprint/UserWidget.h"
#include "BasePlayerController.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnUseItem);

class UInventoryUI;

UCLASS()
class SURVIVALTEST_API ABasePlayerController : public APlayerController
{
	GENERATED_BODY()
public:
	ABasePlayerController();
	virtual void PlayerTick(float DeltaTime) override;
	virtual void OnPossess(APawn* aPawn) override;
	virtual void SetupInputComponent() override;
	void OnPrimaryAction();
	void MoveForward(float Val);
	void MoveRight(float Val);
	void LoadInventories();

	ABaseCharacter* GetBaseCharacter() const { return baseCharacter; }
	void SetBaseCharacter(ABaseCharacter* inBaseCharacter) { this->baseCharacter = inBaseCharacter; }

	UPROPERTY()
		FOnUseItem OnUseItem;
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