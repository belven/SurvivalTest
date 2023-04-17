#include "ContainerTableData.h"

UContainerTableData::UContainerTableData() : Super()
{
	path = CSVT::GetTableFilePath("ContainerData.csv");
}

void UContainerTableData::LoadData(TArray<TArray<FString>> inDataStrings)
{
	for (TArray<FString> row : inDataStrings)
	{
		int index = 0;
		FContainerData data;
		data. ID = GetIntFromString(row[index++]);
		data.slots = GetIntFromString(row[index++]);
		data.name = row[index++];
		data.mesh = row[index++];
		data.type = UMissionStructs::GetMissionType(row[index++]);
		containerData.Add(data.ID,data);
	}
}