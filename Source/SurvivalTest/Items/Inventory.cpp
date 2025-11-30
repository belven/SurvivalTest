#include "Inventory.h"

#include "Armour.h"
#include "ArmourCreator.h"
#include "Weapon.h"
#include "WeaponCreator.h"
#include "SurvivalTest/BaseCharacter.h"

UInventory* UInventory::CreateInventory(const FContainerData& inContainerData, const FInstanceContainerData& inInstanceContainerData, UBaseGameInstance* inGame, ABaseCharacter* inCharacterOwner)
{
	UInventory* ic = NewObject<UInventory>();
	ic->SetContainerData(inContainerData);
	ic->SetInstanceContainerData(inInstanceContainerData);
	ic->baseGameInstance = inGame;
	ic->characterOwner = inCharacterOwner;
	return ic;
}

/**
 * Currently used as a helper method, to add gear spawned from the loadouts.
 *
 * This needs to be changed so that gear equipped on a player, is added to an additional data table,
 * that stores the players inventory, so on reload, they get the same gear back
 *
 * @param itemID the ID of the armour to add
 */
void UInventory::CreateNewItemForInventory(int32 itemID)
{
	if (itemID != UItemStructs::InvalidInt)
	{
		FItemData id = baseGameInstance->GetItemData(itemID);
		TArray<FInstanceItemData> ids;
		FInstanceItemData iid(itemID, 1);
		FInstanceItemData& newItem = AddItem(iid, ids);

		if (newItem.amount == 0 && !ids.IsEmpty())
		{
			FInstanceItemData instanceItemCreated = ids[0];

			if (id.type == EItemType::Armour)
			{
				EquipArmour(UArmourCreator::GetOrCreateArmour(baseGameInstance, instanceItemCreated));
			}
			else if (id.type == EItemType::Weapon)
			{
				FWeaponData wd = GetGame()->GetWeaponData(id.ID);

				if (wd.type == EWeaponType::Projectile)
				{
					FRangedWeaponData rwd = GetGame()->GetRangedWeaponData(wd.ID);
					FProjectileWeaponData pwd = GetGame()->GetProjectileWeaponData(rwd.ID);
					FItemData ammoData = GetGame()->GetItemData(pwd.ammoID);

					// TODO figure out default ammo spawn system
					/*for (int i = 0; i < 1; ++i)
					{*/
					iid.amount = ammoData.maxStack * 3;
					iid.itemID = pwd.ammoID;

					ids.Empty();
					AddItem(iid, ids);
					//}
				}

				SetEquippedWeapon(UWeaponCreator::CreateWeapon(baseGameInstance, instanceItemCreated, id));
			}
		}
	}
}

/**
 *Helper method defining the slots that are valid for specific types of gear
 *
 *This needs to be change to use validSlots in UItemContainer
 *
 *@param type The type of gear to get the slot for
 */
TArray<int32> UInventory::GetSlotForGear(EGearType type)
{
	TArray<int32> validSlotsForGear;
	switch (type)
	{
	case EGearType::Head:
		validSlotsForGear.Add(0);
		break;
	case EGearType::Chest:
		validSlotsForGear.Add(1);
		break;
	case EGearType::Vest:
		validSlotsForGear.Add(2);
		break;
	case EGearType::Legs:
		validSlotsForGear.Add(3);
		break;
	case EGearType::Weapon:
		validSlotsForGear.Add(4);
		validSlotsForGear.Add(5);
		break;
	case EGearType::Sidearm:
		validSlotsForGear.Add(6);
		break;
	case EGearType::Bag:
		validSlotsForGear.Add(7);
		break;
	default:
		break;
	}
	return validSlotsForGear;
}

int32 UInventory::GetPrimaryWeaponSlot()
{
	return GetSlotForGear(EGearType::Weapon)[0];
}

int32 UInventory::GetSecondaryWeaponSlot()
{
	return GetSlotForGear(EGearType::Weapon)[1];
}

int32 UInventory::GetSidearmWeaponSlot()
{
	return GetSlotForGear(EGearType::Sidearm)[0];
}

