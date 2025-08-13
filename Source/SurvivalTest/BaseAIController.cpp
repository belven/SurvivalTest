#include "BaseAIController.h"
#include "BaseProjectile.h"
#include "Kismet/GameplayStatics.h"
#include "Events/BaseEvent.h"
#include "Events/HealthChangeEvent.h"
#include "Events/RPGEventManager.h"
#include "BaseCharacter.h"
#include "Interfaces/Damagable.h"
#include "BaseGameInstance.h"
#include "NavigationSystem.h"
#include "AI/NavigationSystemBase.h"
#include "Components/SplineComponent.h"
#include "EnvironmentQuery/EnvQuery.h"
#include "EnvironmentQuery/EnvQueryManager.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Items/ProjectileWeapon.h"
#include "Items/Weapon.h"
#include "Items/WeaponCreator.h"
#include "Kismet/KismetMathLibrary.h"
#include "Missions/PatrolPath.h"
#include "Navigation/CrowdFollowingComponent.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISenseConfig_Sight.h"

ABaseAIController::ABaseAIController() : Super()
{
	// Initialize pointers to nullptr for safety
	PerceptionComponent = nullptr;
	sightConfig = nullptr;
	FindWeaponLocationQuery = nullptr;

	// Create and configure perception components
	PerceptionComponent = CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("AIPerceptionComponent"));
	sightConfig = CreateDefaultSubobject<UAISenseConfig_Sight>(TEXT("SightConfig"));

	if (PerceptionComponent && sightConfig)
	{
		PerceptionComponent->SetDominantSense(sightConfig->GetSenseImplementation());
		PerceptionComponent->ConfigureSense(*sightConfig);
		PerceptionComponent->OnTargetPerceptionUpdated.AddDynamic(this, &ABaseAIController::TargetPerceptionUpdated);
	}

	// Use FindObject instead of FObjectFinder for runtime safety (FObjectFinder is for editor only)
	static ConstructorHelpers::FObjectFinder<UEnvQuery> PlayerLocationQueryObj(TEXT("EnvQuery'/Game/FirstPerson/EQS_FindPlayer.EQS_FindPlayer'"));
	if (PlayerLocationQueryObj.Succeeded())
	{
		FindWeaponLocationQuery = PlayerLocationQueryObj.Object;
	}
	else
	{
		// Optionally log a warning if the query asset is missing
		UE_LOG(LogTemp, Warning, TEXT("Failed to find EQS_FindPlayer EnvQuery asset."));
	}

	currentPath = nullptr;
	currentPathPoint = 0;
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

void ABaseAIController::OutOfAmmo()
{
	if (HasAmmoForWeapon())
	{
		Reload();
	}
	else
	{
		needsAmmo = true;
		GetAmmo();
	}
}

bool ABaseAIController::HasAmmo(ABaseCharacter* other)
{
	int32 ammoID = projectileWeapon->GetProjectileWeaponData().ammoID;
	return projectileWeapon && other->GetInventory()->GetItemAmount(ammoID) > 0;
}

void ABaseAIController::ReloadComplete()
{
	DetermineNextAction();
}

void ABaseAIController::WeaponEquipped(UWeapon* oldWeapon)
{
	if (oldWeapon && oldWeapon->GetWeaponData().type == EWeaponType::Projectile)
	{
		UProjectileWeapon* pw = Cast<UProjectileWeapon>(oldWeapon);
		pw->OnOutOfAmmo.RemoveAll(this);
		pw->OnReloadComplete.RemoveAll(this);
	}

	UWeapon* weapon = GetBaseCharacter()->GetEquippedWeapon();

	if (weapon)
	{
		weapon->OnWeaponReady.AddUniqueDynamic(this, &ABaseAIController::WeaponReady);

		if (weapon->GetWeaponData().type == EWeaponType::Projectile)
		{
			projectileWeapon = Cast<UProjectileWeapon>(weapon);
			projectileWeapon->OnOutOfAmmo.AddUniqueDynamic(this, &ABaseAIController::OutOfAmmo);
			projectileWeapon->OnReloadComplete.AddUniqueDynamic(this, &ABaseAIController::ReloadComplete);
		}
	}
}

void ABaseAIController::NavDone(ANavigationData* inNavData)
{
	DetermineNextAction();
}

