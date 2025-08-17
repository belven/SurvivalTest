#include "BaseCharacter.h"
#include "BaseProjectile.h"
#include "BaseGameInstance.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Events/CombatStateEvent.h"
#include "Events/HealthChangeEvent.h"
#include "Events/RPGEventManager.h"
#include "GameFramework/InputSettings.h"
#include "Items/Armour.h"
#include "Perception/AIPerceptionSystem.h"
#include "BasePlayerController.h"
#include "NavigationSystem.h"
#include "Components/SphereComponent.h"
#include "GameFramework/CharacterMovementComponent.h"

float ABaseCharacter::interactionRadius = 500;
const FVector ABaseCharacter::cameraCenter = FVector(50.0f, 0.0f, 80.0f);
const FVector ABaseCharacter::leftLean = FVector(50.0f, -60.0f, 60.0f);
const FVector ABaseCharacter::rightLean = FVector(50.0f, 60.0f, 60.0f);

/**
 *Clears the current stats and sets them back to the max they started with.
 *
 */
void ABaseCharacter::ResetStats()
{
	maxStats.health = 100.0f;
	maxStats.water = 100.0f;
	maxStats.hunger = 100.0f;
	maxStats.rest = 100.0f;
	maxStats.stamina = 100.0f;

	currentMovementState = EMovementState::Base;

	// Seconds
	constexpr float minute = 60.0f;
	constexpr float hour = 60.0f * minute;
	constexpr float dayLengthSeconds = hour;

	// Rate per Second
	maxStats.waterLossRate = maxStats.water / dayLengthSeconds;
	maxStats.hungerLossRate = maxStats.hunger / (dayLengthSeconds * 3.0f);
	maxStats.restLossRate = maxStats.rest / (dayLengthSeconds * 5.0f);
	maxStats.staminaLossRate = maxStats.stamina / 2.0f;
	maxStats.staminaRecoverRate = maxStats.stamina / 5.0f;
	currentStats.CopyStats(maxStats);
}

ABaseCharacter::ABaseCharacter()
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(55.f, 96.0f);

	// Create a CameraComponent	
	baseCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("FirstPersonCamera"));
	baseCameraComponent->SetupAttachment(GetCapsuleComponent());
	baseCameraComponent->SetRelativeLocation(FVector(50.0f, 0.0f, 80.0f)); // Position the camera
	baseCameraComponent->bUsePawnControlRotation = true;

	//navInvoker = CreateDefaultSubobject<UNavigationInvokerComponent>(TEXT("navInvoker"));
	//navInvoker->SetGenerationRadii(4000, 5000);

	interactionSphere = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComp"));
	interactionSphere->InitSphereRadius(interactionRadius);
	interactionSphere->SetupAttachment(GetCapsuleComponent());
	interactionSphere->SetCollisionProfileName("Interaction");
	interactionSphere->OnComponentBeginOverlap.AddDynamic(this, &ABaseCharacter::BeginOverlap);
	interactionSphere->OnComponentEndOverlap.AddDynamic(this, &ABaseCharacter::EndOverlap);

	weaponMeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Weapon Mesh"));
	weaponMeshComp->SetupAttachment(GetMesh(), FName(TEXT("GripPoint")));
	weaponMeshComp->SetRelativeRotation(FRotator(180));
	GetMesh()->SetCustomDepthStencilValue(2);

	ResetStats();
}

void ABaseCharacter::StopSprinting()
{
	isRequestingSprint = false;
}

void ABaseCharacter::StartSprinting()
{
	if (GetCurrentStats().stamina > 0)
	{
		isRequestingSprint = true;
	}
}

void ABaseCharacter::SetMovementSpeed(EMovementState state)
{
	switch (state)
	{
	case EMovementState::Sprinting:
		GetCharacterMovement()->MaxWalkSpeed = baseWalkSpeed * 1.3;
		break;
	case EMovementState::Base:
		GetCharacterMovement()->MaxWalkSpeed = baseWalkSpeed;
		break;
	case EMovementState::Steady:
		GetCharacterMovement()->MaxWalkSpeed = baseWalkSpeed * 0.8;
		break;
	case EMovementState::Slow:
		GetCharacterMovement()->MaxWalkSpeed = baseWalkSpeed * 0.5;
		break;
	default:;
	}
}

void ABaseCharacter::SetMovementState(EMovementState inState)
{
	if (currentMovementState != inState)
	{
		currentMovementState = inState;
		SetMovementSpeed(inState);
	}
}

void ABaseCharacter::Interact(ABasePlayerController* instigator)
{
}

void ABaseCharacter::Highlight(bool activate)
{

}

void ABaseCharacter::BeginPlay()
{
	Super::BeginPlay();

	//navInvoker->RegisterWithNavigationSystem(*UNavigationSystemV1::GetCurrent(GetWorld()));
	game = mGameInstance();
	GetOverlapsOnSpawn();
}

