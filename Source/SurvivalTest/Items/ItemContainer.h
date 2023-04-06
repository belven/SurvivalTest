#pragma once
#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "ItemStructs.h"
#include "../BaseGameInstance.h"
#include "ItemContainer.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FItemRemoved, FInstanceItemData, item);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FItemAdded, FInstanceItemData, item);

USTRUCT(BlueprintType)
struct FValidSlots
{
	GENERATED_USTRUCT_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory")
	TArray<int32> validSlots;
};

UCLASS(Blueprintable)
class SURVIVALTEST_API UItemContainer : public UObject
{
	GENERATED_BODY()

public:
	UItemContainer();

	static UItemContainer* CreateItemContainer(FContainerData inContainerData, FInstanceContainerData inInstanceContainerData, UBaseGameInstance* inGame);

	UFUNCTION(BlueprintCallable, Category = "Item Container")
	void DataTableChanged();
	FString GetItemName(int32 itemID);
	int32 GetNextItemID();
	int32 GetItemStackSize(int32 itemID);

	UFUNCTION(BlueprintCallable, Category = "Item Container")
	TArray<FInstanceItemData> GetItems() { return game->GetInstancedItemsForContainer(instanceContainerData.ID); }

	UFUNCTION(BlueprintCallable, Category = "Item Container")
	bool HasSpace(FInstanceItemData item);

	UFUNCTION(BlueprintCallable, Category = "Item Container")
	FInstanceItemData GetExistingItemWithSpace(FInstanceItemData inItem);
	void TransferItem(UItemContainer other, FInstanceItemData data);
	int32 GetNextSlotForItem(int32 itemID);

	UFUNCTION(BlueprintCallable, Category = "Item Container")
	FInstanceItemData AddItem(FInstanceItemData itemToAdd, TArray<int32>& ids);

	TArray<int32> GetEmptySlots();

	UFUNCTION(BlueprintCallable, Category = "Item Container")
	bool RemoveItem(FInstanceItemData itemToRemove);

	UFUNCTION(BlueprintCallable, Category = "Item Container")
	int32 GetItemAmount(int32 id);

	UFUNCTION(BlueprintCallable, Category = "Item Container")
	int32 GetMaxItemCount() { return maxItemCount; }

	UFUNCTION(BlueprintCallable, Category = "Item Container")
	void SetMaxItemCount(int32 newVal) { maxItemCount = newVal; }

	bool HasSpace();

	UFUNCTION(BlueprintCallable, Category = "Item Container")
	int32 GetNextEmptySlot();

	void RemoveFilledSlots(TArray<int32>& slots);

	UPROPERTY(BlueprintCallable, Category = "Item Container")
	FItemRemoved OnItemRemoved;

	UPROPERTY(BlueprintCallable, Category = "Item Container")
	FItemAdded OnItemAdded;

	UFUNCTION(BlueprintCallable, Category = "Item Container")
	int32 GetOwnerID() const { return containerData.ID; }

	UFUNCTION(BlueprintCallable, Category = "Item Container")
	FContainerData GetContainerData() const { return containerData; }

	UFUNCTION(BlueprintCallable, Category = "Item Container")
	FInstanceContainerData GetInstanceContainerData() const { return instanceContainerData; }

	UFUNCTION(BlueprintCallable, Category = "Item Container")
	void SetInstanceContainerData(FInstanceContainerData inInstanceContainerData) { this->instanceContainerData = inInstanceContainerData; }

	UFUNCTION(BlueprintCallable, Category = "Item Container")
	void SetContainerData(FContainerData inContainerData) { this->containerData = inContainerData; }

	UFUNCTION(BlueprintCallable, Category = "Item Container")
	UBaseGameInstance* GetGame() { return game; }

	bool IsValidForSlot(int32 slot, EGearType inType);
	int32 FindNextEmptyValidSlot(EGearType inType);

private:
	UPROPERTY()
	TMap<EGearType, FValidSlots> validSlots;

	UPROPERTY()
	FContainerData containerData;

	UPROPERTY()
	UBaseGameInstance* game;

	UPROPERTY()
	FInstanceContainerData instanceContainerData;

	UPROPERTY()
	int32 maxItemCount = 10;
};
