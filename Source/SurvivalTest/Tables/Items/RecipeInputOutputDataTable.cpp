#include "RecipeInputOutputDataTable.h"

URecipeInputOutputDataTable::URecipeInputOutputDataTable() : Super()
{
	path = CSVT::GetTableFilePath("RecipeInputOutputData.csv");
}

void URecipeInputOutputDataTable::LoadData(TArray<TArray<FString>> inDataStrings)
{
	for (TArray<FString> row : inDataStrings)
	{
		int index = 0;
		FRecipeInputOutputData data;
		data.ID = GetIntFromString(row[index++]);
		data.recipeID = GetIntFromString(row[index++]);
		data.inputOutputDataID = GetIntFromString(row[index++]);
		recipeInputOutputData.Add(data);
	}
}