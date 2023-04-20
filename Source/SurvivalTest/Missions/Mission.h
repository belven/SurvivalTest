#pragma once
#include "CoreMinimal.h"
#include "Engine/TargetPoint.h"
#include "SurvivalTest/Items/ItemStructs.h"
#include "MissionStructs.h"
#include "NavigationInvokerComponent.h"
#include "SurvivalTest/Events/EventListener.h"
#include "Mission.generated.h"

class AMissionArea;
class UBaseGameInstance;
class ABaseCharacter;

UCLASS(HideCategories=("Rendering", "Replication", "Collision", "HLOD", "World_Partition", "Input", "Replication", "Actor", "Cooking", "Data_Layers"))
class SURVIVALTEST_API AMission : public ATargetPoint, public IEventListener
{
	GENERATED_BODY()

public:
	virtual void EventTriggered(UBaseEvent* inEvent) override;
	void MissionComplete();
	bool IsSpawnMission() const { return spawnMission; }
	void SetSpawnMission(bool inSpawnMission) { spawnMission = inSpawnMission; }
	void SpawnMission();
	EMissionType GetMissionType() const { return missionType; }
	void SetMissionType(EMissionType inMissionType) { missionType = inMissionType; }

protected:
	AMission();
	FContainerData GetRandomContainerData();
	void SetUpLootBoxes();
	virtual void BeginPlay() override;
	bool HasPlayers();
	void SpawnBox(FVector location);
	bool IsPlayer(AActor* inActor, UPrimitiveComponent* inOtherComp);

	TSubclassOf<APawn> AIClass;
	FTimerHandle TimerHandle_CheckNoPlayers;
	EMissionType missionType;
	bool spawnMission = true;

	UPROPERTY()
	UBaseGameInstance* game;

	UPROPERTY()
	UNavigationInvokerComponent* navInvoker;

	UFUNCTION()
	void EndOverlap(UPrimitiveComponent* overlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 otherBodyIndex);
	
	UFUNCTION()
	void BeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Stats, meta = (AllowPrivateAccess = "true"))
	int32 boxSize;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Stats, meta = (AllowPrivateAccess = "true"))
	int32 boxHeight;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Stats, meta = (AllowPrivateAccess = "true"))
	bool missionComplete = false;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Stats, meta = (AllowPrivateAccess = "true"))
	TMap<AMissionArea*, int32> players;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Stats, meta = (AllowPrivateAccess = "true"))
	TArray<ABaseCharacter*> aiSpawned;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Stats, meta = (AllowPrivateAccess = "true"))
	TArray<AMissionArea*> missionArea;
};