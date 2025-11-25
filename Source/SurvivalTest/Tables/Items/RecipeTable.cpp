#include "RecipeTable.h"

URecipeTable::URecipeTable() : Super()
{
	path = CSVT::GetTableFilePath("RecipeData.csv");
}

void URecipeTable::LoadData(TArray<TArray<FString>> inDataStrings)
{
	for (TArray<FString> row : inDataStrings)
	{
		int index = 0;
		FRecipeData data;
		data.ID = GetIntFromString(row[index++]);
		data.name = *row[index++];
		data.type = UItemStructs::GetRecipeType(row[index++]);
		data.craftingTime = GetFloatFromString(row[index++]);
		recipeData.Add(data);
	}
}