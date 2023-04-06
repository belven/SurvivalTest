#pragma once
#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include <SurvivalTest/Items/ItemStructs.h>
#include "ItemUI.generated.h"

UCLASS()
class SURVIVALTEST_API UItemUI : public UUserWidget
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintCallable, Category = "Inventory")
		void UpdateItemData(FInstanceItemData inInstanceData, FItemData inItemData);

	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent, Category = "Inventory")
		void UpdateItemDetails();

	UFUNCTION(BlueprintCallable, Category = "Inventory")
		UTexture2D* GetItemIcon();

		UFUNCTION(BlueprintCallable, Category = "Inventory")
		FItemData GetItemData() const { return itemData; }

	UFUNCTION(BlueprintCallable, Category = "Inventory")
		void SetItemData(FItemData inItemData) { itemData = inItemData; }

	UFUNCTION(BlueprintCallable, Category = "Inventory")
		FInstanceItemData GetInstanceItemData() const { return instanceItemData; }

	UFUNCTION(BlueprintCallable, Category = "Inventory")
		void SetInstanceItemData(FInstanceItemData inInstanceItemData) { instanceItemData = inInstanceItemData; }

private:
		FItemData itemData;
		FInstanceItemData instanceItemData;

};
