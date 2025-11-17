#pragma once
#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/HorizontalBox.h"
#include "SurvivalTest/Items/ItemStructs.h"
#include "RecipeUI.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FRecipeSelectionChanged, URecipeUI*, recipeUI);

UCLASS()
class SURVIVALTEST_API URecipeUI : public UUserWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent, Category = "Recipe")
	void UpdateRecpie();

	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent, Category = "Recipe")
	UHorizontalBox* GetRecipeInputContainer();

	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent, Category = "Recipe")
	UHorizontalBox* GetRecipeOutputContainer();

	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent, Category = "Recipe")
	void Select();

	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent, Category = "Recipe")
	void Deselect();

	UFUNCTION(BlueprintCallable, Category = "Recipe")
	FFullRecipe& GetRecipe() { return  recipe; }

	UFUNCTION(BlueprintCallable, Category = "Recipe")
	UBaseGameInstance* GetBaseGameInstance();
	void AddItemToBox(FInputOutputData iod, UHorizontalBox* box);

	UFUNCTION(BlueprintCallable, Category = "Recipe")
	void SetRecipe(const FFullRecipe& newRecipe);

	void SetSelected(bool selectedState);

	FRecipeSelectionChanged OnRecipeSelectionChanged;

protected:
	virtual FReply NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;

	UPROPERTY()
	FFullRecipe recipe;

	UPROPERTY(BlueprintReadWrite, Category="Recipe")
	bool selected;

	UPROPERTY(BlueprintReadWrite, Category = "Recipe")
	UHorizontalBox* inputBox;

	UPROPERTY(BlueprintReadWrite, Category = "Recipe")
	UHorizontalBox* outputBox;

	UPROPERTY()
	UBaseGameInstance* baseGameInstance;

	UPROPERTY()
	TSubclassOf<UUserWidget> itemUIClass;

};
