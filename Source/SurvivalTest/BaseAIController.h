#pragma once
#include "CoreMinimal.h"
#include "AIController.h"
#include "EnvironmentQuery/EnvQueryManager.h"
#include "Events/EventListener.h"
#include "Navigation/CrowdFollowingComponent.h"
#include "Perception/AIPerceptionTypes.h"
#include "Navigation/PathFollowingComponent.h"
#include "BaseAIController.generated.h"

class ABaseCharacter;
class IDamagable;
class UAISenseConfig_Sight;
class UEnvQuery;
class APatrolPath;

#define mAsBaseCharacter(character) Cast<ABaseCharacter>(character)

UCLASS()
class SURVIVALTEST_API ABaseAIController : public AAIController, public IEventListener
{
	GENERATED_BODY()

public:
	ABaseAIController();

	UFUNCTION()
	void TargetPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus);

	/*explicit ABaseAIController(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get())
	{
		Super(ObjectInitializer.SetDefaultSubobjectClass<UCrowdFollowingComponent>(TEXT("PathFollowingComponent")));
	}*/

	void WeaponLocationQueryFinished(TSharedPtr<FEnvQueryResult> Result);
	void MoveToCombatLocation();

	virtual void Tick(float DeltaTime) override;
	void Patrol();
	void KillAI();
	void CalculateCombat();
	void AttackWithWeapon(FRotator FireDirection);
	void LookAt(FVector lookAtLocation);
	
	void MoveComplete(FAIRequestID RequestID, const FPathFollowingResult& result);

	virtual void OnPossess(APawn* aPawn) override;
	void FindNewTarget();
	virtual void EventTriggered(UBaseEvent* inEvent) override;

	FVector GetLastKnowLocation() { return lastKnowLocation; }

	IDamagable* GetTarget() { return target; }

	ABaseCharacter* GetBaseCharacter() { return AICharacter; }

private:
	IDamagable* target;
	FVector lastKnowLocation;
	bool canSee = false;
	bool finishedMove = true;

	UPROPERTY()
	UAISenseConfig_Sight* sightConfig;

	UPROPERTY()
	ABaseCharacter* AICharacter;

	UPROPERTY()
	UEnvQuery* FindWeaponLocationQuery;

	UPROPERTY()
	FEnvQueryRequest FindViableCombatLocationRequest;

	UPROPERTY()
	APatrolPath* currentPath;

	int32 currentPathPoint;
};
