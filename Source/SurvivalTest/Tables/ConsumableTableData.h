#pragma once
#include "CoreMinimal.h"
#include "CSVTable.h"
#include "ConsumableTableData.generated.h"

UCLASS()
class SURVIVALTEST_API UConsumableTableData : public UCSVTable
{
	GENERATED_BODY()
public:
	UConsumableTableData();
		virtual void LoadData(TArray<TArray<FString>> inDataStrings) override;

	TMap<int32, FConsumableData >& GetData() { return consumableData; }
private:
	TMap<int32, FConsumableData > consumableData;
	
};