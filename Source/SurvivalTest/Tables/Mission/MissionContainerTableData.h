#pragma once
#include "CoreMinimal.h"
#include "SurvivalTest/Missions/MissionStructs.h"
#include "SurvivalTest/Tables/CSVTable.h"
#include "MissionContainerTableData.generated.h"

UCLASS()
class SURVIVALTEST_API UMissionContainerTableData : public UCSVTable
{
	GENERATED_BODY()
	public:
		UMissionContainerTableData();
	virtual void LoadData(TArray<TArray<FString>> inDataStrings) override;

	TArray<FMissionContainerData >& GetData() { return missionContainerData; }
private:
	TArray<FMissionContainerData > missionContainerData;
};
