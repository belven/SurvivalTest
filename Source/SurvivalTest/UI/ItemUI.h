#pragma once
#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include <SurvivalTest/Items/ItemStructs.h>
#include "ItemUI.generated.h"

class UItemContainer;

UCLASS(Blueprintable)
class SURVIVALTEST_API UItemUI : public UUserWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "Inventory")
	void UpdateItemData(FInstanceItemData inInstanceData, FItemData inItemData, UItemContainer* inItemContainer);

protected:
	virtual FReply NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;

public:
	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent, Category = "Inventory")
	void UpdateItemDetails();

	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent, Category = "Inventory")
		void ClearItem();

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

	UFUNCTION(BlueprintCallable, Category = "Inventory")
		UItemContainer* GetItemContainer() const { return itemContainer; }

	UFUNCTION(BlueprintCallable, Category = "Inventory")
	void SetItemContainer(UItemContainer* inItemContainer) { itemContainer = inItemContainer; }

private:
	UPROPERTY()
		UItemContainer* itemContainer;
	FItemData itemData;
	FInstanceItemData instanceItemData;
};
