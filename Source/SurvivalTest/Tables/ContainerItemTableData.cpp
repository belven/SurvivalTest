#include "ContainerItemTableData.h"

UContainerItemTableData::UContainerItemTableData()
{
	path = CSVT::GetTableFilePath("ContainerItems.csv");
}

void UContainerItemTableData::LoadData(TArray<TArray<FString>> inDataStrings)
{
	for (TArray<FString> row : inDataStrings)
	{
		int index = 0;
		FContainerItemData data;
		data.ID = GetIntFromString(row[index++]);
		data.containerID = GetIntFromString(row[index++]);
		data.itemID = GetIntFromString(row[index++]);
		containerItemData.Add(data);
	}
}
