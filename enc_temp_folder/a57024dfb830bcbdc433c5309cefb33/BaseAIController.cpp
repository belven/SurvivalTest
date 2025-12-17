#include "BaseAIController.h"
#include "Kismet/GameplayStatics.h"
#include "Events/BaseEvent.h"
#include "Events/HealthChangeEvent.h"
#include "Events/RPGEventManager.h"
#include "BaseCharacter.h"
#include "Interfaces/Damagable.h"
#include "BaseGameInstance.h"
#include "HelperFunctions.h"
#include "NavigationSystem.h"
#include "Components/CombatComponent.h"
#include "Components/SplineComponent.h"
#include "EnvironmentQuery/EnvQuery.h"
#include "EnvironmentQuery/EnvQueryManager.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Items/ProjectileWeapon.h"
#include "Items/Weapon.h"
#include "Kismet/KismetMathLibrary.h"
#include "Missions/PatrolPath.h"
#include "Navigation/CrowdFollowingComponent.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISenseConfig_Sight.h"
#include "Tasks/EquipmentSwapTask.h"
#include "Tasks/TaskManagerComponent.h"

DEFINE_LOG_CATEGORY(AIErrorLog)
DEFINE_LOG_CATEGORY(AIIssue)
DEFINE_LOG_CATEGORY(AILog)

int32 ABaseAIController::KNIFE_ITEM_ID = UItemStructs::InvalidInt;

ABaseAIController::ABaseAIController() : Super(FObjectInitializer::Get())
{
	constexpr int32 range = 13000;
	isInactive = true;
	currentPath = nullptr;
	currentPathPoint = 0;

	// Initialize pointers to nullptr for safety
	PerceptionComponent = nullptr;
	sightConfig = nullptr;
	FindWeaponLocationQuery = nullptr;

	// Create and configure perception components
	PerceptionComponent = CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("AIPerceptionComponent"));
	sightConfig = CreateDefaultSubobject<UAISenseConfig_Sight>(TEXT("SightConfig"));

	// Set up sight config for AI perception
	sightConfig->SightRadius = range * 0.9;
	sightConfig->LoseSightRadius = range;
	sightConfig->PeripheralVisionAngleDegrees = 100.0f;

	// This section is important, as without setting at least bDetectNeutrals to true, the AI will never perceive anything
	// Still not tried to set this up correctly at all
	sightConfig->DetectionByAffiliation.bDetectEnemies = true;
	sightConfig->DetectionByAffiliation.bDetectFriendlies = true;
	sightConfig->DetectionByAffiliation.bDetectNeutrals = true;

	PerceptionComponent->SetDominantSense(sightConfig->GetSenseImplementation());
	PerceptionComponent->ConfigureSense(*sightConfig);
	PerceptionComponent->OnTargetPerceptionUpdated.AddDynamic(this, &ABaseAIController::TargetPerceptionUpdated);

	static ConstructorHelpers::FObjectFinder<UEnvQuery> PlayerLocationQueryObj(TEXT("EnvQuery'/Game/FirstPerson/EQS_FindPlayer.EQS_FindPlayer'"));
	if (PlayerLocationQueryObj.Succeeded())
	{
		FindWeaponLocationQuery = PlayerLocationQueryObj.Object;

		// Set up our EQS query 
		FindViableCombatLocationRequest = FEnvQueryRequest(FindWeaponLocationQuery, this);
	}
	else
	{
		UE_LOG(AIErrorLog, Error, TEXT("Failed to find EQS_FindPlayer EnvQuery asset."));
	}
}

void ABaseAIController::LookAt(const FVector& lookAtLocation)
{
	FRotator lookAt = UKismetMathLibrary::FindLookAtRotation(mActorLocation, lookAtLocation);
	lookAt.Pitch = mActorRotation.Pitch;
	lookAt.Roll = mActorRotation.Roll;
	GetCharacter()->FaceRotation(lookAt);
}

void ABaseAIController::BeginPlay()
{
	Super::BeginPlay();
	DetermineNextAction();
}

