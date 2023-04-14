#pragma once
#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/GridSlot.h"
#include "SurvivalTest/Items/ItemContainer.h"
#include "ItemContainerUI.generated.h"

class UBaseGameInstance;
class UItemUI;

UCLASS(Blueprintable)
class SURVIVALTEST_API UItemContainerUI : public UUserWidget
{
	GENERATED_BODY()
		
public:
	UItemContainerUI(const FObjectInitializer& ObjectInitializer);

	UFUNCTION(BlueprintCallable, Category = "Inventory")
		void GenerateInventory();

	//UFUNCTION(BlueprintCallable, BlueprintImplementableEvent, Category = "Inventory")
		//void SetInventoryText(FString name);

	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent, Category = "Inventory")
		UGridSlot* AddItemWidgetToGrid(UUserWidget* widget, int32 row, int32 column);
	
	UFUNCTION(BlueprintCallable, Category = "Inventory")
		int32 GetCurrentColumn();

	UFUNCTION(BlueprintCallable, Category = "Inventory")
		int32 GetNextRowIndex();

	UFUNCTION(BlueprintCallable, Category = "Inventory")
	void GetGridData(int32& row, int32& column);

	UFUNCTION(BlueprintCallable, Category = "Inventory")
		void SetIndex(int32 value) { index = value; }

	UFUNCTION(BlueprintCallable, Category = "Inventory")
		int32 GetCurrentRow();

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

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Inventory")
		UGridPanel* GetItemsGrid();
	
		UGridPanel* GetItemsGrid_Implementation() { return itemsGrid; }

	UFUNCTION(BlueprintCallable, Category = "Inventory")
	void AddItemToGrid(FInstanceItemData iid);

	UFUNCTION(BlueprintCallable, Category = "Inventory")
	void AddArmourUI(FInstanceItemData iid, UItemUI* itemUI);

	UFUNCTION(BlueprintCallable, Category = "Inventory")
	void SetItemsGrid(UGridPanel* inItemsGrid)
	{
		itemsGrid = inItemsGrid;
	}

private:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
		TSubclassOf<UUserWidget> itemWidgetClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	TSubclassOf<UUserWidget> itemContainerWidgetClass;
	
	UPROPERTY()
		UItemContainer* container;

	UPROPERTY()
		UGridPanel* itemsGrid;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Inventory, meta = (AllowPrivateAccess = "true"))
	int32 index;

	UPROPERTY()
		UBaseGameInstance* gameInstance;
};