void ABaseAIController::OnPossess(APawn* aPawn)
{
	Super::OnPossess(aPawn);
	AICharacter = mAsBaseCharacter(aPawn);
	AICharacter->OnWeaponEquipped.AddUniqueDynamic(this, &ABaseAIController::WeaponEquipped);
	if (AICharacter->GetEquippedWeapon())
	{
		WeaponEquipped(nullptr);
	}

	mGameInstance()->GetEventManager()->OnEventTriggered.AddUniqueDynamic(this, &ABaseAIController::EventTriggered);
	constexpr int32 range = 13000;


	// Set up sight config for AI perception
	sightConfig->SightRadius = range * 0.9;
	sightConfig->LoseSightRadius = range;
	sightConfig->PeripheralVisionAngleDegrees = 100.0f;

	// This section is important, as without setting at least bDetectNeutrals to true, the AI will never perceive anything
	// Still not tried to set this up correctly at all
	sightConfig->DetectionByAffiliation.bDetectEnemies = true;
	sightConfig->DetectionByAffiliation.bDetectFriendlies = true;
	sightConfig->DetectionByAffiliation.bDetectNeutrals = true;
	PerceptionComponent->ConfigureSense(*sightConfig);

	// Add the AIs character to things that can be perceived by this sight config.
	UAIPerceptionSystem::RegisterPerceptionStimuliSource(this, sightConfig->GetSenseImplementation(), aPawn);

	// Set up our EQS query 
	FindViableCombatLocationRequest = FEnvQueryRequest(FindWeaponLocationQuery, this);

	if (!UNavigationSystemV1::GetCurrent(GetWorld())->IsNavigationBuildInProgress())
	{
		UNavigationSystemV1::GetCurrent(GetWorld())->OnNavigationGenerationFinishedDelegate.AddUniqueDynamic(this, &ABaseAIController::NavDone);
	}
	else
	{
		DetermineNextAction();
	}
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
			if (target == NULL && AICharacter->GetRelationship(otherTeam, mGameInstance()) == ERelationshipType::Enemy)
			{
				// Update our target and set that we can see them, we can assume that, if the actor is a team, it's also damagable
				target = Cast<IDamagable>(Actor);
				canSee = true;
				DetermineNextAction();
			}
			else if (!alliesSeen.Contains(Actor))
			{
				alliesSeen.Add(Cast<ABaseCharacter>(Actor));
			}
		}
	}
}

void ABaseAIController::OnMoveCompleted(FAIRequestID RequestID, const FPathFollowingResult& Result)
{
	if (!Result.IsSuccess() && (Result.Code == EPathFollowingResult::Invalid || Result.Code == EPathFollowingResult::Blocked))
	{
		UE_LOG(LogTemp, Log, TEXT("OnMoveCompleted Failed"));
	}

	mSetTimer(TimerHandle_DetermineAction, &ABaseAIController::DetermineNextAction, 1.0f);
}

// ReSharper disable once CppPassValueParameterByConstReference
void ABaseAIController::WeaponLocationQueryFinished(TSharedPtr<FEnvQueryResult> Result)
{
	// Did we find a new location to move to?
	if (Result->IsSuccessful())
	{
		// Get the first item as a location, this will be the highest scoring location in the array, as it orders items by score
		const FVector loc = Result->GetItemAsLocation(0);

		// Move to the location found
		MoveToLocation(loc, acceptanceRadius);
	}
}

void ABaseAIController::DetermineNextAction()
{
	if (GetBaseCharacter() && GetBaseCharacter()->IsAlive())
	{
		inactiveTimerDuration = 3.0f;

		isInactive = false;

		if (needsAmmo)
		{
			GetAmmo();
		}
		else if (target != NULL && target->IsAlive())
		{
			CalculateCombat();
		}
		else if (target != NULL && target->IsDead())
		{
			target = NULL;
			FindNewTarget();
		}
		else if (target == nullptr)
		{
			Patrol();
		}
	}
	else
	{
		isInactive = false;
	}

	mSetTimer(TimerHandle_Inactive, &ABaseAIController::Inactive, inactiveTimerDuration);
}

void ABaseAIController::WeaponReady()
{
	DetermineNextAction();
}