void ABaseAIController::CharacterDied(ABaseCharacter* deadCharacter)
{
	GetWorld()->GetTimerManager().ClearAllTimersForObject(this);
	deadCharacter->GetMovementComponent()->StopMovementImmediately();
	SetActorTickEnabled(false);
	GetBaseGameInstance()->GetEventManager()->OnEventTriggered.RemoveAll(this);
	PerceptionComponent->OnTargetPerceptionUpdated.RemoveAll(this);
	OnStopUsingTool.Broadcast();
	UnPossess();
}

void ABaseAIController::OutOfAmmo()
{
	if (!HasAmmoForWeapon())
	{
		UE_LOG(AILog, Log, TEXT("AI %s OutOfAmmo"), *AICharacter->GetCharacterName());
		needsAmmo = true;
		GetAmmo();
		mOnScreenMessage(TEXT("ABaseAIController OnOutOfAmmo No Ammo"));
	}
}

void ABaseAIController::WeaponEquipped(UWeapon* oldWeapon)
{
	UWeapon* weapon = oldWeapon;

	if (weapon)
	{
		weapon->OnWeaponReady.RemoveAll(this);

		if (weapon->IsProjectileWeapon())
		{
			UProjectileWeapon* pw = Cast<UProjectileWeapon>(weapon);
			pw->OnOutOfAmmo.RemoveAll(this);
		}
	}

	weapon = mCurrentWeapon();

	if (weapon)
	{
		weapon->OnWeaponReady.AddUniqueDynamic(this, &ABaseAIController::WeaponReady);

		if (weapon->IsProjectileWeapon())
		{
			UProjectileWeapon* pw = Cast<UProjectileWeapon>(weapon);
			pw->OnOutOfAmmo.AddUniqueDynamic(this, &ABaseAIController::OutOfAmmo);
			projectileWeapon = pw;
		}
	}
}

bool ABaseAIController::HasAmmo(ABaseCharacter* other)
{
	int32 ammoID = projectileWeapon->GetProjectileWeaponData().ammoID;
	return projectileWeapon && other->GetInventory()->GetItemAmount(ammoID) > 0;
}

bool ABaseAIController::HasRangedWeapon()
{
	return projectileWeapon != NULL;
}

void ABaseAIController::NavDone(ANavigationData* inNavData)
{
	DetermineNextAction();
}

void ABaseAIController::OnPossess(APawn* aPawn)
{
	Super::OnPossess(aPawn);
	AICharacter = mAsBaseCharacter(aPawn);
	AICharacter->OnCharacterDied.AddUniqueDynamic(this, &ABaseAIController::CharacterDied);

	UNavigationSystemV1::GetCurrent(GetWorld())->OnNavigationGenerationFinishedDelegate.AddUniqueDynamic(this, &ABaseAIController::NavDone);
	GetBaseGameInstance()->GetEventManager()->OnEventTriggered.AddUniqueDynamic(this, &ABaseAIController::EventTriggered);

	// Add the AIs character to things that can be perceived by this sight config.
	UAIPerceptionSystem::RegisterPerceptionStimuliSource(this, sightConfig->GetSenseImplementation(), aPawn);

	AddInstanceComponent(UCombatComponent::CreateCombatComponent(this, AICharacter));

	DetermineNextAction();
}

