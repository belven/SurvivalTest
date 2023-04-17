#include "MissionItemTable.h"

UMissionItemTable::UMissionItemTable() : Super()
{
	path = CSVT::GetTableFilePath("MissionItemData.csv");
}

void UMissionItemTable::LoadData(TArray<TArray<FString>> inDataStrings)
{
	for (TArray<FString> row : inDataStrings)
	{
		int index = 0;
		FMissionItemData data;
		data.ID = GetIntFromString(row[index++]);
		data.itemID = GetIntFromString(row[index++]);
		data.type =  UMissionStructs::GetMissionType(row[index++]);
		missionItemData.Add(data);
	}
}