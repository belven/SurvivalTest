#pragma once
#include "CoreMinimal.h"
#include "CSVTable.h"
#include "ContainerTableData.generated.h"

UCLASS()
class SURVIVALTEST_API UContainerTableData : public UCSVTable
{
	GENERATED_BODY()
public:
	UContainerTableData();
	virtual void LoadData(TArray<TArray<FString>> inDataStrings) override;

	TArray<FContainerData >& GetData() { return containerData; }
private:
	TArray<FContainerData > containerData;
	
};