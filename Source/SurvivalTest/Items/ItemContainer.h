#pragma once
#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "ItemStructs.h"
#include "../BaseGameInstance.h"
#include "ItemContainer.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FItemUpdated, FInstanceItemData, newItem, FInstanceItemData, oldItem);

USTRUCT(BlueprintType)
struct FValidSlots
{
	GENERATED_USTRUCT_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory")
	TArray<EGearType> validGear;
};

USTRUCT(BlueprintType)
struct FItemDataPair
{
	GENERATED_USTRUCT_BODY()

public:
	FItemDataPair()
	{
	}

	FItemDataPair(const FInstanceItemData& inIid, const FItemData& inID)
		: id(inID),
		  iid(inIid)
	{
	}

	FItemData id;
	FInstanceItemData iid;
};

UCLASS(Blueprintable)
class SURVIVALTEST_API UItemContainer : public UObject
{
	GENERATED_BODY()

public:
	UItemContainer();

	static UItemContainer* CreateItemContainer(FContainerData inContainerData, FInstanceContainerData inInstanceContainerData, UBaseGameInstance* inGame);

	FString GetItemName(int32 itemID);
	int32 GetNextInstanceItemDataID();
	int32 GetItemStackSize(int32 itemID);
	int32 GetNextEmptySlotForItem(int32 itemID);
	TArray<FInstanceItemData> GetExistingItemsWithSpace(int32 itemID);
	bool CheckForArmourInventory(FInstanceItemData& itemToTransfer);
	void SwapItems(UItemContainer* other, FInstanceItemData& itemToTransfer, const int32 droppedSlot, const FInstanceItemData& originalItemData, int32 maxStackSize, FInstanceItemData& existingItem);
	void MoveItemToSlot(UItemContainer* container, FInstanceItemData& itemToTransfer, const int32 slot, const FInstanceItemData& originalItemData);
	void MoveItemToEmptySlot(UItemContainer* other, FInstanceItemData& itemToTransfer, const FInstanceItemData& originalItemData);
	void FillExistingItems(FInstanceItemData& itemToTransfer, int32 maxStackSize);
	void DropOnExistingItem(UItemContainer* other, FInstanceItemData& itemToTransfer, const int32 droppedSlot, const FInstanceItemData& originalItemData, int32 maxStack, FInstanceItemData& existingItem, EGearType type);
	void AddUpdateItemData(FInstanceItemData& existingItem);
	void UpdateItemData(UItemContainer* container, FInstanceItemData& newItem, const FInstanceItemData& OldItem);
	void RemoveInstanceItem(UItemContainer* other, FInstanceItemData& itemToDelete);
	void FillExistingItemsWithDroppedItem(UItemContainer* other, FInstanceItemData& itemToTransfer, const FInstanceItemData& originalItemData, int32 maxStackSize);
	TArray<int32> GetEmptySlots(bool first = false);
	bool HasSpace();
	bool IsValidForSlot(const int32 slot, const EGearType inType);
	int32 FindNextEmptyValidSlot(EGearType inType);
	void UpdateDebugItemsList();

	UFUNCTION(BlueprintCallable, Category = "Item Container")
	FInstanceItemData& TransferItem(UItemContainer* sourceInventory, FInstanceItemData itemToTransfer, const int32 droppedSlot);

	UFUNCTION(BlueprintCallable, Category = "Item Container")
	TArray<FInstanceItemData> GetItems() { return game->GetInstancedItemsForContainer(instanceContainerData.ID); }

	UFUNCTION(BlueprintCallable, Category = "Item Container")
	bool HasSpace(FInstanceItemData item);

	UFUNCTION(BlueprintCallable, Category = "Item Container")
	FInstanceItemData GetInstanceItemAtSlot(int32 slot);

	UFUNCTION(BlueprintCallable, Category = "Item Container")
	FInstanceItemData& AddItem(FInstanceItemData& itemToAdd, TArray<int32>& ids);
	
	UFUNCTION(BlueprintCallable, Category = "Item Container")
	FInstanceItemData RemoveItem(FInstanceItemData itemToRemove);

	/**
 *Returns the total amount of items for the given item ID
 *
 *@param itemID the ID of the item type to look for
 *
 *@return The amount of items found, if any, of the type of item
 */
	UFUNCTION(BlueprintCallable, Category = "Item Container")
	int32 GetItemAmount(int32 itemID);

	UFUNCTION(BlueprintCallable, Category = "Item Container")
	int32 GetMaxItemCount() { return containerData.slots; }
	
	UFUNCTION(BlueprintCallable, Category = "Item Container")
	int32 GetNextEmptySlot();
	int32 GetContainerInstanceID();

	UFUNCTION(BlueprintCallable, Category = "Item Container")
	bool SplitItem(FInstanceItemData& itemToSplit);
	
	UPROPERTY(BlueprintCallable, Category = "Item Container")
		FItemUpdated OnItemUpdated;
	
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
		UBaseGameInstance* GetGame();

	void AddValidSlot(EGearType type, const int32 slot);
protected:
	UPROPERTY()
	TMap<int32, FValidSlots> validSlots;

	UPROPERTY()
	FContainerData containerData;

	UPROPERTY()
	UBaseGameInstance* game = NULL;

	UPROPERTY()
	FInstanceContainerData instanceContainerData;

	// Debug helper data TODO find a better way to do this
	TArray<FItemDataPair> lastUpdatedItems;
};
