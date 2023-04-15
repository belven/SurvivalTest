#include "InstanceItemDataTable.h"

UInstanceItemDataTable::UInstanceItemDataTable() : Super()
{
	path = CSVT::GetTableFilePath("InstanceItemData.csv");
}

void UInstanceItemDataTable::LoadData(TArray<TArray<FString>> inDataStrings)
{
	for (TArray<FString> row : inDataStrings)
	{
		int index = 0;
		FInstanceItemData data;
		data.ID = GetIntFromString(row[index++]);
		data.itemID = GetIntFromString(row[index++]);
		data.containerInstanceID = GetIntFromString(row[index++]);
		data.amount = GetIntFromString(row[index++]);
		data.slot = GetIntFromString(row[index++]);
		instanceItems.Add(data.ID, data);
	}
}