#pragma once
#include "CoreMinimal.h"
#include "SurvivalTest/Items/ItemStructs.h"
#include "SurvivalTest/Tables/CSVTable.h"
#include "RecipeTable.generated.h"

UCLASS()
class SURVIVALTEST_API URecipeTable : public UCSVTable
{
	GENERATED_BODY()
public:
	URecipeTable();
	virtual void LoadData(TArray<TArray<FString>> inDataStrings) override;
private:
	TArray<FRecipeData> recipeData;
public:

	TArray<FRecipeData>& GetData()
	{
		return recipeData;
	}	
};
