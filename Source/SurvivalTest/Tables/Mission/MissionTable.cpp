#include "MissionTable.h"

UMissionTable::UMissionTable()
{
	path = CSVT::GetTableFilePath("MissionData.csv");
}

void UMissionTable::LoadData(TArray<TArray<FString>> inDataStrings)
{
	for (TArray<FString> row : inDataStrings)
	{
		int index = 0;
		FMissionData data;
		data.ID = GetIntFromString(row[index++]);
		data.type = UMissionStructs::GetMissionType(row[index++]);
		missionData.Add(data);
	}
}