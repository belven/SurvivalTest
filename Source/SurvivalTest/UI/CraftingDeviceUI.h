#pragma once
#include "CoreMinimal.h"
#include "RecipeListUI.h"
#include "Blueprint/UserWidget.h"
#include "SurvivalTest/Items/ItemStructs.h"
#include "CraftingDeviceUI.generated.h"

class ABasePlayerController;

UCLASS()
class SURVIVALTEST_API UCraftingDeviceUI : public UUserWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "Crafting")
	static UCraftingDeviceUI* CreateCraftingDeviceUI(ABasePlayerController* controller, const FCraftingDevice& inCraftingDevice, const FInstanceCraftingDevice& inInstanceCraftingDevice);

	UFUNCTION(BlueprintCallable, Category = "Crafting")
	void OnItemUpdated(const FInstanceItemData& inItem, const FInstanceItemData& oldItem);

	UFUNCTION(BlueprintImplementableEvent, Category = "Crafting")
	UPanelWidget* GetRecipeUIPanelWidget();

	UFUNCTION(BlueprintCallable, Category = "Crafting")
	float GetProgress();

	UFUNCTION(BlueprintCallable, Category = "Crafting")
	bool IsInProgress();

	UFUNCTION(BlueprintCallable, Category = "Recipes")
	void CancelCrafting(FInProgressCrafting cancelledCraft);

	UFUNCTION(BlueprintCallable, Category = "Crafting")
	ABasePlayerController* GetBasePlayerController();

	FFullRecipe GetRecipeByID(int32 ID);

	UFUNCTION(BlueprintCallable, Category = "Recipes")
	void Craft();

protected:
	static	TSubclassOf<UUserWidget> craftingDeviceUIClass;

	FTimerManager* GetTimerManager();
	void CraftComplete();

	void ConsumeRecipeInputs(FFullRecipe recipe);
	void ProduceOutputs(FFullRecipe recipe);

	void Craft_Internal(const FFullRecipe& fullRecipe);
	int32 GetQueuePosition();
	void CheckInventory(int32 itemChanged);
	bool CheckInventoryForRecipe(const FFullRecipe& fullRecipe);

	UPROPERTY()
	URecipeListUI* recipeListUI;

	UPROPERTY()
	FCraftingDevice craftingDeviceData;

	UPROPERTY()
	FInstanceCraftingDevice instanceCraftingDevice;

	FTimerManager* timerManager;

	UPROPERTY()
	TArray<FFullRecipe> deviceRecipes;

	FFullRecipe currentCraftingRecipe;

	UPROPERTY()
	TArray<FInProgressCrafting> craftingQueue;

	UPROPERTY()
	FTimerHandle TimerHandle_ItemCrafted;

	UPROPERTY()
	ABasePlayerController* basePlayerController;
};
