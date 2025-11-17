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

	UFUNCTION(BlueprintCallable, Category = "Recipes")
	static URecipeListUI* CreateRecipeList(ABasePlayerController* controller, TArray<FFullRecipe> recipesToAdd);
	void CheckInventory(int32 itemChanged);

	UFUNCTION(BlueprintCallable, Category = "Recipes")
	void Craft();

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

	static	TSubclassOf<UUserWidget> recipeListUIClass;
	static	TSubclassOf<UUserWidget> recipeUIClass;

	UPROPERTY(BlueprintReadWrite, Category = "Recipe")
	UVerticalBox* recipeContainer;

	UPROPERTY(BlueprintReadWrite, Category = "Recipe")
	URecipeUI* selectedRecipe;

};
