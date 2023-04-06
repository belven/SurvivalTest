#include "InstanceArmourDataTable.h"

UInstanceArmourDataTable::UInstanceArmourDataTable() : Super()
{
	path = CSVT::GetTableFilePath("InstanceArmourData.csv");
}

void UInstanceArmourDataTable::LoadData(TArray<TArray<FString>> inDataStrings)
{
	for (TArray<FString> row : inDataStrings)
	{
		int index = 0;
		FInstanceArmourData data;
		data.ID = GetIntFromString(row[index++]);
		data.containerInstanceID = GetIntFromString(row[index++]);
		data.armourID = GetIntFromString(row[index++]);
		data.instancedItemDataID = GetIntFromString(row[index++]);
		instanceArmourData.Add(data);
	}
}