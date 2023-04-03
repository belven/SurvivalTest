#pragma once
#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "InventoryUI.generated.h"

UCLASS()
class SURVIVALTEST_API UInventoryUI : public UUserWidget
{
	GENERATED_BODY()
public:
		UFUNCTION(BlueprintCallable, BlueprintImplementableEvent, Category = "Inventory")
		void GenerateInventory();
	
};
