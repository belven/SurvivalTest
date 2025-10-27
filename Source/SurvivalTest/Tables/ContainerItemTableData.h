#pragma once
#include "CoreMinimal.h"
#include "CSVTable.h"
#include "SurvivalTest/Items/ItemStructs.h"
#include "ContainerItemTableData.generated.h"

UCLASS()
class SURVIVALTEST_API UContainerItemTableData : public UCSVTable
{
	GENERATED_BODY()
public:
	UContainerItemTableData();
	virtual void LoadData(TArray<TArray<FString>> inDataStrings) override;

	TArray<FContainerItemData >& GetData() { return containerItemData; }
private:
	TArray<FContainerItemData > containerItemData;
};
