#pragma once
#include "CoreMinimal.h"
#include "../CSVTable.h"
#include "SurvivalTest/Missions/MissionStructs.h"
#include "MissionLoadoutTable.generated.h"

UCLASS()
class SURVIVALTEST_API UMissionLoadoutTable : public UCSVTable
{
	GENERATED_BODY()
public:
	UMissionLoadoutTable();
	virtual void LoadData(TArray<TArray<FString>> inDataStrings) override;

	TArray<FMissionLoadoutData> GetLoadoutsForMissionType(EMissionType type);

	TArray<FMissionLoadoutData >& GetData() { return missionLoadoutData; }
private:
	TArray<FMissionLoadoutData > missionLoadoutData;
};