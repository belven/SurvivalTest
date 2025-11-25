
#pragma once
#include "CoreMinimal.h"
#include "SurvivalTest/Items/ItemStructs.h"
#include "SurvivalTest/Tables/CSVTable.h"
#include "CraftingDeviceRecipesTable.generated.h"

UCLASS()
class SURVIVALTEST_API UCraftingDeviceRecipesTable : public UCSVTable
{
	GENERATED_BODY()
public:
	UCraftingDeviceRecipesTable();
	virtual void LoadData(TArray<TArray<FString>> inDataStrings) override;
private:
	TArray<FCraftingDeviceRecipes> CraftingDeviceRecipes;
public:
	TArray<FCraftingDeviceRecipes>& GetData()
	{
		return CraftingDeviceRecipes;
	}
};
