#pragma once

#include "CoreMinimal.h"
#include "CSVTable.h"
#include "ArmourDataTable.generated.h"

UCLASS()
class SURVIVALTEST_API UArmourDataTable : public UCSVTable
{
	GENERATED_BODY()
public:
	UArmourDataTable();
	virtual void LoadData(TArray<TArray<FString>> inDataStrings) override;

	TArray<FArmourData >& GetData() { return armourData; }
private:
	TArray<FArmourData > armourData;
};