void ABaseAIController::GetPatrolPath()
{
	// Do we already have a patrol path
	if (currentPath == NULL)
	{
		// Get the first patrol path from the game instance
		currentPath = GameInstance(GetWorld())->paths[0];

		// Check if the path is valid
		if (currentPath != NULL && currentPath->GetSpline()->GetNumberOfSplinePoints() < 1)
		{
			currentPath = NULL;

			// Set path point to -1, as there aren't any valid patrol paths and we don't keep checking every tick
			currentPathPoint = -1;
		}
		else
		{
			const USplineComponent* spline = currentPath->GetSpline();
			currentPathPoint = FMath::RandRange(0, spline->GetNumberOfSplinePoints() - 1);
		}
	}
}

void ABaseAIController::Patrol()
{
	// Check if we aren't moving or we have an invalid Path
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
			currentPathPoint++;
		}
	}
}

void ABaseAIController::KillAI()
{
	GetWorld()->GetTimerManager().ClearAllTimersForObject(this);
	GetBaseCharacter()->GetMovementComponent()->StopMovementImmediately();
	SetActorTickEnabled(false);
	GetBaseCharacter()->GetGame()->GetEventManager()->OnEventTriggered.RemoveAll(this);
	PerceptionComponent->OnTargetPerceptionUpdated.RemoveAll(this);
	UnPossess();
}

bool ABaseAIController::IsInWeaponsRange(float dist)
{
	return mCurrentWeapon() && dist <= mCurrentWeapon()->GetWeaponData().range;
}

void ABaseAIController::AttackWithWeapon()
{
	const FVector targetLocation = IncreaseVectorHeight(target->asActor()->GetActorLocation(), 50);
	UWeapon* weapon = mCurrentWeapon();

	LookAt(targetLocation);

	// if we have a valid weapon, attack the target
	if (weapon != NULL)
	{
		// Check we're in range of the target
		if (IsInWeaponsRange(FVector::Dist(mActorLocation, targetLocation)))
		{
			StopMovement();
			GetBaseCharacter()->StopSprinting();
			FVector targetLoc = IncreaseVectorHeight(GetPredictedLocation(target->asActor()), 70);

			FRotator rotation = GetBaseCharacter()->GetActorRotation();

			if (weapon->GetWeaponData().type == EWeaponType::Projectile)
			{
				FProjectileWeaponData pw = Cast<UProjectileWeapon>(weapon)->GetProjectileWeaponData();
				float gravity = pw.gravity * GetWorld()->GetGravityZ();
				SolveBallisticArc(mActorLocation, targetLoc, pw.bulletVelocity, gravity, rotation);
			}

			AttackWithWeapon(rotation);
		}
		// Otherwise move towards the targets current location
		else
		{
			// We updated the lastKnowLocation here, as we can still see the target and simply need to move forwards to attack again
			lastKnowLocation = target->asActor()->GetActorLocation();
			MoveToCombatLocation();
		}
	}
	else
	{
		// We have no weapon
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
		// Move to the last known location
		LookAt(lastKnowLocation);

		// TODO Implement Searching
		lastKnowLocation = target->asActor()->GetActorLocation();
		MoveToCombatLocation();
	}
}

FVector ABaseAIController::IncreaseVectorHeight(const FVector& location, int32 increase)
{
	return FVector(location.X, location.Y, location.Z + increase);
}

bool ABaseAIController::HasAmmoForWeapon()
{
	if (projectileWeapon && !projectileWeapon->HasAmmo())
	{
		int32 ammoID = projectileWeapon->GetProjectileWeaponData().ammoID;
		int32 ammoQuantity = GetBaseCharacter()->GetInventory()->GetItemAmount(ammoID);
		return ammoQuantity > 0;
	}
	return true;
}

void ABaseAIController::Reload()
{
	if (projectileWeapon)
	{
		projectileWeapon->Reload();
	}
}

FVector ABaseAIController::GetPredictedLocation(AActor* actor)
{
	float lead;

	UWeapon* weapon = GetBaseCharacter()->GetEquippedWeapon();

	if (weapon->GetWeaponData().type == EWeaponType::Projectile)
	{
		UProjectileWeapon* pw = Cast<UProjectileWeapon>(weapon);
		lead = pw->GetProjectileWeaponData().bulletVelocity;
	}
	else // This would account for melee weapons, dunno if  I need lead at all.
	{
		lead = 1;
	}

	float time = FVector::Dist(GetBaseCharacter()->GetActorLocation(), actor->GetActorLocation()) / lead;
	//time = FMath::RandRange(time * 0.9f, time * 1.1f);
	return actor->GetActorLocation() + (actor->GetVelocity() * time);
}

