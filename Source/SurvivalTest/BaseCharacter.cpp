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
// ReSharper disable once CppUnusedIncludeDirective
#include "BaseGameInstance.h"
#include "Items/Weapon.h"
#include "BasePlayerController.h"
#include "NavigationInvokerComponent.h"
#include "NavigationSystem.h"
#include "Components/SphereComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Items/ItemContainer.h"
#include "Missions/GridSectionData.h"
#include "Missions/MainGrid.h"

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
	default: ;
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
	inventory = UItemContainer::CreateItemContainer(cd, icd, game);

	TArray<EGearType> gearTypes;
	gearTypes.AddUnique(EGearType::Head);
	gearTypes.AddUnique(EGearType::Legs);
	gearTypes.AddUnique(EGearType::Chest);
	gearTypes.AddUnique(EGearType::Weapon);
	gearTypes.AddUnique(EGearType::Bag);
	gearTypes.AddUnique(EGearType::Sidearm);
	gearTypes.AddUnique(EGearType::Vest);

	for (EGearType type : gearTypes)
	{
		for (int32 slot : GetSlotForGear(type))
		{
			if (slot != UItemStructs::InvalidInt)
				inventory->AddValidSlot(type, slot);
		}
	}

	currentStats.health = ld.health;
	maxStats.health = ld.health;
	CreateNewItemForInventory(GetGame()->GetItemDataByName("Knife").ID);
	CreateNewItemForInventory(ld.weaponID);
	CreateNewItemForInventory(ld.headArmourID);
	CreateNewItemForInventory(ld.chestArmourID);
	CreateNewItemForInventory(ld.vestArmourID);
	CreateNewItemForInventory(ld.legsArmourID);

	baseWalkSpeed = ld.moveSpeed;

	inventory->OnItemAdded.AddUniqueDynamic(this, &ABaseCharacter::ItemAdded);
	inventory->OnItemUpdated.AddUniqueDynamic(this, &ABaseCharacter::ItemUpdated);
	inventory->OnItemRemoved.AddUniqueDynamic(this, &ABaseCharacter::ItemRemoved);
}

/**
 *Helper method defining the slots that are valid for specific types of gear
 *
 *This needs to be change to use validSlots in UItemContainer
 *
 *@param type The type of gear to get the slot for
 */
TArray<int32> ABaseCharacter::GetSlotForGear(EGearType type)
{
	TArray<int32> validSlots;
	switch (type)
	{
	case EGearType::Head: validSlots.Add(0);
		break;
	case EGearType::Chest: validSlots.Add(1);
		break;
	case EGearType::Vest: validSlots.Add(2);
		break;
	case EGearType::Legs: validSlots.Add(3);
		break;
	case EGearType::Weapon:
		validSlots.Add(4);
		validSlots.Add(5);
		break;
	case EGearType::Sidearm: validSlots.Add(6);
		break;
	case EGearType::Bag: validSlots.Add(7);
		break;
	default:
		break;
	}
	return validSlots;
}

int32 ABaseCharacter::GetPrimaryWeaponSlot()
{
	return GetSlotForGear(EGearType::Weapon)[0];
}

int32 ABaseCharacter::GetSecondaryWeaponSlot()
{
	return GetSlotForGear(EGearType::Weapon)[1];
}

int32 ABaseCharacter::GetSidearmWeaponSlot()
{
	return GetSlotForGear(EGearType::Sidearm)[0];
}

/**
 * Currently used as a helper method, to add gear spawned from the loadouts.
 *
 * This needs to be changed so that gear equipped on a player, is added to an additional data table,
 * that stores the players inventory, so on reload, they get the same gear back
 *
 * @param itemID the ID of the armour to add
 */
void ABaseCharacter::CreateNewItemForInventory(int32 itemID)
{
	if (itemID != UItemStructs::InvalidInt)
	{
		FItemData id = game->GetItemData(itemID);
		TArray<int32> ids;
		FInstanceItemData iid(itemID, 1);

		if (inventory->AddItem(iid, ids).amount == 0)
		{
			int32 weaponInstanceItemID = ids.IsEmpty() ? UItemStructs::InvalidInt : ids[0];

			if (id.type == EItemType::Armour)
			{
				EquipArmour(UArmourCreator::CreateArmour(itemID, GetWorld(), ids[0]));
			}
			else if (id.type == EItemType::Weapon)
			{
				FWeaponData wd = GetGame()->GetWeaponData(id.ID);

				if (wd.type == EWeaponType::Projectile)
				{
					FRangedWeaponData rwd = GetGame()->GetRangedWeaponData(wd.ID);
					FProjectileWeaponData pwd = GetGame()->GetProjectileWeaponData(rwd.ID);
					FItemData ammoData = GetGame()->GetItemData(pwd.ammoID);

					for (int i = 0; i < 1; ++i)
					{
						iid.amount = ammoData.maxStack * 3;
						iid.itemID = pwd.ammoID;

						ids.Empty();
						GetInventory()->AddItem(iid, ids);
					}
				}

				SetEquippedWeapon(UWeaponCreator::CreateWeapon(itemID, GetWorld(), weaponInstanceItemID));
			}
		}
	}
}

/**
 * Equips a weapon the character, setting the owner as well
 *
 *@param weapon The weapon to equip
 */
