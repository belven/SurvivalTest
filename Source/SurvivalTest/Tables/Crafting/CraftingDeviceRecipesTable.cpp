#include "CraftingDeviceRecipesTable.h"

UCraftingDeviceRecipesTable::UCraftingDeviceRecipesTable() : Super()
{
	path = CSVT::GetTableFilePath("CraftingDeviceRecipe.csv");
}

void UCraftingDeviceRecipesTable::LoadData(TArray<TArray<FString>> inDataStrings)
{
	for (TArray<FString> row : inDataStrings)
	{
		int index = 0;
		FCraftingDeviceRecipes data;
		data.CraftingDeviceID = GetIntFromString(row[index++]);
		data.RecipeID = GetIntFromString(row[index++]);
		CraftingDeviceRecipes.Add(data);
	}
}