void ABaseAIController::MoveToCombatLocation()
{
	FindViableCombatLocationRequest.Execute(EEnvQueryRunMode::SingleResult, this, &ABaseAIController::WeaponLocationQueryFinished);
	StartSprinting();
}

void ABaseAIController::GetNearbyAmmo()
{
	for (IInteractable* inter : GetBaseCharacter()->GetOverlappingInteractables())
	{
		ABaseCharacter* other = Cast<ABaseCharacter>(inter);

		if (other && other->IsDead())
		{
			for (auto iid : other->GetInventory()->GetItems())
			{
				if (projectileWeapon && iid.itemID == projectileWeapon->GetProjectileWeaponData().ammoID)
				{
					GetBaseCharacter()->GetInventory()->TransferItem(other->GetInventory(), iid, UItemStructs::InvalidInt);
					needsAmmo = false;
				}
			}
		}
	}
}

void ABaseAIController::StartSprinting()
{
	if (!GetBaseCharacter()->IsSprinting())
	{
		GetBaseCharacter()->StartSprinting();
	}
}

bool ABaseAIController::FindAllyWithAmmo()
{
	for (auto ally : alliesSeen)
	{
		if (ally->IsDead() && HasAmmo(ally) && FVector::Dist(mActorLocation, ally->GetActorLocation()) < 10000)
		{
			MoveToLocation(ally->GetActorLocation(), ABaseCharacter::interactionRadius * 0.7);
			StartSprinting();
			return true;
		}
	}
	return false;
}

void ABaseAIController::EquipKnife()
{
	needsAmmo = false;
	bool knifeEquipped = false;
	UBaseGameInstance* game = GetBaseCharacter()->GetGame();
	int32 containerID = GetBaseCharacter()->GetInventory()->GetInstanceContainerData().ID;

	// Get the AIs inventory items
	for (auto iid : game->GetInstancedItemsForContainer(containerID))
	{
		FItemData id = game->GetItemData(iid.itemID);

		// Find the knife, all AI should have one by default
		if (id.name.Equals("Knife"))
		{
			GetBaseCharacter()->GetInventory()->SetEquippedWeapon(UWeaponCreator::CreateWeapon(id.ID, GetBaseCharacter()->GetWorld(), iid.ID));
			knifeEquipped = true;
			DetermineNextAction();
			break;
		}
	}

	if (!knifeEquipped)
	{
		GetBaseCharacter()->GetInventory()->SetEquippedWeapon(nullptr);
	}
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
	DetermineNextAction();
}

void ABaseAIController::AttackWithWeapon(const FRotator& FireDirection)
{
	mCurrentWeapon()->UseWeapon(FireDirection);
}

void ABaseAIController::FindNewTarget()
{
	TArray<AActor*> actorsSensed;

	// Get all the actors we've sensed in our lifetime
	PerceptionComponent.Get()->GetKnownPerceivedActors(sightConfig->GetSenseImplementation(), actorsSensed);

	for (AActor* actor : actorsSensed)
	{
		if (actor->Implements<UTeam>())
		{
			ITeam* team = Cast<ITeam>(actor);
			IDamagable* damagable = Cast<IDamagable>(actor);

			// Check if the actor is both damagable and an enemy
			if (damagable->IsAlive()
				&& AICharacter->GetRelationship(team, mGameInstance()) == ERelationshipType::Enemy)
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

		// If our target is NULL, we can check if we're being attacked and maybe assign a new target
		if (hce->GetChange().source == GetBaseCharacter())
		{
			if (GetBaseCharacter()->IsDead())
			{
				KillAI();
			}
		}
		else if (target == NULL)
		{
			// Only trigger after health changed, the change isn't a heal and the owner of change is our Pawn and if the source is alive still
			// It's possible we take damage from a dead source
			if (!hce->GetChange().heals && hce->GetEventOwner() == GetBaseCharacter() && !hce->GetChange().source->IsDead())
			{
				target = hce->GetChange().source;
				LookAt(hce->GetChange().source->GetActorLocation());
				DetermineNextAction();
			}
		}
		// Check if the change is damage
		// Check if the change is against our target
		// Check if our target is dead
		else if (!hce->GetChange().heals && hce->GetEventOwner() == *target && target->IsDead())
		{
			// clear sight
			canSee = false;

			// Try to find a new target
			FindNewTarget();
		}
	}
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
