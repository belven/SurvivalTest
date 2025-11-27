#pragma once
#include "CoreMinimal.h"
#include "ItemContainerUI.h"
#include "Blueprint/UserWidget.h"
#include "SurvivalTest/Items/ItemContainer.h"
#include "InventoryUI.generated.h"

class UInventory;
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
	void SetupInventory(ABasePlayerController* inController, UBaseGameInstance* inGameInstance);

	UFUNCTION(BlueprintCallable, Category = "Inventory")
	void GenerateInventory_CPP();

	UFUNCTION(BlueprintCallable, Category = "Inventory")
	UItemContainerUI* AddContainerToPanel(UItemContainer* container, UPanelWidget* panel);

	UFUNCTION(BlueprintCallable, Category = "Inventory")
	void RemoveContainerFromPanel(UItemContainer* container, UPanelWidget* panel);

	UFUNCTION(BlueprintCallable, Category = "Inventory")
	void ContainerAdded(UItemContainer* container);

	UFUNCTION(BlueprintCallable, Category = "Inventory")
	void ContainerRemoved(UItemContainer* container);

	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent, Category = "Inventory")
	UPanelWidget* GetNearbyContainerGrid();

	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent, Category = "Inventory")
	UPanelWidget* GetPlayerInventoryPanel();

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
	UInventory* GetPlayerInventory();

	UFUNCTION(BlueprintCallable, Category = "Inventory")
	UItemContainerUI* GetPlayerContainerUI() const
	{
		return playerContainerUI;
	}
protected:
	UPROPERTY(BlueprintReadWrite, Category = "Inventory")
	ABasePlayerController* controller;

	UPROPERTY()
	UBaseGameInstance* gameInstance;

	UPROPERTY()
	UItemContainerUI* playerContainerUI;

	UPROPERTY()
	TMap<int32, UItemContainerUI*> createdItemContainerUI;
};
