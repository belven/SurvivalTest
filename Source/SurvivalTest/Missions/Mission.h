#pragma once
#include "CoreMinimal.h"
#include "Engine/TargetPoint.h" 	
#include "Engine/TriggerBox.h"
#include "Mission.generated.h"

class AMissionArea;

UCLASS()
class SURVIVALTEST_API AMission : public ATargetPoint
{
	GENERATED_BODY()

protected:
	AMission();
	virtual void BeginPlay() override;
	bool HasPlayers();
	void SpawnBox(FVector location);

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

private:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Stats, meta = (AllowPrivateAccess = "true"))
	int32 boxSize;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Stats, meta = (AllowPrivateAccess = "true"))
	int32 boxHeight;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Stats, meta = (AllowPrivateAccess = "true"))
		TMap<AMissionArea*, int32> players;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Stats, meta = (AllowPrivateAccess = "true"))
		TArray<AMissionArea*>missionArea;
};
