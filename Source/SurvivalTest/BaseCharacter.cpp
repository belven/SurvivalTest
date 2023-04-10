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
	interactionSphere->SetCollisionProfileName("Interaction");
	interactionSphere->OnComponentBeginOverlap.AddDynamic(this, &ABaseCharacter::BeginOverlap);
	interactionSphere->OnComponentEndOverlap.AddDynamic(this, &ABaseCharacter::EndOverlap);
	
	ResetStats();
}

void ABaseCharacter::BeginPlay()
{
	Super::BeginPlay();
	SetupLoadout();
}

/**
 * This method will load the starting gear for both players and AI.
 *
 * This will need to be updated so that players either load existing gear or a new starter set.
 * AI will also need to have gear sets set in the database, so we can spawn them with specific gear based on the mission
 */
void ABaseCharacter::SetupLoadout()
{
	const FLoadoutData ld = mGameInstance()->GetLoadoutData(1);

	SetEquippedWeapon(UWeaponCreator::CreateWeapon(ld.weaponID, GetWorld()));

	int32 instanceContainerDataID = mGameInstance()->GetNextInstanceContainerDataID();
	
	FContainerData cd = mGameInstance()->GetContainerDataName("Character Inventory");

	FInstanceContainerData icd;
	icd.ID = instanceContainerDataID;
	icd.containerID = cd.ID;
	icd.type = EContainerType::Player;
	icd.name = cd.name;
	mGameInstance()->GetInstancedContainers().Add(icd.ID, icd);
	inventory = UItemContainer::CreateItemContainer(cd, icd, mGameInstance());

	CreateNewItemForInventory(ld.headArmourID, EGearType::Head);
	CreateNewItemForInventory(ld.chestArmourID, EGearType::Chest);
	CreateNewItemForInventory(ld.legsArmourID, EGearType::Legs);
}


/**
 *Helper method defining the slots that are valid for specific types of gear
 *
 *This needs to be change to use validSlots in UItemContainer
 *
 *@param type The type of gear to get the slot for
 */
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


/**
 * Currently used as a helper method, to add gear spawned from the loadouts.
 *
 * This needs to be changed so that gear equipped on a player, is added to an additional data table,
 * that stores the players inventory, so on reload, they get the same gear back
 *
 *TODO change this to work with the new inventory system
 *
 * @param armourID the ID of the armour to add
 * @param type The location of the gear to add
 */
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

/**
 * Equips a weapon the character, setting the owner as well
 *
 *@param weapon The weapon to equip
 */
void ABaseCharacter::SetEquippedWeapon(UWeapon* weapon)
{
	equippedWeapon = weapon;
	equippedWeapon->SetOwner(this);
}

/**
 *Equips the given armour to the character. Adds it the slot that is viable for the armour
 *
 *@param armour The armour to equip
 *
 * TODO make this work with the new inventory system. Maybe even remove the concept entirely from character and make a sub class of inventory?
 */
void ABaseCharacter::EquipArmour(UArmour* armour)
{
	equippedArmour.FindOrAdd(armour->GetData().slot, armour);
}

/**
 * This is used to change the characters health, taking into account the type of change and possibly armour for damage reduction
 *
 * @param health_change The data related to the health change, the amount of change, the source of the damage etc.
 */
void ABaseCharacter::ChangeHealth(FHealthChange& health_change)
{
	mEventTriggered(mGameInstance(), mCreateHealthChangeEvent(this, health_change, true));

	if (!inCombat && !health_change.heals)
	{
		FCombatStateChange csc;
		csc.source = this;
		csc.oldState = false;
		csc.newState = true;
		inCombat = true;
		mEventTriggered(mGameInstance(), mCreateCombatStateEvent(this, csc));
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

	TArray<UArmour*> armour;
	equippedArmour.GenerateValueArray(armour);

	for (const UArmour* a : armour)
	{
		total += a->GetData().resistance;
	}

	return total;
}

/**
 * This method gets all the UInteractable objects around the character on spawn.
 * This is so we can highlight objects the player is near when they spawn in the game
 */
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

/**
 * Adds a given IInteractable to our interactable list
 *
 * @param inter The interactable to add
 *
 */
void ABaseCharacter::AddInteractable(IInteractable* inter)
{
	inter->Highlight(true);
	overlappingInteractables.AddUnique(inter);
	OnContainersUpdated.Broadcast();
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
	OnContainersUpdated.Broadcast();
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
 * An event listener for the players inventory that responds to items being added
 *
 *@param inItem The item added to the inventory
 *
 */
void ABaseCharacter::ItemAdded(FInstanceItemData inItem)
{
	OnContainersUpdated.Broadcast();
}

/**
 * An event listener for the players inventory that responds to items being removed
 *
 *@param inItem The item removed from the inventory
 *
 */
void ABaseCharacter::ItemRemoved(FInstanceItemData inItem)
{
	OnContainersUpdated.Broadcast();
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
 * TODO find a better way to spawn the interactionSphere and set it up
 */
void ABaseCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	if (NewController->IsA(ABasePlayerController::StaticClass()))
	{
		GetOverlapsOnSpawn();
	}
	else
	{
		interactionSphere->DestroyComponent();
	}
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
	}
}