void ABaseCharacter::SetEquippedWeapon(UWeapon* weapon)
{
	UWeapon* oldWeapon = equippedWeapon;
	equippedWeapon = weapon;

	if (equippedWeapon)
	{
		equippedWeapon->SetOwner(this);
	}

	OnWeaponEquipped.Broadcast(oldWeapon);
}

UStaticMesh* ABaseCharacter::GetItemMesh(FItemData data)
{
	if (data.mesh.Equals(""))
		return nullptr;

	return LoadObject<UStaticMesh>(this, *data.mesh);
}

/**
 *Equips the given armour to the character. Adds it the slot that is viable for the armour
 *
 *@param armour The armour to equip
 *
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
	ABaseCharacter* other = Cast<ABaseCharacter>(inter);
	if (!other || (other != this && other->IsDead()))
	{
		inter->Highlight(true);
		overlappingInteractables.AddUnique(inter);
		OnContainersUpdated.Broadcast();
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
	OnContainersUpdated.Broadcast();
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
	default: ;
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

void ABaseCharacter::ItemUpdated(FInstanceItemData inItem, FInstanceItemData oldItem)
{
	FItemData id = game->GetItemData(inItem.itemID);

	if (id.type == EItemType::Weapon)
	{
		bool movedSlot = inItem.slot != oldItem.slot;
		bool isWeaponSlot = GetSlotForGear(EGearType::Weapon).Contains(inItem.slot) || GetSlotForGear(EGearType::Sidearm).Contains(inItem.slot);

		if (GetEquippedWeapon())
		{
			// Have we updated our equipped weapon?
			if (GetEquippedWeapon()->GetInstanceWeaponData().instanceItemID == inItem.ID)
			{
				// Has the weapon moved out of an equipped slot?
				if (movedSlot && !isWeaponSlot)
				{
					SetEquippedWeapon(nullptr);
				}
			}
			// Is the new item in a valid weapon slot
			// If so equip the new weapon
			else if(isWeaponSlot)
			{
				SetEquippedWeapon(UWeaponCreator::CreateWeapon(inItem.itemID, GetWorld(), inItem.ID));				
			}
		}
		else if (isWeaponSlot)
		{
			SetEquippedWeapon(UWeaponCreator::CreateWeapon(inItem.itemID, GetWorld(), inItem.ID));
		}
	}
	else if (id.type == EItemType::Armour)
	{
		UArmour* armourFound = nullptr;

		for (auto& a : equippedArmour)
		{
			if (a.Value->GetInstanceArmourData().instancedItemDataID == inItem.ID
				// Has the armour moved out of an equipped slot?
				&& !GetSlotForGear(a.Value->GetData().slot).Contains(inItem.slot))
			{
				armourFound = a.Value;
				break;
			}
		}

		if (armourFound)
		{
			equippedArmour.Remove(armourFound->GetData().slot);
		}
	}
	OnContainersUpdated.Broadcast();
}

/**
 * An event listener for the players inventory that responds to items being added
 *
 *@param inItem The item added to the inventory
 *
 */
void ABaseCharacter::ItemAdded(FInstanceItemData inItem)
{
	FItemData id = game->GetItemData(inItem.itemID);

	// If we added armour, then update the Equipped armour
	if (id.type == EItemType::Armour)
	{
		FInstanceArmourData iad = game->GetInstanceArmourDataByInstanceItemID(inItem.ID);
		EquipArmour(UArmour::CreateArmour(inItem.itemID, game, inItem.ID));
	}
	// If we added a weapon and don't have one equipped, then equip it
	else if (id.type == EItemType::Weapon)
	{
		FWeaponData wd = game->GetWeaponData(id.ID);
		if (!GetEquippedWeapon() && GetSlotForGear(wd.gearType).Contains(inItem.slot))
		{
			SetEquippedWeapon(UWeaponCreator::CreateWeapon(inItem.itemID, GetWorld(), inItem.ID));
		}
	}
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
	FItemData id = game->GetItemData(inItem.itemID);

	if (id.type == EItemType::Weapon)
	{
		if (GetEquippedWeapon() && GetEquippedWeapon()->GetInstanceWeaponData().instanceItemID == inItem.ID)
		{
			SetEquippedWeapon(nullptr);
		}
	}
	else if (id.type == EItemType::Armour)
	{
		UArmour* armourFound = nullptr;

		for (auto& a : equippedArmour)
		{
			if (a.Value->GetInstanceArmourData().instancedItemDataID == inItem.ID)
			{
				armourFound = a.Value;
				break;
			}
		}

		if (armourFound)
		{
			equippedArmour.Remove(armourFound->GetData().slot);
		}
	}
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

	//if (game->grid)
	//{
	//	AGridSection* gs = game->grid->GetGridSection(GetActorLocation());
	//	if (gs)
	//		gs->HighlightSection(DeltaSeconds);
	//}

	if (currentStats.health > 0)
	{
		DrainStat(currentStats.water, currentStats.waterLossRate, 0.5f, DeltaSeconds);
		DrainStat(currentStats.rest, currentStats.restLossRate, 0.25f, DeltaSeconds);
		DrainStat(currentStats.hunger, currentStats.hungerLossRate, 0.1f, DeltaSeconds);
		CalculateSprint(DeltaSeconds);
	}
}
