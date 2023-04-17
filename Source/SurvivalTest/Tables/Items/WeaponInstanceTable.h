#pragma once
#include "CoreMinimal.h"
#include "SurvivalTest/Tables/CSVTable.h"
#include "WeaponInstanceTable.generated.h"

UCLASS()
class SURVIVALTEST_API UWeaponInstanceTable : public UCSVTable
{
	GENERATED_BODY()
public:
	UWeaponInstanceTable();
	virtual void LoadData(TArray<TArray<FString>> inDataStrings) override;

	TMap<int32, FInstanceWeaponData>& GetData() { return instanceWeaponData; }

	virtual TArray<FString> GetDataStrings() override;

private:
	TMap<int32, FInstanceWeaponData> instanceWeaponData;
	
};