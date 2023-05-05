#include "MissionManager.h"
#include "Mission.h"

void UMissionManager::CreateMissions()
{
	
}

void UMissionManager::AddMission(AMission* inMission)
{
	missions.Add(inMission);
}

void UMissionManager::StartPlay()
{
	TArray<EMissionType> types;
	//types.Add(EMissionType::Civilian);
	types.Add(EMissionType::Medical);
	//types.Add(EMissionType::Military);
	//types.Add(EMissionType::Secret);

	// Spawn Random Missions
	int32 randomMissionAmount = GetRandomMissionAmount();

	for (int i = 0; i < randomMissionAmount; ++i)
	{
		AMission* mission = nullptr;
		
		while(mission == nullptr)
		{
			mission = GetRandom<AMission*>(missions);

			if (!mission->MissionSpawned()) {
				EMissionType mt = GetRandom<EMissionType>(types);
				mission->SetMissionType(mt);
				mission->SpawnMission();
			}
			else
			{
				mission = nullptr;
			}
		}
	}	

	// Spawn loot in non spawned missions
	for(AMission* m : missions)
	{
		if (!m->MissionSpawned()) {
			//m->SpawnLoot();
		}
	}
}

int32 UMissionManager::GetRandomMissionAmount()
{
	int32 amount = 0;
	float missionsNum = missions.Num();

	if(missionsNum == 1)
	{
		amount = 1;
	}
	else {
		amount = FMath::Min(1, FMath::RoundHalfToEven(missionsNum / 2));
	}

	return amount;
}

template <class T> T UMissionManager::GetRandom(TArray<T> itemArray)
{
	return itemArray[FMath::RandRange(0, itemArray.Num() - 1)];
}
