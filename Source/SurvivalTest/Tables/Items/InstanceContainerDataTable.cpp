#include "InstanceContainerDataTable.h"

UInstanceContainerDataTable::UInstanceContainerDataTable() : Super()
{
	path = CSVT::GetTableFilePath("InstanceContainerData.csv");
}

void UInstanceContainerDataTable::LoadData(TArray<TArray<FString>> inDataStrings)
{
	for (TArray<FString> row : inDataStrings)
	{
		int index = 0;
		FInstanceContainerData data;
		data.ID = GetIntFromString(row[index++]);
		data.containerID = GetIntFromString(row[index++]);
		data.type = UItemStructs::GetContainerType(row[index++]);
		instanceContainerData.Add(data);
	}
}