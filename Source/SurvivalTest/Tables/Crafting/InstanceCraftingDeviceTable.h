#pragma once
#include "CoreMinimal.h"
#include "SurvivalTest/Items/ItemStructs.h"
#include "SurvivalTest/Tables/CSVTable.h"
#include "InstanceCraftingDeviceTable.generated.h"

UCLASS()
class SURVIVALTEST_API UInstanceCraftingDeviceTable : public UCSVTable
{
	GENERATED_BODY()
public:
	UInstanceCraftingDeviceTable();
	virtual void LoadData(TArray<TArray<FString>> inDataStrings) override;
private:
	TArray<FInstanceCraftingDevice> InstanceCraftingDevice;
public:
	TArray<FInstanceCraftingDevice>& GetData()
	{
		return InstanceCraftingDevice;
	}
};