// ReSharper disable once CppPassValueParameterByConstReference
void ABaseAIController::TargetPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus)
{
	// Is the actor our current target?
	if (*target == Actor)
	{
		// Update the lastKnowLocation, as regardless of if we see the target or not, we might re-adjust our movement to a better location
		lastKnowLocation = Stimulus.StimulusLocation;

		// Did we loose sight of them?
		if (!Stimulus.WasSuccessfullySensed())
		{
			// We can't see them
			canSee = false;

			// Move to their last seen location
			MoveToCombatLocation();
		}
		// Have we seen them again?
		else
		{
			// We can see them again
			canSee = true;
			DetermineNextAction();
		}
	}
	// If we don't have a target, then check if this is a new viable target
	else
	{
		// Get the Actors team, if it has one, and check if we're enemies
		ITeam* otherTeam = Cast<ITeam>(Actor);

		if (otherTeam != NULL)
		{
			// Are we enemies with the perceived actor?
			if (AICharacter->GetRelationship(otherTeam, GetBaseGameInstance()) == ERelationshipType::Enemy)
			{
				if (target == NULL)
				{
					// Update our target and set that we can see them, we can assume that, if the actor is a team, it's also damagable
					IDamagable* damagable = Cast<IDamagable>(Actor);

					if (damagable->IsAlive())
					{
						target = damagable;
						canSee = true;
						DetermineNextAction();
						UE_LOG(AILog, Log, TEXT("AI %s found a hostile target"), *AICharacter->GetCharacterName());
					}
				}
			}
			else if (!alliesSeen.Contains(Actor) && AICharacter->GetRelationship(otherTeam, GetBaseGameInstance()) == ERelationshipType::Ally)
			{
				UE_LOG(AILog, Log, TEXT("AI %s found an ally"), *AICharacter->GetCharacterName());
				alliesSeen.Add(Cast<ABaseCharacter>(Actor));
			}
		}
	}
}

void ABaseAIController::OnMoveCompleted(FAIRequestID RequestID, const FPathFollowingResult& Result)
{
	lastMoveResult = Result;

	if (!Result.IsSuccess() && (Result.Code == EPathFollowingResult::Invalid || Result.Code == EPathFollowingResult::Blocked))
	{
		UE_LOG(AIIssue, Warning, TEXT("OnMoveCompleted Failed"));
	}

	mSetTimer(TimerHandle_DetermineAction, &ABaseAIController::DetermineNextAction, 1.0f);
}

void ABaseAIController::SetIsAttacking(bool inIsAttacking)
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

// ReSharper disable once CppPassValueParameterByConstReference
void ABaseAIController::WeaponLocationQueryFinished(TSharedPtr<FEnvQueryResult> Result)
{
	// Did we find a new location to move to?
	if (Result->IsSuccessful())
	{
		EPathFollowingRequestResult::Type result = EPathFollowingRequestResult::Failed;

		// Get the first item as a location, this will be the highest scoring location in the array, as it orders items by score

		for (int i = 0; i < Result->Items.Num(); i++)
		{
			const FVector loc = Result->GetItemAsLocation(i);

			// Move to the location found
			result = MoveToLocation(loc, 0);

			if (result == EPathFollowingRequestResult::RequestSuccessful)
			{
				break;
			}
		}

		if (result == EPathFollowingRequestResult::Failed)
		{
			UE_LOG(AIIssue, Warning, TEXT("EPathFollowingRequestResult Failed"));
		}
	}
	else
	{
		target = NULL;
		FindNewTarget();
		UE_LOG(AIIssue, Warning, TEXT("WeaponLocationQueryFinished Failed"));
	}
}

void ABaseAIController::DetermineNextAction()
{
	if (GetBaseCharacter())
	{
		if (GetBaseCharacter()->IsAlive())
		{
			inactiveTimerDuration = 5.0f;

			if (needsAmmo)
			{
				mOnScreenMessage(TEXT("ABaseAIController needsAmmo"));
				GetAmmo();
				isInactive = false;
			}
			else if (target != NULL && target->IsAlive())
			{
				CalculateCombat();
				isInactive = false;
			}
			else if (target != NULL && target->IsDead())
			{
				target = NULL;
				FindNewTarget();
				isInactive = false;
			}
			else if (target == NULL)
			{
				Patrol();
				isInactive = false;
			}
		}
	}

	mSetTimer(TimerHandle_Inactive, &ABaseAIController::Inactive, inactiveTimerDuration);
}

void ABaseAIController::WeaponReady()
{
	//UE_LOG(AILog, Log, TEXT("AI %s WeaponReady"), *AICharacter->GetCharacterName());
	DetermineNextAction();
}

