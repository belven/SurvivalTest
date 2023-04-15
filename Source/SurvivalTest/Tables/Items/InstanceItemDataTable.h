#pragma once
#include "CoreMinimal.h"
#include "../CSVTable.h"
#include "InstanceItemDataTable.generated.h"

UCLASS()
class SURVIVALTEST_API UInstanceItemDataTable : public UCSVTable
{
	GENERATED_BODY()
public:
		UInstanceItemDataTable();
	virtual void LoadData(TArray<TArray<FString>> inDataStrings) override;

	TMap<int32, FInstanceItemData>& GetData() { return instanceItems; }

	virtual TArray<FString> GetDataStrings() override;

private:
	TMap<int32, FInstanceItemData> instanceItems;
};