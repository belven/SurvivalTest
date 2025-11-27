#pragma once
#include "CoreMinimal.h"
#include "CSVTable.generated.h"

#define DS UDataStructs
#define SourceFolder FPaths::GameSourceDir()
#define TablesFolder SourceFolder + "Tables/"

// ReSharper disable once IdentifierTypo
#define CSVT UCSVTable

UCLASS()
class SURVIVALTEST_API UCSVTable : public UObject
{
	GENERATED_BODY()

public:
	virtual void LoadData(TArray<TArray<FString>> inDataStrings);
	virtual TArray<FString> GetDataStrings();

	FString GetPath() const { return path; }

	static FString GetTableFilePath(const FString& table)
	{
		FString path = TablesFolder + table;
		path.ReplaceCharInline(*"/", *"\\");
		//FPaths::NormalizeDirectoryName(path);
		return path;
	};

protected:
	TArray<TArray<FString>> dataStrings;
	FString path;

	static int32 GetIntFromString(const FString& value) { return FCString::Atoi(*value); }
	static float GetFloatFromString(const FString& value) { return FCString::Atof(*value); }

	static FVector GetVectorFromString(const FString& value)
	{
		FVector location;
		TArray<FString> strings;
		value.ParseIntoArray(strings, TEXT(";"));

		if (strings.Num() == 3)
		{
			int32 index = 0;
			location.X = GetFloatFromString(strings[index++]);
			location.Y = GetFloatFromString(strings[index++]);
			location.Z = GetFloatFromString(strings[index++]);
		}
		return location;
	}
};
