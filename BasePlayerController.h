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

protected:
	virtual void BeginPlay() override;

public:
	void MoveForward(float Val);
	void MoveRight(float Val);
	void LoadInventories();

	UPROPERTY(BlueprintAssignable, Category = "Interaction")
		FOnUseItem OnUseItem;

	UPROPERTY()
		TSubclassOf<UUserWidget> inventoryWidgetClass;

	UPROPERTY()
		UInventoryUI* inventoryWidget;
};