void ABaseAIController::GetPatrolPath()
{
	// Do we already have a patrol path
	if (currentPath == NULL)
	{
		UBaseGameInstance* gi = GetBaseGameInstance();

		if (!gi->paths.IsEmpty())
		{
			// Get the first patrol path from the game instance
			currentPath = gi->paths[0];

			int32 numberOfSplinePoints = currentPath->GetSpline()->GetNumberOfSplinePoints();

			// Check if the path is valid
			if (currentPath != NULL && numberOfSplinePoints < 1)
			{
				currentPath = NULL;

				// Set path point to -1, as there aren't any valid patrol paths, and we don't keep checking every tick
				currentPathPoint = -1;
			}
			else
			{
				currentPathPoint = FMath::RandRange(0, numberOfSplinePoints - 1);
			}
		}
	}
}

void ABaseAIController::Patrol()
{
	// Check if we aren't moving, or we have an invalid Path
	// (the currentPathPoint is set to -1 when we didn't find any patrol paths in the world)
	if (currentPathPoint > -1)
	{
		GetPatrolPath();

		if (currentPath != NULL)
		{
			// Get the spline of the patrol path
			const USplineComponent* spline = currentPath->GetSpline();

			// Ensure the path point doesn't exceed the amount of points
			if (currentPathPoint > spline->GetNumberOfSplinePoints() - 1)
			{
				currentPathPoint = 0;
			}

			// Get the next spline point to move to
			const FVector loc = spline->GetWorldLocationAtSplinePoint(currentPathPoint);

			// Move to the spline point
			MoveToLocation(loc, acceptanceRadius);

			// Increment the path point, to move onto the next one
			// TODO figure out how to handle the AI movement getting stuck
			if (lastMoveResult.IsSuccess())
			{
				currentPathPoint++;
			}
		}
	}
}

bool ABaseAIController::IsInWeaponsRange(float dist)
{
	return dist <= GetBaseCharacter()->GetWeaponRange();
}

void ABaseAIController::AttackWithWeapon()
{
	UWeapon* weapon = mCurrentWeapon();

	// if we have a valid weapon, attack the target
	if (weapon != NULL)
	{
		const FVector predictedLoc = GetPredictedLocation(target->asActor());

		float increase = target->asActor()->GetSimpleCollisionHalfHeight() / 2;
		const FVector targetLocation = UHelperFunctions::IncreaseVectorHeight(predictedLoc, increase);

		LookAt(targetLocation);

		// Check we're in range of the target
		if (IsInWeaponsRange(FVector::Dist(mActorLocation, targetLocation)))
		{
			StopMovement();
			GetBaseCharacter()->StopSprinting();

			if (HasRangedWeapon())
			{
				FProjectileWeaponData pw = projectileWeapon->GetProjectileWeaponData();
				float gravity = pw.gravity * GetWorld()->GetGravityZ();

				SolveBallisticArc(mActorLocation, targetLocation, pw.bulletVelocity, gravity, GetBaseCharacter()->aimRotation);
			}

			if (!isAttacking)
			{
				SetIsAttacking(true);
			}
		}
		// Otherwise move towards the targets current location
		else
		{
			// We updated the lastKnowLocation here, as we can still see the target and simply need to move forwards to attack again
			lastKnowLocation = target->asActor()->GetActorLocation();
			MoveToCombatLocation();
			SetIsAttacking(false);
		}
	}
	// We have no weapon so equip a knife
	else
	{
		SetIsAttacking(false);
		EquipKnife();
		CalculateCombat();
	}
}

bool ABaseAIController::SolveBallisticArc(const FVector& StartLocation, const FVector& TargetLocation, float LaunchSpeed, float GravityZ, FRotator& OutRotation) {
	FVector Delta = TargetLocation - StartLocation;
	FVector DeltaXZ = FVector(Delta.X, Delta.Y, 0.f);
	float DeltaZ = Delta.Z;
	float DeltaXY = DeltaXZ.Size();

	float SpeedSq = LaunchSpeed * LaunchSpeed;
	float Gravity = FMath::Abs(GravityZ); // Ensure gravity is positive
	float Root = SpeedSq * SpeedSq - Gravity * (Gravity * DeltaXY * DeltaXY + 2 * DeltaZ * SpeedSq);

	if (Root < 0)
	{
		// No solution
		return false;
	}

	float RootSqrt = FMath::Sqrt(Root);

	// Low angle solution
	float Angle = FMath::Atan((SpeedSq - RootSqrt) / (Gravity * DeltaXY));

	FVector DirectionXY = DeltaXZ.GetSafeNormal();
	FVector LaunchVelocity = LaunchSpeed * (DirectionXY * FMath::Cos(Angle) + FVector::UpVector * FMath::Sin(Angle));

	OutRotation = LaunchVelocity.Rotation();
	return true;
}

