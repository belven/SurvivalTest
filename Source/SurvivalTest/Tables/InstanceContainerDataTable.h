#pragma once
#include "CoreMinimal.h"
#include "CSVTable.h"
#include "InstanceContainerDataTable.generated.h"

UCLASS()
class SURVIVALTEST_API UInstanceContainerDataTable : public UCSVTable
{
	GENERATED_BODY()
public:
		UInstanceContainerDataTable();
	virtual void LoadData(TArray<TArray<FString>> inDataStrings) override;

	TArray<FInstanceContainerData >& GetData() { return instanceContainerData; }
private:
	TArray<FInstanceContainerData > instanceContainerData;	
};