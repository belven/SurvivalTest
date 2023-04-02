#include "InstanceBoxDataTable.h"

UInstanceBoxDataTable::UInstanceBoxDataTable() : Super()
{
	path = CSVT::GetTableFilePath("InstanceBoxDataTable.csv");
}

void UInstanceBoxDataTable::LoadData(TArray<TArray<FString>> inDataStrings)
{
	for (TArray<FString> row : inDataStrings)
	{
		int index = 0;
		FInstanceBoxData data;
		data.ID = GetIntFromString(row[index++]);
		data.boxID = GetIntFromString(row[index++]);
		data.containerInstanceID = GetIntFromString(row[index++]);
		instanceBoxData.Add(data);
	}
}