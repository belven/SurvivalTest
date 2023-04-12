#include "ConsumableTableData.h"

UConsumableTableData::UConsumableTableData() : Super()
{
	path = CSVT::GetTableFilePath("ConsumableData.csv");
}

void UConsumableTableData::LoadData(TArray<TArray<FString>> inDataStrings)
{
	for (TArray<FString> row : inDataStrings)
	{
		int index = 0;
		FConsumableData data;
		data.ID = GetIntFromString(row[index++]);
		data.itemID = GetIntFromString(row[index++]);
		data.consumableType = UItemStructs::GetConsumableType(row[index++]);
		data.value = GetIntFromString(row[index++]);
		consumableData.Add(data.ID, data);
	}
}