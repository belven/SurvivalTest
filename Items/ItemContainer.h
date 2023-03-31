#pragma once
#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "ItemStructs.h"
#include "ItemContainer.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FItemRemoved, FInventoryItemData, item);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FItemAdded, FInventoryItemData, item);

UCLASS()
class SURVIVALTEST_API UItemContainer : public UObject
{
	GENERATED_BODY()

public:
	UItemContainer();

	UFUNCTION(BlueprintCallable, Category = "Item Container")
		void DataTableChanged();
	FString GetItemName(int32 itemID);
	int32 GetNextInventoryID();
	int32 GetItemStackSize(int32 itemID);

	UFUNCTION(BlueprintCallable, Category = "Item Container")
		TArray<FInventoryItemData>& GetItems() { return items; }

	UFUNCTION(BlueprintCallable, Category = "Item Container")
		void SetItems(TArray<FInventoryItemData> newVal) { items = newVal; }

	bool HasSpace(FInventoryItemData item);
	FInventoryItemData GetExistingItemWithSpace(FInventoryItemData inItem);

	UFUNCTION(BlueprintCallable, Category = "Item Container")
		FInventoryItemData AddItem(FInventoryItemData itemToAdd);

	UFUNCTION(BlueprintCallable, Category = "Item Container")
		bool RemoveItem(FInventoryItemData itemToRemove);

	UFUNCTION(BlueprintCallable, Category = "Item Container")
		int32 GetItemAmount(int32 id);

	UFUNCTION(BlueprintCallable, Category = "Item Container")
		int32 GetMaxItemCount() { return maxItemCount; }

	UFUNCTION(BlueprintCallable, Category = "Item Container")
		void SetMaxItemCount(int32 newVal) { maxItemCount = newVal; }

	UFUNCTION(BlueprintCallable, Category = "Item Container")
		bool HasSpace() { return items.Num() < GetMaxItemCount(); }

	UFUNCTION(BlueprintCallable, Category = "Item Container")
		int32 GetNextEmptySpace();

	void RemoveFilledSlots(TArray<int32>& slots);

	UPROPERTY(BlueprintCallable, Category = "Item Container")
		FItemRemoved OnItemRemoved;

	UPROPERTY(BlueprintCallable, Category = "Item Container")
		FItemAdded OnItemAdded;

	UFUNCTION(BlueprintCallable, Category = "Item Container")
		FString GetOwnerID() const { return ownerID; }

	UFUNCTION(BlueprintCallable, Category = "Item Container")
		void SetOwnerID(FString val) { ownerID = val; }
private:

	UPROPERTY()
		FString ownerID;

	UPROPERTY()
		TArray<FInventoryItemData> items;

	UPROPERTY()
		int32 maxItemCount = 10;
};