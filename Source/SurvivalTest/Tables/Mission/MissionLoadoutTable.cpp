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
		data.loadoutID = GetIntFromString(row[index++]);
		data.type = UMissionStructs::GetMissionType(row[index++]);
		missionLoadoutData.Add(data);
	}
}

TArray<FMissionLoadoutData> UMissionLoadoutTable::GetLoadoutsForMissionType(EMissionType type)
{
	TArray<FMissionLoadoutData> loadouts;
	for(auto& mld : GetData())
	{
		if (mld.type == type)
			loadouts.Add(mld);
	}

	return loadouts;
}