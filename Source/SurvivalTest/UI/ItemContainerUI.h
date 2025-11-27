#pragma once
#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/GridPanel.h"
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
	static const int itemsPerRow;

	UFUNCTION(BlueprintCallable, Category = "Inventory")
	static UItemContainerUI* CreateOrGetItemContainerUI(APlayerController* controller, UItemContainer* inContainer, UBaseGameInstance* inGameInstance);

	UFUNCTION(BlueprintCallable, Category = "Inventory")
	void SetupItemContainerUI(UItemContainer* inContainer, UBaseGameInstance* inGameInstance);

	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent, Category = "Inventory")
	void SetInventoryName();

	UFUNCTION(BlueprintCallable, Category = "Inventory")
	void AddItemToGrid(FInstanceItemData iid, int32 index);

	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent, Category = "Inventory")
	void ContainerRemovedFromUI();

	UFUNCTION(BlueprintImplementableEvent, Category = "Inventory")
	UGridPanel* GetItemGrid();

	UFUNCTION(BlueprintCallable, Category = "Inventory")
	void GenerateInventory();

	UFUNCTION(BlueprintCallable, Category = "Inventory")
	int32 GetColumn(int32 index);

	UFUNCTION(BlueprintCallable, Category = "Inventory")
	int32 GetRow(int32 index);

	UFUNCTION(BlueprintCallable, Category = "Inventory")
	UItemContainer* GetItemContainer() const { return container; }

	UFUNCTION(BlueprintCallable, Category = "Inventory")
	void ItemUpdated(const FInstanceItemData& newItem, const FInstanceItemData& oldItem);

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
	UItemUI* GetItemAtSlot(int32 itemSlot);

	UFUNCTION(BlueprintCallable, Category = "Inventory")
	FInstanceItemData GetBlankInstanceItemData(int32 containerSlot);

	UFUNCTION(BlueprintCallable, Category = "Inventory")
	void UpdateItemUI(const FInstanceItemData& newItem);

	UFUNCTION(BlueprintCallable, Category = "Inventory")
	TMap<int32, UItemUI*>& GetItemUIAtSlots()
	{
		return itemUIAtSlots;
	}

private:
	static TSubclassOf<UUserWidget> itemUIClass;
	static TSubclassOf<UUserWidget> itemContainerClass;

	UPROPERTY()
	UInventoryUI* inventory;

	UPROPERTY()
	UItemContainer* container;

	UPROPERTY()
	UBaseGameInstance* gameInstance;

	UPROPERTY()
	FString name = "";

	UPROPERTY()
	TMap<int32, UItemUI*> itemUIAtSlots;
};