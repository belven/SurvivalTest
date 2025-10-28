#include "MissionContainerTableData.h"

UMissionContainerTableData::UMissionContainerTableData() : Super()
{
	path = CSVT::GetTableFilePath("MissionContainerData.csv");
}

void UMissionContainerTableData::LoadData(TArray<TArray<FString>> inDataStrings)
{
	for (TArray<FString> row : inDataStrings)
	{
		int index = 0;
		FMissionContainerData data;
		data.ID = GetIntFromString(row[index++]);
		data.type = UMissionStructs::GetMissionType(row[index++]);
		data.containerID = GetIntFromString(row[index++]);
		missionContainerData.Add(data);
	}
}
