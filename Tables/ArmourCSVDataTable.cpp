#include "ArmourCSVDataTable.h"

UArmourCSVDataTable::UArmourCSVDataTable() : Super()
{
	path = CSVT::GetTableFilePath("ArmourData.csv");
}

void UArmourCSVDataTable::LoadData(TArray<TArray<FString>> inDataStrings)
{
	for (TArray<FString> row : inDataStrings)
	{
		int index = 0;
		FArmourData data;
		data.ID = GetIntFromString(row[index++]);
		data.itemID = GetIntFromString(row[index++]);
		data.slot = UItemStructs::GetArmourSlot(row[index++]);
		data.resistance = GetIntFromString(row[index++]);
		armourData.Add(data);
	}
}