#include "InputOutputDataTable.h"

UInputOutputDataTable::UInputOutputDataTable() : Super()
{
	path = CSVT::GetTableFilePath("InputOutputData.csv");
}

void UInputOutputDataTable::LoadData(TArray<TArray<FString>> inDataStrings)
{
	for (TArray<FString> row : inDataStrings)
	{
		int index = 0;
		FInputOutputData data;
		data.ID = GetIntFromString(row[index++]);
		data.inputOutputID = GetIntFromString(row[index++]);
		data.amount = GetIntFromString(row[index++]);
		data.type = UItemStructs::GetInputOutputType(row[index++]);
		data.inputOrOutput = UItemStructs::GetInputOrOutput(row[index++]);
		inputOutputData.Add(data);
	}
}