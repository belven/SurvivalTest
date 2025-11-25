
#pragma once
#include "CoreMinimal.h"
#include "SurvivalTest/Items/ItemStructs.h"
#include "SurvivalTest/Tables/CSVTable.h"
#include "CraftingDeviceTable.generated.h"

UCLASS()
class SURVIVALTEST_API UCraftingDeviceTable : public UCSVTable
{
	GENERATED_BODY()
public:
	UCraftingDeviceTable();
	virtual void LoadData(TArray<TArray<FString>> inDataStrings) override;
private:
	TArray<FCraftingDevice> CraftingDevice;
public:
	TArray<FCraftingDevice>& GetData()
	{
		return CraftingDevice;
	}

};
