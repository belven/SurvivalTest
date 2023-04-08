#pragma once
#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "SurvivalTest/Items/ItemContainer.h"
#include "InventoryUI.generated.h"

class UBaseGameInstance;
class ABasePlayerController;

UCLASS(Blueprintable)
class SURVIVALTEST_API UInventoryUI : public UUserWidget
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent, Category = "Inventory")
		void GenerateInventory();
	
	UFUNCTION(BlueprintCallable, Category = "Inventory")
		ABasePlayerController* GetController() const { return controller; }
	UFUNCTION(BlueprintCallable, Category = "Inventory")
		void SetController(ABasePlayerController* inController) { this->controller = inController; }

	UFUNCTION(BlueprintCallable, Category = "Inventory")
		UBaseGameInstance* GetBaseGameInstance() const { return gameInstance; }

	UFUNCTION(BlueprintCallable, Category = "Inventory")
		void SetBaseGameInstance(UBaseGameInstance* inGameInstance) { this->gameInstance = inGameInstance; }

	UFUNCTION(BlueprintCallable, Category = "Inventory")
		TArray<UItemContainer*> GetContainers();

	UFUNCTION(BlueprintCallable, Category = "Inventory")
		UItemContainer* GetPlayerInventory();
private:
	UPROPERTY()
		ABasePlayerController* controller;

	UPROPERTY()
		UBaseGameInstance* gameInstance;
};
