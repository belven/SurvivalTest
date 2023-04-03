#pragma once
#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "ItemStructs.h"
#include "../BaseGameInstance.h"
#include "ItemContainer.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FItemRemoved, FInstanceItemData, item);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FItemAdded, FInstanceItemData, item);

UCLASS(Blueprintable)
class SURVIVALTEST_API UItemContainer : public UObject
{
	GENERATED_BODY()

public:
	UItemContainer();

	static UItemContainer* CreateItemContainer(FContainerData inContainerData, FInstanceContainerData inInstanceContainerData);

	UFUNCTION(BlueprintCallable, Category = "Item Container")
		void DataTableChanged();
	FString GetItemName(int32 itemID);
	int32 GetNextInventoryID();
	int32 GetItemStackSize(int32 itemID);

	UFUNCTION(BlueprintCallable, Category = "Item Container")
		TArray<FInstanceItemData>& GetItems() { return items; }

	UFUNCTION(BlueprintCallable, Category = "Item Container")
		void SetItems(TArray<FInstanceItemData> newVal) { items = newVal; }

	bool HasSpace(FInstanceItemData item);
	FInstanceItemData GetExistingItemWithSpace(FInstanceItemData inItem);

	UFUNCTION(BlueprintCallable, Category = "Item Container")
		FInstanceItemData AddItem(FInstanceItemData itemToAdd);

	UFUNCTION(BlueprintCallable, Category = "Item Container")
		bool RemoveItem(FInstanceItemData itemToRemove);

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
		int32 GetOwnerID() const { return containerData.ID; }

	UFUNCTION(BlueprintCallable, Category = "Item Container")
		FContainerData GetContainerData() const { return containerData; }

	FInstanceContainerData GetInstanceContainerData() const { return instanceContainerData; }

	void SetInstanceContainerData(FInstanceContainerData inInstanceContainerData) { this->instanceContainerData = inInstanceContainerData; }

	UFUNCTION(BlueprintCallable, Category = "Item Container")
		void SetContainerData(FContainerData inContainerData) { this->containerData = inContainerData; }

private:
	UPROPERTY()
		FContainerData containerData;

	UPROPERTY()
		FInstanceContainerData instanceContainerData;

	UPROPERTY()
		TArray<FInstanceItemData> items;

	UPROPERTY()
		int32 maxItemCount = 10;

	UBaseGameInstance* GetGame();
};