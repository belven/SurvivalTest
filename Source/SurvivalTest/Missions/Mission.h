#pragma once
#include "CoreMinimal.h"
#include "Engine/TargetPoint.h"
#include "SurvivalTest/Items/ItemStructs.h"
#include "MissionStructs.h"
#include "NavigationData.h"
#include "NavigationInvokerComponent.h"
#include "Mission.generated.h"

class AMissionArea;
class UBaseGameInstance;
class ABaseCharacter;

UCLASS(HideCategories=("Rendering", "Replication", "Collision", "HLOD", "World_Partition", "Input", "Replication", "Actor", "Cooking", "Data_Layers"))
class SURVIVALTEST_API AMission : public ATargetPoint
{
	GENERATED_BODY()

public:
	void MissionComplete();
	void SpawnAI();
	void SpawnMission_Internal();
	bool ShouldSpawnMission() const { return spawnMission; }
	bool MissionSpawned() const { return missionSpawned; }
	void SetSpawnMission(bool inSpawnMission) { spawnMission = inSpawnMission; }
	UFUNCTION()
	void NavDone(ANavigationData* inNavData);
	void SpawnMission();
	EMissionType GetMissionType() const { return missionType; }
	void SetMissionType(EMissionType inMissionType) { missionType = inMissionType; }
	void SetUpLootBoxes();
	void SpawnDefault();

protected:
	AMission();
	FContainerData GetRandomContainerData();
	virtual void BeginPlay() override;
	bool HasPlayers();
	void SpawnBox(const FVector& location);
	bool IsPlayer(AActor* inActor, UPrimitiveComponent* inOtherComp);

	TSubclassOf<APawn> AIClass;
	FTimerHandle TimerHandle_CheckNoPlayers;
	bool spawnMission = true;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Stats, meta = (AllowPrivateAccess = "true"))
	bool missionSpawned = false;

	UPROPERTY()
	TArray<FContainerData> cds;

	UPROPERTY()
	UBaseGameInstance* game;

	UPROPERTY()
	UNavigationInvokerComponent* navInvoker;

	UFUNCTION()
	void EndOverlap(UPrimitiveComponent* overlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 otherBodyIndex);
	
	UFUNCTION()
	void BeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void CharacterDied(ABaseCharacter* character);

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Stats, meta = (AllowPrivateAccess = "true"))
	int32 boxSize;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Stats, meta = (AllowPrivateAccess = "true"))
	EMissionType missionType;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Stats, meta = (AllowPrivateAccess = "true"))
	int32 boxHeight;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Stats, meta = (AllowPrivateAccess = "true"))
	bool missionComplete = false;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Stats, meta = (AllowPrivateAccess = "true"))
	TMap<AMissionArea*, int32> players;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Stats, meta = (AllowPrivateAccess = "true"))
	TArray<ABaseCharacter*> aiSpawned;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Stats, meta = (AllowPrivateAccess = "true"))
	TArray<ABaseCharacter*> aiAlive;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Stats, meta = (AllowPrivateAccess = "true"))
	TArray<AMissionArea*> missionArea;
};