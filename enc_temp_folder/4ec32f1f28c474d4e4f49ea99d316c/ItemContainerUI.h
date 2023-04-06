#pragma once
#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "SurvivalTest/Items/ItemContainer.h"
#include "ItemContainerUI.generated.h"

class UBaseGameInstance;

UCLASS()
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
		UItemContainer* GetItemContainer() const { return container; }

	UFUNCTION(BlueprintCallable, Category = "Inventory")
		void SetItemContainer(UItemContainer* inContainer) { container = inContainer; }

	UFUNCTION(BlueprintCallable, Category = "Inventory")
		UBaseGameInstance* GetBaseGameInstance() const { return gameInstance; }

	UFUNCTION(BlueprintCallable, Category = "Inventory")
		FString GetContainerName();

	UFUNCTION(BlueprintCallable, Category = "Inventory")
		void SetBaseGameInstance(UBaseGameInstance* inGameInstance) { gameInstance = inGameInstance; }

	UFUNCTION(BlueprintCallable, Category = "Inventory")
		UItemContainer* GetItemContainerForArmour(FInstanceItemData data);

private:
	UPROPERTY()
		UItemContainer* container;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Inventory, meta = (AllowPrivateAccess = "true"))
	int32 index;

	UPROPERTY()
		UBaseGameInstance* gameInstance;
};