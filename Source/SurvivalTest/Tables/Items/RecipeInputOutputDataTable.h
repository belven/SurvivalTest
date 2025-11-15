#pragma once
#include "CoreMinimal.h"
#include "SurvivalTest/Items/ItemStructs.h"
#include "SurvivalTest/Tables/CSVTable.h"
#include "RecipeInputOutputDataTable.generated.h"

UCLASS()
class SURVIVALTEST_API URecipeInputOutputDataTable : public UCSVTable
{
	GENERATED_BODY()
public:
	URecipeInputOutputDataTable();
	virtual void LoadData(TArray<TArray<FString>> inDataStrings) override;
private:
	TArray<FRecipeInputOutputData> recipeInputOutputData;
public:

	TArray<FRecipeInputOutputData>& GetData()
	{
		return recipeInputOutputData;
	}
};