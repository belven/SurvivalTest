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
	int32 GetNextItemID();
	int32 GetItemStackSize(int32 itemID);
	int32 GetNextEmptySlotForItem(int32 itemID);
	TArray<int32> GetEmptySlots();
	bool HasSpace();
	void RemoveFilledSlots(TArray<int32>& slots);
	bool IsValidForSlot(int32 slot, EGearType inType);
	int32 FindNextEmptyValidSlot(EGearType inType);
	void UpdateDebugItemsList();

	UFUNCTION(BlueprintCallable, Category = "Item Container")
	void TransferItem(UItemContainer* other, FInstanceItemData data, int32 droppedSlot);

	UFUNCTION(BlueprintCallable, Category = "Item Container")
	TArray<FInstanceItemData> GetItems() { return game->GetInstancedItemsForContainer(instanceContainerData.ID); }

	UFUNCTION(BlueprintCallable, Category = "Item Container")
	bool HasSpace(FInstanceItemData item);

	UFUNCTION(BlueprintCallable, Category = "Item Container")
	FInstanceItemData GetItemAtSlot(int32 slot);

	UFUNCTION(BlueprintCallable, Category = "Item Container")
	FInstanceItemData GetExistingItemWithSpace(FInstanceItemData inItem);

	UFUNCTION(BlueprintCallable, Category = "Item Container")
	FInstanceItemData AddItem(FInstanceItemData itemToAdd, TArray<int32>& ids);
	
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

private:
	UPROPERTY()
	TMap<EGearType, FValidSlots> validSlots;

	UPROPERTY()
	FContainerData containerData;

	UPROPERTY()
	UBaseGameInstance* game;

	UPROPERTY()
	FInstanceContainerData instanceContainerData;

	// Debug helper data TODO find a better way to do this
	TArray<FItemDataPair> lastUpdatedItems;
};
