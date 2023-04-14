#pragma once
#include "CoreMinimal.h"
#include "../CSVTable.h"
#include "SurvivalTest/Missions/MissionStructs.h"
#include "MissionTable.generated.h"

UCLASS()
class SURVIVALTEST_API UMissionTable : public UCSVTable
{
	GENERATED_BODY()
public:
	UMissionTable();
	virtual void LoadData(TArray<TArray<FString>> inDataStrings) override;

	TArray<FMissionData >& GetData() { return missionData; }
private:
	TArray<FMissionData > missionData;
};