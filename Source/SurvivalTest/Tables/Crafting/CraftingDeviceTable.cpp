#include "CraftingDeviceTable.h"

UCraftingDeviceTable::UCraftingDeviceTable() : Super()
{
	path = CSVT::GetTableFilePath("CraftingDevice.csv");
}

void UCraftingDeviceTable::LoadData(TArray<TArray<FString>> inDataStrings)
{
	for (TArray<FString> row : inDataStrings)
	{
		int index = 0;
		FCraftingDevice data;
		data.ID = GetIntFromString(row[index++]);
		data.Name = *row[index++];
		CraftingDevice.Add(data);
	}
}
