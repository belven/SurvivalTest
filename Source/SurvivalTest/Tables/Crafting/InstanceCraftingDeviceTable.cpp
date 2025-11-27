#include "InstanceCraftingDeviceTable.h"

UInstanceCraftingDeviceTable::UInstanceCraftingDeviceTable() : Super()
{
	path = CSVT::GetTableFilePath("InstanceCraftingDevice.csv");
}

void UInstanceCraftingDeviceTable::LoadData(TArray<TArray<FString>> inDataStrings)
{
	for (TArray<FString> row : inDataStrings)
	{
		int index = 0;
		FInstanceCraftingDevice data;
		data.ID = GetIntFromString(row[index++]);
		data.CraftingDeviceID = GetIntFromString(row[index++]);
		data.Location = GetVectorFromString(row[index++]);
		data.OwnerID = GetIntFromString(row[index++]);
		data.OwnerType = UItemStructs::GetOwnerType(row[index++]);
		InstanceCraftingDevice.Add(data);
	}
}
