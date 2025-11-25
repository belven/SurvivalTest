
#include "InProgressCraftingTable.h"

UInProgressCraftingTable::UInProgressCraftingTable() : Super()
{
	path = CSVT::GetTableFilePath("InProgressCrafting.csv");
}

void UInProgressCraftingTable::LoadData(TArray<TArray<FString>> inDataStrings)
{
	for (TArray<FString> row : inDataStrings)
	{
		int index = 0;
		FInProgressCrafting data;
		data.ID = GetIntFromString(row[index++]);
		data.InstanceCraftingDeviceID = GetIntFromString(row[index++]);
		data.RecipeID = GetIntFromString(row[index++]);
		data.QueuePosition = GetIntFromString(row[index++]);
		data.Progress = GetFloatFromString(row[index++]);
		InProgressCrafting.Add(data);
	}
}