UWorld* UInventory::GetWorld() const
{
	return characterOwner->GetWorld();
}

/**
 * Equips a weapon the character, setting the owner as well
 *
 *@param weapon The weapon to equip
 */
void UInventory::SetEquippedWeapon(UWeapon* weapon)
{
	UWeapon* oldWeapon = equippedWeapon;
	equippedWeapon = weapon;

	if (equippedWeapon)
	{
		equippedWeapon->SetOwner(characterOwner);
	}

	characterOwner->OnWeaponEquipped.Broadcast(oldWeapon);
}

/**
 *Equips the given armour to the character. Adds it the slot that is viable for the armour
 *
 *@param armour The armour to equip
 *
 */
void UInventory::EquipArmour(UArmour* armour)
{
	if (equippedArmour.Contains(armour->GetData().slot))
	{
		equippedArmour.Remove(armour->GetData().slot);
	}

	equippedArmour.Add(armour->GetData().slot, armour);
}


void UInventory::SetupLoadout(const FLoadoutData& ld)
{
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
				AddValidSlot(type, slot);
		}
	}

	CreateNewItemForInventory(GetGame()->GetItemDataByName("Knife").ID);
	CreateNewItemForInventory(ld.weaponID);
	CreateNewItemForInventory(ld.headArmourID);
	CreateNewItemForInventory(ld.chestArmourID);
	CreateNewItemForInventory(ld.vestArmourID);
	CreateNewItemForInventory(ld.legsArmourID);

	OnItemUpdated.AddUniqueDynamic(this, &UInventory::ItemUpdated);
}

void UInventory::GetItemArmourData(int32 instanceItemID, FInstanceArmourData& instanceArmourData, FArmourData& armourData)
{
	instanceArmourData = baseGameInstance->GetTableManager()->GetInstanceArmourDataByInstanceItemID(instanceItemID);
	armourData = baseGameInstance->GetTableManager()->GetArmourData(instanceArmourData.armourID);
}

void UInventory::CheckArmourItems(const FInstanceItemData& updatedInstanceItem, const FInstanceItemData& newInstanceItemData, const FInstanceItemData& oldInstanceItemData, const FItemData& newItemData, const FItemData& oldItemData)
{
	//UE_LOG(LogTemp, Log, TEXT("Armour moved from %d to %d"), oldInstanceItemData.containerInstanceID, newInstanceItemData.containerInstanceID);
	FInstanceArmourData newInstanceArmourData;
	FArmourData newArmourData;

	int32 slot = oldInstanceItemData.slot;
	GetItemArmourData(newInstanceItemData.ID, newInstanceArmourData, newArmourData);

	FInstanceArmourData oldInstanceArmourData;
	FArmourData oldArmourData;

	GetItemArmourData(oldInstanceItemData.ID, oldInstanceArmourData, oldArmourData);

	// If both items are armour, replace the old armour with the new
	if (newItemData.type == EItemType::Armour && oldItemData.type == EItemType::Armour)
	{
		if (GetSlotForGear(newArmourData.slot).Contains(slot))
		{
			EquipArmour(UArmourCreator::GetOrCreateArmour(baseGameInstance, updatedInstanceItem));
			UE_LOG(LogTemp, Log, TEXT("Armour Swapped from %s to %s"), *oldItemData.name, *newItemData.name);
		}
	}
	// Old slot was blank, so new armour is added
	else	if (newItemData.type == EItemType::Armour)
	{
		if (GetSlotForGear(newArmourData.slot).Contains(slot))
		{
			UE_LOG(LogTemp, Log, TEXT("Equipped Armour %s"), *newItemData.name);
			EquipArmour(UArmourCreator::GetOrCreateArmour(baseGameInstance, updatedInstanceItem));
		}
	}
	// Most likely armour replaced with nothing, i.e.e moved out of valid slot to blank space
	else if (oldItemData.type == EItemType::Armour)
	{
		if (GetSlotForGear(oldArmourData.slot).Contains(slot))
		{
			bool equippedArmourContains = equippedArmour.Contains(oldArmourData.slot);

			// Remove existing armour from equippedArmour
			if (equippedArmourContains && equippedArmour.FindChecked(oldArmourData.slot)->GetInstanceArmourData() == oldInstanceArmourData)
			{
				UE_LOG(LogTemp, Log, TEXT("Armour Removed %s"), *oldItemData.name);
				equippedArmour.Remove(oldArmourData.slot);
			}
		}
	}


	UE_LOG(LogTemp, Log, TEXT("Armour Resistance %d"), characterOwner->GetDamageResistance());
}

