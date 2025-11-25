#pragma once
#include "CoreMinimal.h"
#include "SurvivalTest/Items/ItemStructs.h"
#include "SurvivalTest/Tables/CSVTable.h"
#include "InProgressCraftingTable.generated.h"

UCLASS()
class SURVIVALTEST_API UInProgressCraftingTable : public UCSVTable
{
	GENERATED_BODY()
public:
	UInProgressCraftingTable();
	virtual void LoadData(TArray<TArray<FString>> inDataStrings) override;
private:
	TArray<FInProgressCrafting> InProgressCrafting;
public:
	TArray<FInProgressCrafting>& GetData()
	{
		return InProgressCrafting;
	}
};
