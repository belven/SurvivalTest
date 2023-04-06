#pragma once
#include "CoreMinimal.h"
#include "CSVTable.h"
#include "InstanceArmourDataTable.generated.h"

UCLASS()
class SURVIVALTEST_API UInstanceArmourDataTable : public UCSVTable
{
	GENERATED_BODY()
public:
	UInstanceArmourDataTable();
	virtual void LoadData(TArray<TArray<FString>> inDataStrings) override;

	TArray<FInstanceArmourData >& GetData() { return instanceArmourData; }
private:
	TArray<FInstanceArmourData > instanceArmourData;
	
};