/**
 * This method will load the starting gear for both players and AI.
 *
 * This will need to be updated so that players either load existing gear or a new starter set.
 * AI will also need to have gear sets set in the database, so we can spawn them with specific gear based on the mission
 */
void ABaseCharacter::SetupLoadout(FString loadoutName)
{
	const FLoadoutData ld = game->GetLoadoutData(loadoutName);
	int32 instanceContainerDataID = game->GetNextInstanceContainerDataID();
	FContainerData cd = game->GetContainerDataName("Character Inventory");

	FInstanceContainerData icd;
	icd.ID = instanceContainerDataID;
	icd.containerID = cd.ID;
	icd.type = EContainerType::Player;
	icd.name = cd.name;
	game->GetInstancedContainers().Add(icd.ID, icd);
	inventory = UInventory::CreateInventory(cd, icd, game, this);

	currentStats.health = ld.health;
	maxStats.health = ld.health;
	baseWalkSpeed = ld.moveSpeed;

	inventory->SetupLoadout(ld);
}

UStaticMesh* ABaseCharacter::GetItemMesh(FItemData data)
{
	if (data.mesh.Equals(""))
		return nullptr;

	return LoadObject<UStaticMesh>(this, *data.mesh);
}


/**
 * This is used to change the characters health, taking into account the type of change and possibly armour for damage reduction
 *
 * @param health_change The data related to the health change, the amount of change, the source of the damage etc.
 */
void ABaseCharacter::ChangeHealth(FHealthChange& health_change)
{
	mEventTriggered(game, mCreateHealthChangeEvent(this, health_change, true));

	if (!inCombat && !health_change.heals)
	{
		FCombatStateChange csc;
		csc.source = this;
		csc.oldState = false;
		csc.newState = true;
		inCombat = true;
		mEventTriggered(game, mCreateCombatStateEvent(this, csc));
	}

	if (health_change.heals)
	{
		currentStats.health += health_change.changeAmount;
	}
	else
	{
		health_change.changeAmount = GetDamageAfterResistance(health_change.changeAmount);
		currentStats.health -= health_change.changeAmount;
	}

	FMath::Clamp(health_change.heals, 0, maxStats.health);

	if (IsDead())
	{
		if (FVector::Dist(GetActorLocation(), health_change.source->GetActorLocation()) <= interactionRadius)
			health_change.source->AddInteractable(this);
		//UAIPerceptionSystem::GetCurrent(this)->UnregisterSource(*this, nullptr);
	}

	health_change.source->EnemyHit(this);

	mEventTriggered(game, mCreateHealthChangeEvent(this, health_change, false));
}

/**
 *This method calculates damage taken after damage resistance applied.
 *I.e. 20 resistance could reduce damage by 15% etc.
 *
 *@param damage The damage dealt to the character
 *
 *@return the damage taken after resistance is applied.
 */
float ABaseCharacter::GetDamageAfterResistance(float damage)
{
	const float resistance = 100.0f + GetDamageResistance();
	const float damageReduction = 100.0f / resistance;
	return damage * damageReduction;
}

/**
 *Totals up all the damage resistances for currently equipped armour
 *
 *@return The total amount of damage resistance
 *
 */
int32 ABaseCharacter::GetDamageResistance()
{
	int32 total = 0;

	for (auto a : GetInventory()->GetEquippedArmour())
	{
		total += a.Value->GetData().resistance;
	}

	return total;
}

/**
 * This method gets all the UInteractable objects around the character on spawn.
 * This is so we can highlight objects the player is near when they spawn in the game
 */
void ABaseCharacter::GetOverlapsOnSpawn()
{
	if (interactionSphere)
	{
		TArray<AActor*> actors;
		interactionSphere->GetOverlappingActors(actors);

		for (AActor* actor : actors)
		{
			if (actor->Implements<UInteractable>())
			{
				AddInteractable(Cast<IInteractable>(actor));
			}
		}
	}
}

/**
 * Adds a given IInteractable to our interactable list
 *
 * @param inter The interactable to add
 *
 */
void ABaseCharacter::AddInteractable(IInteractable* inter)
{
	bool shouldAdd = false;

	ABaseCharacter* other = Cast<ABaseCharacter>(inter);

	// If the other is not a Character, then it's a regular container, so add it regardless.
	// TODO - This is where we should check for loot permissions in teams
	if (other == NULL)
	{
		shouldAdd = true;
	}
	// Otherwise, only show inventories of dead characters nearby
	else if (other != this && other->IsDead())
	{
		shouldAdd = true;
	}

	if (shouldAdd)
	{
		inter->Highlight(true);
		overlappingInteractables.AddUnique(inter);

		if (GetInventory()) {
			GetInventory()->OnContainersUpdated.Broadcast();
		}
	}
}

