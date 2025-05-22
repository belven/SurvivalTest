#pragma once
#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include <SurvivalTest/Items/ItemStructs.h>
#include "ItemUI.generated.h"

class UItemContainer;
class UImage;

UCLASS(Blueprintable)
class SURVIVALTEST_API UItemUI : public UUserWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "Item")
	void UpdateItemData(FInstanceItemData inInstanceData, FItemData inItemData, UItemContainer* inItemContainer);

protected:
	virtual FReply NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;

public:
	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent, Category = "Item")
	void UpdateItemDetails();

	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent, Category = "Item")
	void ClearItem();

	UFUNCTION(BlueprintCallable, Category = "Item")
	FString GetItemAmount();

	UFUNCTION(BlueprintCallable, Category = "Item")
	UTexture2D* GetItemIcon();

	UFUNCTION(BlueprintCallable, Category = "Item")
	void SetImage(UImage* image);

	UFUNCTION(BlueprintCallable, Category = "Item")
	void ClearItemData();

	UFUNCTION(BlueprintCallable, Category = "Item")
	FItemData GetItemData() const { return itemData; }

	UFUNCTION(BlueprintCallable, Category = "Item")
	void SetItemData(FItemData inItemData) { itemData = inItemData; }

	UFUNCTION(BlueprintCallable, Category = "Item")
	FInstanceItemData GetInstanceItemData() const { return instanceItemData; }

	UFUNCTION(BlueprintCallable, Category = "Item")
	void SetInstanceItemData(FInstanceItemData inInstanceItemData) { instanceItemData = inInstanceItemData; }

	UFUNCTION(BlueprintCallable, Category = "Item")
	UItemContainer* GetItemContainer() const { return itemContainer; }

	UFUNCTION(BlueprintCallable, Category = "Item")
	void SetItemContainer(UItemContainer* inItemContainer) { itemContainer = inItemContainer; }

	UFUNCTION(BlueprintCallable, Category = "Item")
	bool IsItemValid();
private:
	UPROPERTY()
	UItemContainer* itemContainer;
	FItemData itemData;
	int32 imageSize = 100;
	FInstanceItemData instanceItemData;
};
