#pragma once
#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "SurvivalTest/Items/ItemContainer.h"
#include "ItemContainerUI.generated.h"

class UBaseGameInstance;

UCLASS()
class SURVIVALTEST_API UItemContainerUI : public UUserWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent, Category = "Inventory")
		void GenerateInventory();

	UFUNCTION(BlueprintCallable, Category = "Inventory")
		UItemContainer* GetItemContainer() const { return container; }

	UFUNCTION(BlueprintCallable, Category = "Inventory")
		void SetItemContainer(UItemContainer* inContainer) { container = inContainer; }

	UFUNCTION(BlueprintCallable, Category = "Inventory")
		UBaseGameInstance* GetBaseGameInstance() const { return gameInstance; }

	UFUNCTION(BlueprintCallable, Category = "Inventory")
		void SetBaseGameInstance(UBaseGameInstance* inGameInstance) { gameInstance = inGameInstance; }

private:
	UPROPERTY()
		UItemContainer* container;

	UPROPERTY()
		UBaseGameInstance* gameInstance;
};