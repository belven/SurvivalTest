#pragma once
#include "CoreMinimal.h"
#include "SurvivalTest/Tables/CSVTable.h"
#include "SurvivalTest/Missions/MissionStructs.h"
#include "MissionItemTable.generated.h"

UCLASS()
class SURVIVALTEST_API UMissionItemTable : public UCSVTable
{
	GENERATED_BODY()
public:
		UMissionItemTable();
	virtual void LoadData(TArray<TArray<FString>> inDataStrings) override;

	TArray<FMissionItemData >& GetData() { return missionItemData; }
private:
	TArray<FMissionItemData > missionItemData;
};