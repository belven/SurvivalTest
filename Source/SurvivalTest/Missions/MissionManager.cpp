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

	for(AMission* m : missions)
	{
		EMissionType mt = GetRandom<EMissionType>(types);
		//types.Remove(mt);
		m->SetMissionType(mt);
		m->SpawnMission();
	}
}

template <class T> T UMissionManager::GetRandom(TArray<T> itemArray)
{
	return itemArray[FMath::RandRange(0, itemArray.Num() - 1)];
}
