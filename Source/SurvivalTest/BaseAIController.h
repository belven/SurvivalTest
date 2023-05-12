#pragma once
#include "CoreMinimal.h"
#include "AIController.h"
#include "EnvironmentQuery/EnvQueryManager.h"
#include "Events/EventListener.h"
#include "Items/Weapon.h"
#include "Navigation/CrowdFollowingComponent.h"
#include "Perception/AIPerceptionTypes.h"
#include "Navigation/PathFollowingComponent.h"
#include "BaseAIController.generated.h"

class ABaseCharacter;
class IDamagable;
class UAISenseConfig_Sight;
class UEnvQuery;
class APatrolPath;
class UProjectileWeapon;

#define mAsBaseCharacter(character) Cast<ABaseCharacter>(character)

UCLASS()
class SURVIVALTEST_API ABaseAIController : public AAIController, public IEventListener
{
	GENERATED_BODY()

public:
	ABaseAIController();

	UFUNCTION()
	void TargetPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus);

	FVector GetLastKnowLocation() { return lastKnowLocation; }
	IDamagable* GetTarget() { return target; }
	ABaseCharacter* GetBaseCharacter() { return AICharacter; }

	TArray<ABaseCharacter*> alliesSeen;
	bool isInactive;

protected:
	void WeaponLocationQueryFinished(TSharedPtr<FEnvQueryResult> Result);
	void DetermineNextAction();
	void MoveToCombatLocation();
	void GetNearbyAmmo();
	bool FindAllyWithAmmo();
	void EquipKnife();

	void GetAmmo();
	void Inactive();
	virtual void Tick(float DeltaTime) override;
	void GetPatrolPath();
	void Patrol();
	void KillAI();
	bool IsInWeaponsRange(float dist);
	void AttackWithWeapon();
	void CalculateCombat();
	FVector IncreaseVectorHeight(FVector location, int32 increase);
	bool HasAmmoForWeapon();
	void Reload();
	FVector GetPredictedLocation(AActor* actor);
	void AttackWithWeapon(FRotator FireDirection);
	void LookAt(FVector lookAtLocation);
	virtual void BeginPlay() override;
	void MoveComplete(FAIRequestID RequestID, const FPathFollowingResult& result);

	UFUNCTION()
	void OutOfAmmo();

	UFUNCTION()
	void WeaponReady();

	bool HasAmmo(ABaseCharacter* other);

	UFUNCTION()
	void ReloadComplete();

	UFUNCTION()
	void WeaponEquipped(UWeapon* oldWeapon);

	UFUNCTION()
	void NavDone(ANavigationData* inNavData);
	UFUNCTION()
	virtual void OnPossess(APawn* aPawn) override;
	void FindNewTarget();
	virtual void EventTriggered(UBaseEvent* inEvent) override;


private:
	IDamagable* target;
	FVector lastKnowLocation;
	bool canSee = false;
	bool needsAmmo = false;
	FTimerHandle TimerHandle_Inactive;
	FTimerHandle TimerHandle_DetemineAction;


	UPROPERTY()
	UProjectileWeapon* projectileWeapon;

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