void ABaseAIController::CalculateCombat()
{
	// Can we see our current target?
	if (canSee)
	{
		AttackWithWeapon();
	}
	// We can't see the target, make sure we're not already trying to move to the target
	else
	{
		SetIsAttacking(false);

		// Move to the last known location
		LookAt(lastKnowLocation);

		// TODO Implement Searching
		lastKnowLocation = target->asActor()->GetActorLocation();
		MoveToCombatLocation();
	}
}


bool ABaseAIController::HasAmmoForWeapon()
{
	bool result = false;

	if (HasRangedWeapon())
	{
		int32 ammoID = projectileWeapon->GetProjectileWeaponData().ammoID;
		int32 ammoQuantity = GetBaseCharacter()->GetInventory()->GetItemAmount(ammoID);
		result = ammoQuantity > 0;
	}

	return result;
}

void ABaseAIController::Reload()
{
	OnReload.Broadcast();
}

FVector ABaseAIController::GetPredictedLocation(AActor* actor)
{
	float lead = 1;
	FVector actorLocation = actor->GetActorLocation();

	if (HasRangedWeapon())
	{
		lead = projectileWeapon->GetProjectileWeaponData().bulletVelocity;
	}

	// TODO Do we create an AI accuracy stat to use here?
	// time = FMath::RandRange(time * 0.9f, time * 1.1f);

	float time = FVector::Dist(GetBaseCharacter()->GetActorLocation(), actorLocation) / lead;
	return actorLocation + (actor->GetVelocity() * time);
}

void ABaseAIController::MoveToCombatLocation()
{
	FindViableCombatLocationRequest.Execute(EEnvQueryRunMode::RandomBest5Pct, this, &ABaseAIController::WeaponLocationQueryFinished);
	StartSprinting();
}

/**
 * The AI will check the inventories of nearby dead characters, to try and get ammo from them.
 */
void ABaseAIController::GetNearbyAmmo()
{
	for (IInteractable* inter : GetBaseCharacter()->GetOverlappingInteractables())
	{
		ABaseCharacter* other = Cast<ABaseCharacter>(inter);
		bool weaponFound = false;

		if (other && other->IsDead())
		{
			for (auto iid : other->GetInventory()->GetItems())
			{
				bool isAmmoForWeapon = iid.itemID == projectileWeapon->GetProjectileWeaponData().ammoID;

				FItemData id = GetBaseGameInstance()->GetItemData(iid.itemID);

				TArray<int32> ammoIDs;

				if (id.type == EItemType::Ammo)
				{
					UE_LOG(AILog, Log, TEXT("AI %s found ammo from a body"), *AICharacter->GetCharacterName());
					GetBaseCharacter()->GetInventory()->TransferItem(other->GetInventory(), iid, UItemStructs::InvalidInt);
					needsAmmo = false;
					FString text = "AI " + AICharacter->GetCharacterName() + " found ammo from a body";
					mOnScreenMessage(*text);

					if (ammoIDs.Contains(id.ID) || isAmmoForWeapon)
					{
						needsAmmo = false;
					}
				}
				else if (iid.itemID != GetKnifeID() && id.type == EItemType::Weapon)
				{
					FRangedWeaponData rwd = GetBaseGameInstance()->GetRangedWeaponData(id.ID);
					FProjectileWeaponData pwd = GetBaseGameInstance()->GetProjectileWeaponData(rwd.ID);
					FInstanceWeaponData iwd = GetBaseGameInstance()->GetInstanceWeaponDataByInstanceItemID(iid.ID);

					if ((iwd.ammo > 0 || other->GetInventory()->GetItemAmount(pwd.ammoID)) && !weaponFound)
					{
						UE_LOG(AILog, Log, TEXT("AI %s found a weapon from a body"), *AICharacter->GetCharacterName());
						GetBaseCharacter()->GetInventory()->TransferItem(other->GetInventory(), iid, projectileWeapon->GetInstanceItemData().slot);
						FString text = "AI " + AICharacter->GetCharacterName() + " found a " + id.name + " on " + other->GetCharacterName() + "s body";
						mOnScreenMessage(*text);
						ammoIDs.Add(pwd.ammoID);
						weaponFound = true;
					}
				}
			}
		}
	}
}

