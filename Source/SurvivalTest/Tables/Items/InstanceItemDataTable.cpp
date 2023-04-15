#include "InstanceItemDataTable.h"

UInstanceItemDataTable::UInstanceItemDataTable() : Super()
{
	path = CSVT::GetTableFilePath("InstanceItemData.csv");
}

void UInstanceItemDataTable::LoadData(TArray<TArray<FString>> inDataStrings)
{
	for (TArray<FString> row : inDataStrings)
	{
		int index = 0;
		FInstanceItemData data;
		data.ID = GetIntFromString(row[index++]);
		data.itemID = GetIntFromString(row[index++]);
		data.containerInstanceID = GetIntFromString(row[index++]);
		data.amount = GetIntFromString(row[index++]);
		data.slot = GetIntFromString(row[index++]);
		instanceItems.Add(data.ID, data);
	}
}

TArray<FString> UInstanceItemDataTable::GetDataStrings()
{
	TArray<FString> rows;
	for (auto& data : GetData())
	{
		TArray< FStringFormatArg > args;
		args.Add(FStringFormatArg(data.Value.ID));
		args.Add(FStringFormatArg(data.Value.itemID));
		args.Add(FStringFormatArg(data.Value.containerInstanceID));
		args.Add(FStringFormatArg(data.Value.amount));
		args.Add(FStringFormatArg(data.Value.slot));
		rows.Add(FString::Format(TEXT("{0}, {1}, {2}, {3}, {4}"), args));
	}
	return rows;
}
