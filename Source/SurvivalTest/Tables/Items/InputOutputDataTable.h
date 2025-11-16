#pragma once
#include "CoreMinimal.h"
#include "SurvivalTest/Items/ItemStructs.h"
#include "SurvivalTest/Tables/CSVTable.h"
#include "InputOutputDataTable.generated.h"

UCLASS()
class SURVIVALTEST_API UInputOutputDataTable : public UCSVTable
{
	GENERATED_BODY()
public:
	UInputOutputDataTable();
	virtual void LoadData(TArray<TArray<FString>> inDataStrings) override;
private:
	TArray<FInputOutputData> inputOutputData;
public:

	TArray<FInputOutputData>& GetData()
	{
		return inputOutputData;
	}
};