/**
 * Removes a given IInteractable from our interactable list
 *
 * @param inter The interactable to remove
 *
 */
void ABaseCharacter::RemoveInteractable(IInteractable* inter)
{
	inter->Highlight(false);
	overlappingInteractables.Remove(inter);
	GetInventory()->OnContainersUpdated.Broadcast();
}

void ABaseCharacter::Consume(EConsumableType type, int32 value)
{
	switch (type)
	{
	case EConsumableType::Food:
		currentStats.hunger += value;
		break;
	case EConsumableType::Drink:
		currentStats.water += value;
		break;
	case EConsumableType::Medical:
		currentStats.health += value;
		break;
	default:;
	}
}

/**
 * This is typically triggered by the BeginOverlap of the interactionSphere.
 * We use this to automate highlighting of in world actors, so the player can see them easier
 */
void ABaseCharacter::BeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor->Implements<UInteractable>())
	{
		AddInteractable(Cast<IInteractable>(OtherActor));
	}
}

/**
 * This is typically triggered by the BeginOverlap of the interactionSphere.
 * We use this to automate highlighting of in world actors, so the player can see them easier
 */
void ABaseCharacter::EndOverlap(UPrimitiveComponent* overlappedComponent, AActor* otherActor, UPrimitiveComponent* otherComp, int32 otherBodyIndex)
{
	if (otherActor->Implements<UInteractable>())
	{
		RemoveInteractable(Cast<IInteractable>(otherActor));
	}
}

/**
 * This method is used to change a given stat over time, like food, water, rest etc.
 *
 * It's the same calculation for all stats currently, and therefore can be re-used for each, given the parameters
 *
 * @param stat The stat to change, typically from currentStats.Water etc.
 * @param drainRate The amount of the stat that is lost per second
 * @param healthDamage The amount of damage to do, if the stat is less than 0
 * @param deltaSeconds The amount of time that has passed, often 0.02 something, about the same as the FPS
 */
void ABaseCharacter::DrainStat(float& stat, float drainRate, float healthDamage, float deltaSeconds)
{
	if (stat > 0)
	{
		stat -= drainRate * deltaSeconds;
	}
	else
	{
		stat = 0;
		currentStats.health -= healthDamage * deltaSeconds;

		if (IsDead())
		{
			SetActorHiddenInGame(true);
			SetActorEnableCollision(false);
			UAIPerceptionSystem::GetCurrent(this)->UnregisterSource(*this, nullptr);
		}
	}
}

/**
 * This occurs when the character is taken over by a controller.
 * We use this to determine if the character is a player or not.
 * If they are a player, we can setup the listeners for the Overlap of the interactionSphere.
 * Otherwise, we remove it entirely
 *
 */
void ABaseCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);
	game = mGameInstance();

	if (NewController->IsA(ABasePlayerController::StaticClass()))
	{
		GetOverlapsOnSpawn();

		SetupLoadout("Player");
	}
}

void ABaseCharacter::CalculateSprint(float DeltaSeconds)
{
	// Check if we're moving more than a low speed
	if (!GetVelocity().IsNearlyZero())
	{
		timeMoved += DeltaSeconds;
	}
	// If we're not moving, reset timeMoved
	else
	{
		timeMoved = 0;
	}

	// If we're sprinting and we've been moving for more than a third of a second, start sprinting speed
	if (isRequestingSprint && timeMoved > 0.33)
	{
		// Consume Stamina
		DrainStat(currentStats.stamina, currentStats.staminaLossRate, 0.0f, DeltaSeconds);

		// Stop sprinting if we have no stamina
		if (currentStats.stamina <= 0)
		{
			StopSprinting();
			SetMovementState(EMovementState::Base);
		}
		else
		{
			SetMovementState(EMovementState::Sprinting);
		}
	}
	else if (currentStats.stamina <= maxStats.stamina)
	{
		currentStats.stamina += currentStats.staminaRecoverRate * DeltaSeconds;

		SetMovementState(EMovementState::Base);
	}

	FMath::Clamp(currentStats.stamina, 0, maxStats.stamina);
}

void ABaseCharacter::EnemyHit(ABaseCharacter* inActor)
{
	OnEnemyHit.Broadcast(inActor);
}

/**
 * Almost entirely used for passive stat draining like food and water.
 */
void ABaseCharacter::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (currentStats.health > 0)
	{
		DrainStat(currentStats.water, currentStats.waterLossRate, 0.5f, DeltaSeconds);
		DrainStat(currentStats.rest, currentStats.restLossRate, 0.25f, DeltaSeconds);
		DrainStat(currentStats.hunger, currentStats.hungerLossRate, 0.1f, DeltaSeconds);
		CalculateSprint(DeltaSeconds);
	}
}
