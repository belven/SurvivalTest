#pragma once
#include "CoreMinimal.h"
#include "Blueprint/DragDropOperation.h"
#include "ItemContainerUI.h"
#include "ItemDragDrop.generated.h"

class UItemUI;

UCLASS()
class SURVIVALTEST_API UItemDragDrop : public UDragDropOperation
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "Inventory")
	static UItemDragDrop* CreateDragDrop(UItemUI* item);

	UFUNCTION(BlueprintCallable, Category = "Inventory")
	UItemUI* GetDraggedItem() const { return draggedItem; }

	UFUNCTION(BlueprintCallable, Category = "Inventory")
	void SetDraggedItem(UItemUI* inDraggedItem) { draggedItem = inDraggedItem; }

private:
	UPROPERTY()
	UItemUI* draggedItem;
};
