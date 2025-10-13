#pragma once
#include "CoreMinimal.h"
#include "AIController.h"
#include "EnvironmentQuery/EnvQueryManager.h"
#include "Events/EventListener.h"
#include "Items/Weapon.h"
#include "Navigation/CrowdFollowingComponent.h"
#include "Perception/AIPerceptionTypes.h"
#include "Navigation/PathFollowingComponent.h"
#include "Tasks/ReloadTask.h"
#include "BaseAIController.generated.h"

class UEquipmentSwapTask;
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
	bool SolveBallisticArc(const FVector& StartLocation, const FVector& TargetLocation, float LaunchSpeed, float GravityZ, FRotator& OutRotation);

	UFUNCTION()
	void TargetPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus);
	virtual void OnMoveCompleted(FAIRequestID RequestID, const FPathFollowingResult& Result) override;
	FVector GetLastKnowLocation() { return lastKnowLocation; }
	IDamagable* GetTarget() { return target; }
	ABaseCharacter* GetBaseCharacter() { return AICharacter; }

	UPROPERTY()
	TArray<ABaseCharacter*> alliesSeen;
	bool isInactive;

protected:
	void WeaponLocationQueryFinished(TSharedPtr<FEnvQueryResult> Result);
	void DetermineNextAction();
	void MoveToCombatLocation();
	void GetNearbyAmmo();
	void StartSprinting();
	bool FindAllyWithAmmo();
	void EquipKnife();
	void EquipWeaponAtSlot(int32 slot, EGearType type);

	void GetAmmo();
	void Inactive();
	void GetPatrolPath();
	void Patrol();
	bool IsInWeaponsRange(float dist);
	void AttackWithWeapon();
	void CalculateCombat();
	FVector IncreaseVectorHeight(const FVector& location, int32 increase);
	bool HasAmmoForWeapon();
	void Reload();
	FVector GetPredictedLocation(AActor* actor);
	void AttackWithWeapon(const FRotator& FireDirection);
	void LookAt(const FVector& lookAtLocation);
	virtual void BeginPlay() override;

	UFUNCTION()
	void CharacterDied(ABaseCharacter* deadCharacter);

	UFUNCTION()
	void OutOfAmmo();

	UFUNCTION()
	void WeaponReady();

	bool HasAmmo(ABaseCharacter* other);

	UFUNCTION()
	void ReloadComplete();
	bool HasRangedWeapon();

	UFUNCTION()
	void WeaponEquipped(UWeapon* oldWeapon);

	UFUNCTION()
	void NavDone(ANavigationData* inNavData);
	UFUNCTION()
	virtual void OnPossess(APawn* aPawn) override;
	void FindNewTarget();

	UFUNCTION()
	virtual void EventTriggered(UBaseEvent* inEvent) override;

	UPROPERTY()
	UEquipmentSwapTask* equipmentSwapTask;

public:
	virtual FPathFollowingRequestResult MoveTo(const FAIMoveRequest& MoveRequest, FNavPathSharedPtr* OutPath) override;

private:
	IDamagable* target;
	FVector lastKnowLocation;
	bool canSee = false;
	bool needsAmmo = false;
	FTimerHandle TimerHandle_Inactive;
	FTimerHandle TimerHandle_DetermineAction;
	float inactiveTimerDuration;

	UPROPERTY()
	UReloadTask* reloadTask;

	FPathFollowingResult lastMoveResult;

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
	int acceptanceRadius = 400;
};