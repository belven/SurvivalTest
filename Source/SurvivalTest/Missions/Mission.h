#pragma once
#include "CoreMinimal.h"
#include "Engine/TargetPoint.h"
#include "SurvivalTest/Items/ItemStructs.h"
#include "MissionStructs.h"
#include "Mission.generated.h"

class AMissionArea;
class UBaseGameInstance;

UCLASS(HideCategories=("Rendering", "Replication", "Collision", "HLOD", "World_Partition", "Input", "Replication", "Actor", "Cooking", "Data_Layers"))
class SURVIVALTEST_API AMission : public ATargetPoint
{
	GENERATED_BODY()

public:
	bool IsSpawnMission() const { return spawnMission; }
	void SetSpawnMission(bool inSpawnMission) { spawnMission = inSpawnMission; }
	void SpawnMission();
	EMissionType GetMissionType() const { return missionType; }
	void SetMissionType(EMissionType inMissionType) { missionType = inMissionType; }

protected:
	AMission();
	void SetUpLootBoxes();
	virtual void BeginPlay() override;
	bool HasPlayers();
	void SpawnBox(FVector location);

	UPROPERTY()
	UBaseGameInstance* game;

	UFUNCTION()
	void EndOverlap(UPrimitiveComponent* overlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 otherBodyIndex);

	bool IsPlayer(AActor* inActor, UPrimitiveComponent* inOtherComp);
	UFUNCTION()
	void BeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Stats)
	int32 size;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Stats)
	int32 enemyAmount;
	TSubclassOf<APawn> AIClass;

	FTimerHandle TimerHandle_CheckNoPlayers;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Stats, meta = (AllowPrivateAccess = "true"))
	int32 boxSize;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Stats, meta = (AllowPrivateAccess = "true"))
	int32 boxHeight;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Stats, meta = (AllowPrivateAccess = "true"))
	EMissionType missionType;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Stats, meta = (AllowPrivateAccess = "true"))
	bool spawnMission = true;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Stats, meta = (AllowPrivateAccess = "true"))
	TMap<EItemType, int32> itemTypes;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Stats, meta = (AllowPrivateAccess = "true"))
	TMap<AMissionArea*, int32> players;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Stats, meta = (AllowPrivateAccess = "true"))
	TArray<AMissionArea*> missionArea;
};
