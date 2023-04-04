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
#include "Items/ArmourCreator.h"
#include "Items/WeaponCreator.h"
#include "Perception/AIPerceptionSystem.h"
#include "BaseGameInstance.h"
#include "Components/SphereComponent.h"

ABaseCharacter::ABaseCharacter()
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(55.f, 96.0f);

	// Create a CameraComponent	
	FirstPersonCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("FirstPersonCamera"));
	FirstPersonCameraComponent->SetupAttachment(GetCapsuleComponent());
	FirstPersonCameraComponent->SetRelativeLocation(FVector(-39.56f, 1.75f, 150.f)); // Position the camera
	FirstPersonCameraComponent->bUsePawnControlRotation = true;

	interactionSphere = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComp"));
	interactionSphere->InitSphereRadius(500.0f);
	interactionSphere->SetupAttachment(GetCapsuleComponent());
	interactionSphere->OnComponentBeginOverlap.AddDynamic(this, &ABaseCharacter::BeginOverlap);
	interactionSphere->OnComponentEndOverlap.AddDynamic(this, &ABaseCharacter::EndOverlap);
	interactionSphere->SetWalkableSlopeOverride(FWalkableSlopeOverride(WalkableSlope_Unwalkable, 0.f));
	interactionSphere->CanCharacterStepUpOn = ECB_No;

	//// Create a mesh component that will be used when being viewed from a '1st person' view (when controlling this pawn)
	//Mesh1P = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("CharacterMesh1P"));
	//Mesh1P->SetupAttachment(FirstPersonCameraComponent);
	//Mesh1P->SetRelativeRotation(FRotator(1.9f, -19.19f, 5.2f));
	//Mesh1P->SetRelativeLocation(FVector(-0.5f, -4.4f, -155.7f));

	maxStats.health = 100.0f;
	maxStats.water = 100.0f;
	maxStats.hunger = 100.0f;
	maxStats.rest = 100.0f;

	// Seconds
	constexpr float minute = 60.0f;
	constexpr float hour = 60.0f * minute;
	constexpr float dayLengthSeconds = minute * 0.3;

	// Rate per Second
	maxStats.waterLossRate = maxStats.water / dayLengthSeconds;
	maxStats.hungerLossRate = maxStats.hunger / (dayLengthSeconds * 3.0f);
	maxStats.restLossRate = maxStats.rest / (dayLengthSeconds * 5.0f);

	currentStats.CopyStats(maxStats);
}

void ABaseCharacter::BeginPlay()
{
	Super::BeginPlay();
	SetupLoadout();
	GetOverlapsOnSpawn();
}

void ABaseCharacter::SetupLoadout()
{
	const FLoadoutData ld = GetBaseGameInstance()->GetLoadoutData(1);

	SetEquippedWeapon(UWeaponCreator::CreateWeapon(ld.weaponID, GetWorld()));

	EquipArmour(UArmourCreator::CreateArmour(ld.headArmourID, GetWorld(), UItemStructs::InvalidInt));
	EquipArmour(UArmourCreator::CreateArmour(ld.chestArmourID, GetWorld(), UItemStructs::InvalidInt));
	EquipArmour(UArmourCreator::CreateArmour(ld.legsArmourID, GetWorld(), UItemStructs::InvalidInt));
}

void ABaseCharacter::EquipArmour(UArmour* armour)
{
	equippedArmour.FindOrAdd(armour->GetData().slot, armour);
}

UBaseGameInstance* ABaseCharacter::GetBaseGameInstance()
{
	if (gameInstance == NULL)
		gameInstance = GameInstance(GetWorld());
	return gameInstance;
}

void ABaseCharacter::ChangeHealth(FHealthChange& health_change)
{
	mEventTriggered(GetBaseGameInstance(), mCreateHealthChangeEvent(this, health_change, true));

	if (!inCombat && !health_change.heals)
	{
		FCombatStateChange csc;
		csc.source = this;
		csc.oldState = false;
		csc.newState = true;
		inCombat = true;
		mEventTriggered(GetBaseGameInstance(), mCreateCombatStateEvent(this, csc));
	}

	if (health_change.heals) {
		currentStats.health += health_change.changeAmount;
	}
	else {
		health_change.changeAmount = GetDamageAfterResistance(health_change.changeAmount);
		currentStats.health -= health_change.changeAmount;
	}

	FMath::Clamp(health_change.heals, 0, maxStats.health);

	if (IsDead())
	{
		SetActorHiddenInGame(true);
		SetActorEnableCollision(false);
		UAIPerceptionSystem::GetCurrent(this)->UnregisterSource(*this, NULL);
	}
}

float  ABaseCharacter::GetDamageAfterResistance(float damage) {
	const float resistance = 100.0f + GetDamageResistance();
	const float damageReduction = 100.0f / resistance;
	return damage * damageReduction;
}

int32  ABaseCharacter::GetDamageResistance() {
	int32 total = 0;

	TArray<UArmour*> armour;
	equippedArmour.GenerateValueArray(armour);

	for (const UArmour* a : armour)
	{
		total += a->GetData().resistance;
	}

	return total;
}

void ABaseCharacter::GetOverlapsOnSpawn()
{
	TArray<AActor*> actors;
	interactionSphere->GetOverlappingActors(actors);

	for (AActor* actor : actors) {
		if (actor->Implements<UInteractable>())
		{
			AddInteractable(Cast<IInteractable>(actor));
		}
	}
}

void ABaseCharacter::AddInteractable(IInteractable* inter)
{
	inter->Highlight(true);
	overlappingInteractables.AddUnique(inter);
	OnContainersUpdated.Broadcast();
}

void ABaseCharacter::RemoveInteractable(IInteractable* inter)
{
	inter->Highlight(false);
	overlappingInteractables.Remove(inter);
	OnContainersUpdated.Broadcast();
}

void ABaseCharacter::BeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if(OtherActor->Implements<UInteractable>())
	{
		AddInteractable(Cast<IInteractable>(OtherActor));
	}
}

void ABaseCharacter::EndOverlap(UPrimitiveComponent* overlappedComponent, AActor* otherActor,
	UPrimitiveComponent* otherComp, int32 otherBodyIndex)
{
	if (otherActor->Implements<UInteractable>())
	{
		RemoveInteractable(Cast<IInteractable>(otherActor));
	}
}

void ABaseCharacter::DrainStat(float& stat, float drainRate, float healthDamage, float deltaSeconds)
{
	if (stat > 0) {
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
			UAIPerceptionSystem::GetCurrent(this)->UnregisterSource(*this, NULL);
		}
	}
}

void ABaseCharacter::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (currentStats.health > 0)
	{
		DrainStat(currentStats.water, currentStats.waterLossRate, 0.5f, DeltaSeconds);
		DrainStat(currentStats.rest, currentStats.restLossRate, 0.25f, DeltaSeconds);
		DrainStat(currentStats.hunger, currentStats.hungerLossRate, 0.1f, DeltaSeconds);	
	}
}