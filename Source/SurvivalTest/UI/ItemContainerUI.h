#pragma once
#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "SurvivalTest/Items/ItemContainer.h"
#include "ItemContainerUI.generated.h"

class UBaseGameInstance;

UCLASS(Blueprintable)
class SURVIVALTEST_API UItemContainerUI : public UUserWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent, Category = "Inventory")
		void GenerateInventory();

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
		void SetItemContainer(UItemContainer* inContainer);

	UFUNCTION(BlueprintCallable, Category = "Inventory")
		UBaseGameInstance* GetBaseGameInstance() const { return gameInstance; }

	UFUNCTION(BlueprintCallable, Category = "Inventory")
		FString GetContainerName();

	UFUNCTION(BlueprintCallable, Category = "Inventory")
		void SetBaseGameInstance(UBaseGameInstance* inGameInstance) { gameInstance = inGameInstance; }

	UFUNCTION(BlueprintCallable, Category = "Inventory")
		UItemContainer* GetItemContainerForArmour(FInstanceItemData data);

	UFUNCTION()
		void ItemAdded(FInstanceItemData inItem);

	UFUNCTION()
		void ItemRemoved(FInstanceItemData inItem);

	UFUNCTION(BlueprintCallable, Category = "Inventory")
	UGridSlot* AddToGrid(UUserWidget* widget);

	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent, Category = "Inventory")
		UGridPanel* GetItemsGrid();
	
	UFUNCTION(BlueprintCallable, Category = "Inventory")
	void AddItemToGrid(FInstanceItemData iid);

	UFUNCTION(BlueprintCallable, Category = "Inventory")
	void AddArmourUI(FInstanceItemData iid, UItemUI* itemUI);
	
private:
	UPROPERTY()
		UItemContainer* container;

	UPROPERTY()
		UGridPanel* itemsGrid;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Inventory, meta = (AllowPrivateAccess = "true"))
	int32 index;

	UPROPERTY()
		UBaseGameInstance* gameInstance;
};