/**
 * Starts the AI sprinting, if it's not already sprinting
 */
void ABaseAIController::StartSprinting()
{
	if (!GetBaseCharacter()->IsSprinting())
	{
		GetBaseCharacter()->StartSprinting();
	}
}

/**
 * Looks through all the seen allies, trying to find replacement ammo and weapons from any that are dead.
 *
 * @return True, if an ally with any ammo or weapons we can use
 */
bool ABaseAIController::FindAllyWithAmmo()
{
	bool result = false;

	for (auto ally : alliesSeen)
	{
		if (ally && ally->IsDead() && HasAmmo(ally) && FVector::Dist(mActorLocation, ally->GetActorLocation()) < 10000)
		{
			UE_LOG(AILog, Log, TEXT("AI %s found ammo on ally %s"), *AICharacter->GetCharacterName(), *ally->GetCharacterName());
			MoveToLocation(ally->GetActorLocation(), ABaseCharacter::interactionRadius * 0.7);
			StartSprinting();
			result = true;
		}
	}
	return result;
}

int32 ABaseAIController::GetKnifeID()
{
	if (KNIFE_ITEM_ID == UItemStructs::InvalidInt)
	{
		FItemData id = GetBaseGameInstance()->GetItemDataByName("Knife");

		if (id.ID != UItemStructs::InvalidInt)
		{
			KNIFE_ITEM_ID = id.ID;
		}
		else
		{
			UE_LOG(AIErrorLog, Error, TEXT("Knife ID not found"));
		}
	}
	return KNIFE_ITEM_ID;
}

/**
 * Equips the AI with a knife in their inventory, assuming they have one
 */
void ABaseAIController::EquipKnife()
{
	needsAmmo = false;
	bool knifeEquipped = false;

	// Find the ID for the knife. It might change, so we'll search by name. Once set, we don't find it again

	if (GetKnifeID() != UItemStructs::InvalidInt)
	{
		// Get the AIs inventory items
		for (auto iid : GetBaseCharacter()->GetInventory()->GetItems())
		{
			// Find the knife, all AI should have one by default
			if (iid.itemID == GetKnifeID())
			{
				EquipWeaponAtSlot(iid.slot, EGearType::Weapon);
				knifeEquipped = true;
				DetermineNextAction();
				break;
			}
		}
	}

	if (!knifeEquipped)
	{
		GetBaseCharacter()->GetInventory()->SetEquippedWeapon(nullptr);
	}
}

void ABaseAIController::EquipWeaponAtSlot(int32 slot, EGearType type)
{
	if (!equipmentSwapTask)
	{
		equipmentSwapTask = NewObject<UEquipmentSwapTask>();
	}

	equipmentSwapTask->SetSlot(slot);

	GetBaseCharacter()->GetTaskManager()->PerformTask(equipmentSwapTask, false);
}

void ABaseAIController::GetAmmo()
{
	// See if there's any allies with ammo nearby
	GetNearbyAmmo();

	// Do we still need ammo?
	if (needsAmmo)
	{
		// Find an ally we've seen, with ammo
		if (!FindAllyWithAmmo())
		{
			// We didn't find a dead ally with ammo, so equip our knife
			EquipKnife();
		}
	}
	else
	{
		Reload();
	}
}

void ABaseAIController::Inactive()
{
	isInactive = true;
	SetIsAttacking(false);
	DetermineNextAction();
}

