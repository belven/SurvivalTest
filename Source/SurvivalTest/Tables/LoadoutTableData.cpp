#include "LoadoutTableData.h"

ULoadoutTableData::ULoadoutTableData() : Super()
{
	path = CSVT::GetTableFilePath("LoadoutData.csv");
}

void ULoadoutTableData::LoadData(TArray<TArray<FString>> inDataStrings)
{
	for (TArray<FString> row : inDataStrings)
	{
		int index = 0;
		FLoadoutData data;
		data.ID = GetIntFromString(row[index++]);
		data.type = UItemStructs::GetCharacterType(row[index++]);
		data.entityID = GetIntFromString(row[index++]);
		data.weaponID = GetIntFromString(row[index++]);
		data.headArmourID = GetIntFromString(row[index++]);
		data.chestArmourID = GetIntFromString(row[index++]);
		data.legsArmourID = GetIntFromString(row[index++]);
		data.health = GetIntFromString(row[index++]);
		data.moveSpeed = GetIntFromString(row[index++]);
		loadoutData.Add(data);
	}
}