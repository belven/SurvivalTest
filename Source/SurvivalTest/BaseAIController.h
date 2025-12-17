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

DECLARE_LOG_CATEGORY_EXTERN(AIErrorLog, Error, All)
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
	void WeaponReady();

	UFUNCTION()
	void Reload();

	UFUNCTION()
	void OutOfAmmo();

	UFUNCTION()
	void WeaponEquipped(UWeapon* oldWeapon);

	virtual FPathFollowingRequestResult MoveTo(const FAIMoveRequest& MoveRequest, FNavPathSharedPtr* OutPath) override;

	bool IsIsAttacking() const { return isAttacking; }
	void SetIsAttacking(bool inIsAttacking);

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
	bool FindAllyWithAmmo();
	int32 GetKnifeID();
	void GetNearbyAmmo();
	bool HasAmmo(ABaseCharacter* other);

	void Inactive();
	void GetPatrolPath();
	void Patrol();
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

	UBaseGameInstance* GetBaseGameInstance();

private:
	static int32 KNIFE_ITEM_ID;
	float inactiveTimerDuration;

	bool canSee = false;
	bool needsAmmo = false;
	bool isAttacking = false;

	int32 currentPathPoint;
	int32 acceptanceRadius = 100;

	IDamagable* target;
	FVector lastKnowLocation;

	FTimerHandle TimerHandle_Inactive;
	FTimerHandle TimerHandle_DetermineAction;

	FPathFollowingResult lastMoveResult;

	UPROPERTY()
	UProjectileWeapon* projectileWeapon;

	UPROPERTY()
	UAISenseConfig_Sight* sightConfig;

	UPROPERTY()
	ABaseCharacter* AICharacter;

	UPROPERTY()
	UBaseGameInstance* baseGameInstance;

	UPROPERTY()
	UEnvQuery* FindWeaponLocationQuery;

	UPROPERTY()
	FEnvQueryRequest FindViableCombatLocationRequest;

	UPROPERTY()
	APatrolPath* currentPath;

};