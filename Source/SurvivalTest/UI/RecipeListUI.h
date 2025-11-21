#pragma once
#include "CoreMinimal.h"
#include "RecipeUI.h"
#include "Blueprint/UserWidget.h"
#include "RecipeListUI.generated.h"

class UVerticalBox;
class ABasePlayerController;

UCLASS()
class SURVIVALTEST_API URecipeListUI : public UUserWidget
{
	GENERATED_BODY()
public:

	UFUNCTION(BlueprintCallable, Category = "Recipes")
	void RecipeSelectionChanged(URecipeUI* recipeUI);

	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent, Category = "Recipe")
	UVerticalBox* GetRecipeListContainer();


	ABasePlayerController* GetBasePlayerController();

	UFUNCTION(BlueprintCallable, Category = "Recipes")
	void OnItemUpdated(const FInstanceItemData& inItem, const FInstanceItemData& oldItem);

	UFUNCTION(BlueprintCallable, Category = "Recipes")
	static URecipeListUI* CreateRecipeList(ABasePlayerController* controller, TArray<FFullRecipe> recipesToAdd);
	void CheckInventory(int32 itemChanged);
	bool CheckInventoryForRecipe(FFullRecipe recipe);
	void Craft_Internal(URecipeUI* recipe);

	UFUNCTION(BlueprintCallable, Category = "Recipes")
	void Craft();
	void ConsumeRecipeInputs(URecipeUI* recipe);
	void ProduceOutputs(URecipeUI* recipe);
	FFullRecipe GetRecipe(int32 recipeID);

	UFUNCTION(BlueprintCallable, Category = "Recipes")
	void CancelCrafting(FFullRecipe cancelledRecipe);

	UFUNCTION()
	void CraftComplete();

	TArray<URecipeUI*>& GetRecipes()
	{
		return recipes;
	}


	UFUNCTION(BlueprintCallable, Category = "Recipes")
	float GetProgress();

	UFUNCTION(BlueprintCallable, Category = "Recipes")
	bool IsInProgress();

	UFUNCTION(BlueprintCallable, Category = "Recipes")
	URecipeUI* GetSelectedRecipe()
	{
		return selectedRecipe;
	}

protected:
	UPROPERTY(BlueprintReadWrite, Category = "Recipe")
	TArray<URecipeUI*> recipes;

	UPROPERTY(BlueprintReadWrite, Category = "Recipe")
	TArray<URecipeUI*> inProgressRecipes;
	FTimerManager* timerManager;

public:
	FTimerManager* GetTimerManager();

protected:
	static	TSubclassOf<UUserWidget> recipeListUIClass;
	static	TSubclassOf<UUserWidget> recipeUIClass;

	UPROPERTY(BlueprintReadWrite, Category = "Recipe")
	UVerticalBox* recipeContainer;

	UPROPERTY(BlueprintReadWrite, Category = "Recipe")
	URecipeUI* selectedRecipe;

	FTimerHandle TimerHandle_ItemCrafted;

	UPROPERTY(BlueprintReadWrite, Category = "Recipe")
	ABasePlayerController* basePlayerController;

};
