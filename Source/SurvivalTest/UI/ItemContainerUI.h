#pragma once
#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "SurvivalTest/Items/ItemContainer.h"
#include "ItemContainerUI.generated.h"

class UBaseGameInstance;
class UItemUI;
class UInventoryUI;

UCLASS(Blueprintable)
class SURVIVALTEST_API UItemContainerUI : public UUserWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent, Category = "Inventory")
		void GenerateInventory();

	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent, Category = "Inventory")
		void UpdateItem(FInstanceItemData instanceItemData, FItemData itemData);

	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent, Category = "Inventory")
		void RemoveItem(FInstanceItemData instanceItemData);

	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent, Category = "Inventory")
		void AddItem(FInstanceItemData instanceItemData, FItemData itemData);

	UFUNCTION(BlueprintCallable, Category = "Inventory")
		int32 GetColumn();

	UFUNCTION(BlueprintCallable, Category = "Inventory")
		int32 GetNextRowIndex();

	UFUNCTION(BlueprintCallable, Category = "Inventory")
	void GetGridData(int32& row, int32& column);

	UFUNCTION(BlueprintCallable, Category = "Inventory")
		void SetIndex(int32 value) { index = value; }

	UFUNCTION(BlueprintCallable, Category = "Inventory")
		int32 GetRow();

	UFUNCTION(BlueprintCallable, Category = "Inventory")
		void ResetIndex() { index = 0; }

	UFUNCTION(BlueprintCallable, Category = "Inventory")
		void SetNextRowIndex() { index = GetNextRowIndex(); }

	UFUNCTION(BlueprintCallable, Category = "Inventory")
		void IncrementIndex() { index++; }

	UFUNCTION(BlueprintCallable, Category = "Inventory")
		UItemContainer* GetItemContainer() const { return container; }

	UFUNCTION(BlueprintCallable, Category = "Inventory")
	void ItemUpdated(FInstanceItemData inItem, FInstanceItemData oldItem);

	UFUNCTION(BlueprintCallable, Category = "Inventory")
		void SetItemContainer(UItemContainer* inContainer);

	UFUNCTION(BlueprintCallable, Category = "Inventory")
		UBaseGameInstance* GetBaseGameInstance() const { return gameInstance; }

	UFUNCTION(BlueprintCallable, Category = "Inventory")
		FString GetContainerName();

	UFUNCTION(BlueprintCallable, Category = "Inventory")
		void SetBaseGameInstance(UBaseGameInstance* inGameInstance) { gameInstance = inGameInstance; }

	UFUNCTION(BlueprintCallable, Category = "Inventory")
		UItemContainer* GetItemContainerForArmour(FInstanceItemData data);

	UFUNCTION(BlueprintCallable, Category = "Inventory")
	bool IsArmour(FItemData id);

	UFUNCTION(BlueprintCallable, Category = "Inventory")
	UItemUI* GetItemAtSlot(int32 itemSlot, TArray<UWidget*> widgets);

	UFUNCTION()
		void ItemAdded(FInstanceItemData inItem);

	UFUNCTION()
		void ItemRemoved(FInstanceItemData inItem);

private:
	UPROPERTY()
		UInventoryUI* inventory;

	UPROPERTY()
		UItemContainer* container;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Inventory, meta = (AllowPrivateAccess = "true"))
	int32 index;

	UPROPERTY()
		UBaseGameInstance* gameInstance;
};