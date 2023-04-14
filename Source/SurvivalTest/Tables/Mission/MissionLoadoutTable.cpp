#include "MissionLoadoutTable.h"

UMissionLoadoutTable::UMissionLoadoutTable()
{
	path = CSVT::GetTableFilePath("MissionLoadoutData.csv");
}

void UMissionLoadoutTable::LoadData(TArray<TArray<FString>> inDataStrings)
{
	for (TArray<FString> row : inDataStrings)
	{
		int index = 0;
		FMissionLoadoutData data;
		data.ID = GetIntFromString(row[index++]);
		data.name = row[index++];
		data.type = UMissionStructs::GetMissionType(row[index++]);
		missionLoadoutData.Add(data);
	}
}