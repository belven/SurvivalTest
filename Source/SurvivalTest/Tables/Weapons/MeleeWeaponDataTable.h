#pragma once
#include "CoreMinimal.h"
#include "SurvivalTest/Tables/CSVTable.h"
#include "MeleeWeaponDataTable.generated.h"

UCLASS()
class SURVIVALTEST_API UMeleeWeaponDataTable : public UCSVTable
{
	GENERATED_BODY()
public:
	UMeleeWeaponDataTable();
	virtual void LoadData(TArray<TArray<FString>> inDataStrings) override;

	TArray<FMeleeWeaponData >& GetData() { return weaponData; }
private:
	TArray<FMeleeWeaponData > weaponData;
};
