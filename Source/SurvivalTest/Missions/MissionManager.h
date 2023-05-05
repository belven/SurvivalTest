#pragma once
#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "MissionManager.generated.h"

class AMission;
class UBaseGameInstance;

UCLASS()
class SURVIVALTEST_API UMissionManager : public UObject
{
	GENERATED_BODY()

public:
	void CreateMissions();

	TArray<AMission*> GetMissions() const { return missions; }
	void SetMissions(TArray<AMission*> inMissions) { missions = inMissions; }

	UBaseGameInstance* GetGame() const	{		return game;	}
	void SetGame(UBaseGameInstance* inGame)	{		game = inGame;	}

	void AddMission(AMission* inMission);
	void StartPlay();
	int32 GetRandomMissionAmount();

private:
	TArray<AMission*> missions;

	UPROPERTY()
	UBaseGameInstance* game;

	template<class T> T GetRandom(TArray<T> itemArray);
};