void UInventory::CheckWeaponItems(const FInstanceItemData& updatedInstanceItem, const FInstanceItemData& newInstanceItemData, const FInstanceItemData& oldInstanceItemData, const FItemData& newItemData, const FItemData& inOldItemData)
{
	int32 slot = oldInstanceItemData.slot;
	bool isWeaponSlot = GetSlotForGear(EGearType::Weapon).Contains(slot) || GetSlotForGear(EGearType::Sidearm).Contains(slot);

	// Have we moved from a weapon slot?
	if (isWeaponSlot)
	{
		// Replaced Weapon with weapon
		if (newItemData.type == EItemType::Weapon)
		{
			if (GetEquippedWeapon() != NULL)
			{
				FInstanceItemData equippedWeaponInstanceItemData = GetEquippedWeapon()->GetInstanceItemData();

				bool areDifferentInstances = equippedWeaponInstanceItemData.ID != newInstanceItemData.ID;
				bool doTheSlotsMatch = equippedWeaponInstanceItemData.slot == slot;

				/*UE_LOG(LogTemp, Log, TEXT("Weapon Equipped %s at slot %d"), *GetEquippedWeapon()->GetItemData().name, equippedWeaponInstanceItemData.slot);
				UE_LOG(LogTemp, Log, TEXT("areDifferentInstances %d"), areDifferentInstances);
				UE_LOG(LogTemp, Log, TEXT("doTheSlotsMatch %d"), doTheSlotsMatch);
				UE_LOG(LogTemp, Log, TEXT("Attempting to swap from %s to %s"), *inOldItemData.name, *newItemData.name);*/

				if (areDifferentInstances && doTheSlotsMatch)
				{
					UE_LOG(LogTemp, Log, TEXT("Weapon Swapped from %s to %s"), *inOldItemData.name, *newItemData.name);
					SetEquippedWeapon(UWeaponCreator::CreateWeapon(baseGameInstance, updatedInstanceItem, newItemData));
				}
			}
			// Nothing is equipped
			else
			{
				UE_LOG(LogTemp, Log, TEXT("Weapon Equipped %s"), *newItemData.name);
				SetEquippedWeapon(UWeaponCreator::CreateWeapon(baseGameInstance, updatedInstanceItem, newItemData));
			}
		}
		else if (inOldItemData.type == EItemType::Weapon)
		{
			if (GetEquippedWeapon() != NULL)
			{
				FInstanceItemData iid = baseGameInstance->GetTableManager()->GetInstanceItemDataByID(GetEquippedWeapon()->GetInstanceWeaponData().instanceItemID);

				if (iid.slot == slot)
				{
					UE_LOG(LogTemp, Log, TEXT("Weapon Removed %s"), *inOldItemData.name);
					SetEquippedWeapon(nullptr);
				}
			}
		}
	}
}

void UInventory::ItemUpdated(const FInstanceItemData& newItem, const FInstanceItemData& oldItem)
{
	FItemData newItemData = baseGameInstance->GetItemData(newItem.itemID);
	FItemData oldItemData = baseGameInstance->GetItemData(oldItem.itemID);
	FInstanceItemData updatedInstanceItem = newItem.CopySlotAndContainer(oldItem);

	if (newItemData.type == EItemType::Armour || oldItemData.type == EItemType::Armour)
	{
		CheckArmourItems(updatedInstanceItem, newItem, oldItem, newItemData, oldItemData);
	}
	else	if (newItemData.type == EItemType::Weapon || oldItemData.type == EItemType::Weapon)
	{
		CheckWeaponItems(updatedInstanceItem, newItem, oldItem, newItemData, oldItemData);
	}
}