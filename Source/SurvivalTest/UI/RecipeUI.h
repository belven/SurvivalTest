#pragma once
#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/HorizontalBox.h"
#include "SurvivalTest/Items/ItemStructs.h"
#include "RecipeUI.generated.h"

UCLASS()
class SURVIVALTEST_API URecipeUI : public UUserWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent, Category = "Recipe")
	void UpdateRecpie();

	UFUNCTION(BlueprintCallable, Category = "Recipe")
	FFullRecipe& GetRecipe() { return  recipe; }

	UFUNCTION(BlueprintCallable, Category = "Recipe")
	UBaseGameInstance* GetBaseGameInstance();
	void AddItemToBox(FInputOutputData iod, UHorizontalBox* box);

	UFUNCTION(BlueprintCallable, Category = "Recipe")
	void SetRecipe(const FFullRecipe& newRecipe);

protected:
	UPROPERTY()
	FFullRecipe recipe;

	UPROPERTY(BlueprintReadWrite, Category = "Recipe")
	UHorizontalBox* inputBox;

	UPROPERTY(BlueprintReadWrite, Category = "Recipe")
	UHorizontalBox* outputBox;

	UPROPERTY()
	UBaseGameInstance* baseGameInstance;

	UPROPERTY()
	TSubclassOf<UUserWidget> itemUIClass;

};
