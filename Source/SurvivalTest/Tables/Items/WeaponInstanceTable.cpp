// Fill out your copyright notice in the Description page of Project Settings.


#include "WeaponInstanceTable.h"


UWeaponInstanceTable::UWeaponInstanceTable() : Super()
{
	path = CSVT::GetTableFilePath("InstanceWeaponData.csv");
}

void UWeaponInstanceTable::LoadData(TArray<TArray<FString>> inDataStrings)
{
	for (TArray<FString> row : inDataStrings)
	{
		int index = 0;
		FInstanceWeaponData data;
		data.ID = GetIntFromString(row[index++]);
		data.instanceItemID = GetIntFromString(row[index++]);
		data.ammo = GetIntFromString(row[index++]);
		data.mode = UItemStructs::GetFireMode(row[index++]);
		instanceWeaponData.Add(data.ID, data);
	}
}

TArray<FString> UWeaponInstanceTable::GetDataStrings()
{
	TArray<FString> rows;
	for (auto& data : GetData())
	{
		TArray< FStringFormatArg > args;
		args.Add(FStringFormatArg(data.Value.ID));
		args.Add(FStringFormatArg(data.Value.instanceItemID));
		args.Add(FStringFormatArg(data.Value.ammo));
		args.Add(UItemStructs::GetFireMode(data.Value.mode));
		rows.Add(FString::Format(TEXT("{0}, {1}, {2}, {3}, {4}"), args));
	}
	return rows;
}