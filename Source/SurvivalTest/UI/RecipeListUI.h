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

	UFUNCTION(BlueprintImplementableEvent, Category = "Recipe")
	UVerticalBox* GetRecipeListContainer();

	ABasePlayerController* GetBasePlayerController();

	UFUNCTION(BlueprintCallable, Category = "Recipes")
	void OnItemUpdated(const FInstanceItemData& inItem, const FInstanceItemData& oldItem);

	UFUNCTION(BlueprintCallable, Category = "Recipes")
	static URecipeListUI* CreateRecipeList(ABasePlayerController* controller, TArray<FFullRecipe> recipesToAdd);
	void CheckInventory(int32 itemChanged);
	bool CheckInventoryForRecipe(FFullRecipe recipe);

	FFullRecipe GetRecipe(int32 recipeID);

	TArray<URecipeUI*>& GetRecipes()
	{
		return recipes;
	}

	UFUNCTION(BlueprintCallable, Category = "Recipes")
	URecipeUI* GetSelectedRecipe()
	{
		return selectedRecipe;
	}

protected:
	UPROPERTY(BlueprintReadWrite, Category = "Recipe")
	TArray<URecipeUI*> recipes;

protected:
	static	TSubclassOf<UUserWidget> recipeListUIClass;
	static	TSubclassOf<UUserWidget> recipeUIClass;

	UPROPERTY(BlueprintReadWrite, Category = "Recipe")
	UVerticalBox* recipeContainer;

	UPROPERTY(BlueprintReadWrite, Category = "Recipe")
	URecipeUI* selectedRecipe;

	UPROPERTY(BlueprintReadWrite, Category = "Recipe")
	ABasePlayerController* basePlayerController;
	};