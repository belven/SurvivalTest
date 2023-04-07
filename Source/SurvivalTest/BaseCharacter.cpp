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
#include "Items/Weapon.h"
#include "BasePlayerController.h"
#include "Components/SphereComponent.h"
#include "Items/ItemContainer.h"

float ABaseCharacter::interactionRadius = 500;

void ABaseCharacter::ResetStats()
{
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
	interactionSphere->InitSphereRadius(interactionRadius);
	interactionSphere->SetupAttachment(GetCapsuleComponent());
	//interactionSphere->SetCollisionResponseToChannel(ECC_EngineTraceChannel1, ECR_Ignore);
	interactionSphere->SetCollisionProfileName("Interaction");
	ResetStats();
}

void ABaseCharacter::BeginPlay()
{
	Super::BeginPlay();
	SetupLoadout();
}

void ABaseCharacter::SetupLoadout()
{
	const FLoadoutData ld = GetBaseGameInstance()->GetLoadoutData(1);

	SetEquippedWeapon(UWeaponCreator::CreateWeapon(ld.weaponID, GetWorld()));

	int32 instanceContainerDataID = GetBaseGameInstance()->GetNextInstanceContainerDataID();

	FContainerData cd = GetBaseGameInstance()->GetContainerDataByID(3);

	FInstanceContainerData icd;
	icd.ID = instanceContainerDataID;
	icd.containerID = cd.ID;
	icd.type = EContainerType::Player;
	icd.name = cd.name;
	GetBaseGameInstance()->GetInstancedContainers().Add(icd.ID, icd);
	inventory = UItemContainer::CreateItemContainer(cd, icd, GetBaseGameInstance());

	CreateNewItemForInventory(ld.headArmourID, EGearType::Head);
	CreateNewItemForInventory(ld.chestArmourID, EGearType::Chest);
	CreateNewItemForInventory(ld.legsArmourID, EGearType::Legs);
}

int32 ABaseCharacter::GetSlotForGear(EGearType type)
{
	switch (type)
	{
	case EGearType::Head: return 1;
	case EGearType::Chest: return 2;
	case EGearType::Legs: return 3;
	case EGearType::Primary_Weapon: return 4;
	case EGearType::Secondary_Weapon: return 5;
	case EGearType::Sidearm: return 6;
	case EGearType::Bag: return 7;
	case EGearType::Vest: return 8;
	default: return 9;
	}
}

void ABaseCharacter::CreateNewItemForInventory(int32 armourID, EGearType type)
{
	TArray<int32> ids;
	FInstanceItemData id;
	id.itemID = armourID;
	id.amount = 1;
	id.slot = GetSlotForGear(type);
	inventory->AddItem(id, ids);

	EquipArmour(UArmourCreator::CreateArmour(armourID, GetWorld(), ids[0]));
}

void ABaseCharacter::SetEquippedWeapon(UWeapon* weapon)
{
	equippedWeapon = weapon;
	equippedWeapon->SetOwner(this);
}

void ABaseCharacter::EquipArmour(UArmour* armour)
{
	equippedArmour.FindOrAdd(armour->GetData().slot, armour);
}

UBaseGameInstance* ABaseCharacter::GetBaseGameInstance()
{
	if (gameInstance == nullptr)
	{
		gameInstance = GameInstance(GetWorld());
	}
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
		SetActorHiddenInGame(true);
		SetActorEnableCollision(false);
		UAIPerceptionSystem::GetCurrent(this)->UnregisterSource(*this, nullptr);
	}
}

float ABaseCharacter::GetDamageAfterResistance(float damage)
{
	const float resistance = 100.0f + GetDamageResistance();
	const float damageReduction = 100.0f / resistance;
	return damage * damageReduction;
}

int32 ABaseCharacter::GetDamageResistance()
{
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

	for (AActor* actor : actors)
	{
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

void ABaseCharacter::BeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor->Implements<UInteractable>())
	{
		AddInteractable(Cast<IInteractable>(OtherActor));
	}
}

void ABaseCharacter::EndOverlap(UPrimitiveComponent* overlappedComponent, AActor* otherActor, UPrimitiveComponent* otherComp, int32 otherBodyIndex)
{
	if (otherActor->Implements<UInteractable>())
	{
		RemoveInteractable(Cast<IInteractable>(otherActor));
	}
}

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

void ABaseCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	if (NewController->IsA(ABasePlayerController::StaticClass()))
	{
		interactionSphere->OnComponentBeginOverlap.AddDynamic(this, &ABaseCharacter::BeginOverlap);
		interactionSphere->OnComponentEndOverlap.AddDynamic(this, &ABaseCharacter::EndOverlap);
		GetOverlapsOnSpawn();
	}
	else
	{
		interactionSphere->DestroyComponent();
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