void ABaseAIController::FindNewTarget()
{
	TArray<AActor*> actorsSensed;

	// Get all the actors we've sensed in our lifetime
	PerceptionComponent.Get()->GetKnownPerceivedActors(sightConfig->GetSenseImplementation(), actorsSensed);

	for (AActor* actor : actorsSensed)
	{
		if (actor->Implements<UTeam>() && actor->Implements<UDamagable>())
		{
			ITeam* team = Cast<ITeam>(actor);
			IDamagable* damagable = Cast<IDamagable>(actor);

			// Check if the actor is both damagable and an enemy
			if (damagable->IsAlive()
				&& AICharacter->GetRelationship(team, GetBaseGameInstance()) == ERelationshipType::Enemy)
			{
				// Move to the targets last known location
				// As the target is set in here, the code in tick, in the canSee = false section, will update the lastKnowLocation to their current location
				// This means the AI will move to the targets exact location, rather than just their last location
				target = damagable;
				lastKnowLocation = PerceptionComponent.Get()->GetActorInfo(*actor)->GetLastStimulusLocation();
				MoveToLocation(lastKnowLocation, acceptanceRadius);
				StartSprinting();
				return;
			}
		}
	}

	DetermineNextAction();
}

void ABaseAIController::EventTriggered(UBaseEvent* inEvent)
{
	// Check if the event is a post health change
	if (inEvent->GetEventType() == EEventType::PostHealthChange)
	{
		UHealthChangeEvent* hce = Cast<UHealthChangeEvent>(inEvent);
		bool wasThisAnAttack = !hce->GetChange().heals;

		// If our target is NULL, we can check if we're being attacked and maybe assign a new target
		if (target == NULL)
		{
			// The UHealthChangeEvent EventOwner is the one who the health change affected.
			// Therefore, if the event owner is our controller character, then we've either been attacked or healed
			bool hasOurCharacterBeenAttacked = hce->GetEventOwner() == GetBaseCharacter();
			bool isTheSourceStillAlive = hce->GetChange().source->IsAlive();

			// Only trigger after health changed, the change isn't a heal and the owner of change is our Pawn and if the source is alive still
			// It's possible we take damage from a dead source
			if (wasThisAnAttack
				&& hasOurCharacterBeenAttacked
				&& isTheSourceStillAlive)
			{
				target = hce->GetChange().source;
				DetermineNextAction();
			}
		}
		// Check if our target has died and if we therefore need a new one
		// Check if the change is damage
		// Check if the change is against our target
		// Check if our target is dead
		else if (wasThisAnAttack
			&& hce->GetEventOwner() == *target
			&& target->IsDead())
		{
			// clear sight
			canSee = false;
			SetIsAttacking(false);

			// Try to find a new target
			FindNewTarget();
		}
		// Maybe add function to shoot back at a new target, if we get shot by something else
		//else if (wasThisAnAttack	&& !canSee && hce->GetEventOwner() != *target && target->IsAlive())
		//{
		//	target = hce->GetChange().source;
		//	//LookAt(target->asActor()->GetActorLocation());
		//	DetermineNextAction();
		//}
	}
}

UBaseGameInstance* ABaseAIController::GetBaseGameInstance()
{
	if (baseGameInstance == NULL)
	{
		if (GetBaseCharacter())
		{
			baseGameInstance = GetBaseCharacter()->GetGame();
		}
		else if (GetWorld())
		{
			baseGameInstance = mGameInstance();
		}
	}
	return baseGameInstance;
}

FPathFollowingRequestResult ABaseAIController::MoveTo(const FAIMoveRequest& MoveRequest, FNavPathSharedPtr* OutPath)
{
	//FPathFollowingRequestResult res = Super::MoveTo(MoveRequest, OutPath);
	//inactiveTimerDuration = OutPath->Get()->GetLength() / (GetBaseCharacter()->GetMovementComponent()->GetMaxSpeed() * 1.2);
	//return res;
	// TODO figure out inactive timer based on travel time
	inactiveTimerDuration = 15.0f;
	return Super::MoveTo(MoveRequest, OutPath);
}
