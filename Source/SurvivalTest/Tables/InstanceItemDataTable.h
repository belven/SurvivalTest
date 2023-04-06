#pragma once
#include "CoreMinimal.h"
#include "CSVTable.h"
#include "InstanceItemDataTable.generated.h"

UCLASS()
class SURVIVALTEST_API UInstanceItemDataTable : public UCSVTable
{
	GENERATED_BODY()
		UInstanceItemDataTable();
	virtual void LoadData(TArray<TArray<FString>> inDataStrings) override;

	TArray<FInstanceItemData >& GetData() { return instanceItemData; }
private:
	TArray<FInstanceItemData > instanceItemData;

};