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

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FUseTool);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FStopUsingTool);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnReload);

#define mAsBaseCharacter(character) Cast<ABaseCharacter>(character)

DECLARE_LOG_CATEGORY_EXTERN(AIError, Error, All)
DECLARE_LOG_CATEGORY_EXTERN(AILog, Log, All)
DECLARE_LOG_CATEGORY_EXTERN(AIIssue, Warning, All)

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

	UPROPERTY()
	FUseTool OnUseTool;

	UPROPERTY()
	FStopUsingTool OnStopUsingTool;

	UPROPERTY()
	FOnReload OnReload;

	UFUNCTION()
	void ReloadComplete();

	UFUNCTION()
	void WeaponReady();

	UFUNCTION()
	void Reload();
protected:
	void WeaponLocationQueryFinished(TSharedPtr<FEnvQueryResult> Result);
	void DetermineNextAction();
	void MoveToCombatLocation();
	void StartSprinting();
	void EquipKnife();
	void EquipWeaponAtSlot(int32 slot, EGearType type);

	bool IsInWeaponsRange(float dist);
	void AttackWithWeapon();
	void CalculateCombat();

	bool HasAmmoForWeapon();
	void GetAmmo();
	void OutOfAmmo();
	bool FindAllyWithAmmo();
	void GetNearbyAmmo();
	bool HasAmmo(ABaseCharacter* other);

	void Inactive();
	void GetPatrolPath();
	void Patrol();
	FVector IncreaseVectorHeight(const FVector& location, int32 increase);
	FVector GetPredictedLocation(AActor* actor);
	void LookAt(const FVector& lookAtLocation);
	virtual void BeginPlay() override;

	UFUNCTION()
	void CharacterDied(ABaseCharacter* deadCharacter);

	UFUNCTION()
	bool HasRangedWeapon();

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
	bool isAttacking = false;
	static int32 KNIFE_ITEM_ID;

public:
	bool IsIsAttacking() const
	{
		return isAttacking;
	}

	void SetIsAttacking(bool inIsAttacking)
	{
		isAttacking = inIsAttacking;

		if (isAttacking)
		{
			OnUseTool.Broadcast();
		}
		else
		{
			OnStopUsingTool.Broadcast();
		}
	}

private:
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