#pragma once
#include "CoreMinimal.h"
#include "CSVTable.h"
#include "InstanceBoxDataTable.generated.h"

UCLASS()
class SURVIVALTEST_API UInstanceBoxDataTable : public UCSVTable
{
	GENERATED_BODY()
public:
	UInstanceBoxDataTable();
	virtual void LoadData(TArray<TArray<FString>> inDataStrings) override;

	TArray<FInstanceBoxData >& GetData() { return instanceBoxData; }
private:
	TArray<FInstanceBoxData > instanceBoxData;
};