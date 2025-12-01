#include "Inventory.h"

#include "Armour.h"
#include "ArmourCreator.h"
#include "Weapon.h"
#include "WeaponCreator.h"
#include "SurvivalTest/BaseCharacter.h"
#include "SurvivalTest/Tasks/EquipmentSwapTask.h"
#include "SurvivalTest/Tasks/TaskManagerComponent.h"

DEFINE_LOG_CATEGORY(Inventory)

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
		TArray<FInstanceItemData> newItemInstances;
		FItemsToAdd itemsToAdd(itemID, 1);
		FItemsToAdd& newItem = AddItem(itemsToAdd, newItemInstances);

		if (newItem.amount == 0 && !newItemInstances.IsEmpty())
		{
			FInstanceItemData instanceItemCreated = newItemInstances[0];

			if (id.type == EItemType::Armour)
			{
				EquipArmour(UArmourCreator::CreateArmour(baseGameInstance, instanceItemCreated));
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
					itemsToAdd = FItemsToAdd(pwd.ammoID, ammoData.maxStack * 3);
					newItemInstances.Empty();
					AddItem(itemsToAdd, newItemInstances);
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

/**
 * Equips a weapon the character, setting the owner as well
 *
 *@param weapon The weapon to equip
 */
void UInventory::SetEquippedWeapon(UWeapon* weapon)
{
	FString oldName = equippedWeapon ? equippedWeapon->GetItemData().name : "Empty";
	FString newName = weapon ? weapon->GetItemData().name : "Empty";
	UWeapon* oldWeapon = equippedWeapon;

	if (oldWeapon != NULL)
	{
		oldWeapon->Unequip();
	}

	equippedWeapon = weapon;

	if (weapon != NULL) 
	{
		equippedWeapon->Equip(characterOwner);
	}

	characterOwner->OnWeaponEquipped.Broadcast(oldWeapon);
	UE_LOG(LogTemp, Log, TEXT("Swapping from %s to %s"), *oldName, *newName);
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
			EquipArmour(UArmourCreator::CreateArmour(baseGameInstance, updatedInstanceItem));
			UE_LOG(Inventory, Log, TEXT("Armour Swapped from %s to %s"), *oldItemData.name, *newItemData.name);
		}
	}
	// Old slot was blank, so new armour is added
	else	if (newItemData.type == EItemType::Armour)
	{
		if (GetSlotForGear(newArmourData.slot).Contains(slot))
		{
			UE_LOG(Inventory, Log, TEXT("Equipped Armour %s"), *newItemData.name);
			EquipArmour(UArmourCreator::CreateArmour(baseGameInstance, updatedInstanceItem));
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
				UE_LOG(Inventory, Log, TEXT("Armour Removed %s"), *oldItemData.name);
				equippedArmour.Remove(oldArmourData.slot);
			}
		}
	}
}

void UInventory::CheckWeaponItems(const FInstanceItemData& oldInstanceItemData, const FItemData& newItemData, const FItemData& inOldItemData)
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

				bool doTheSlotsMatch = equippedWeaponInstanceItemData.slot == slot;

				// If the slots match, then we've replaced our equipped weapon, with a new one, so swap them over.
				// Otherwise, we've place a weapon in a weapon slot, but it's not an equipped one
				if (doTheSlotsMatch)
				{
					UE_LOG(Inventory, Log, TEXT("Weapon Swapped from %s to %s"), *inOldItemData.name, *newItemData.name);
					PerformWeaponSwap(slot);
				}
			}
			// Nothing is equipped
			else
			{
				UE_LOG(Inventory, Log, TEXT("Weapon Equipped %s"), *newItemData.name);
				PerformWeaponSwap(slot);
			}
		}
		// If only the old item is a weapon, then we've removed the weapon, so unequip it
		else if (inOldItemData.type == EItemType::Weapon)
		{
			if (GetEquippedWeapon() != NULL)
			{
				FInstanceItemData equippedWeaponInstanceItemData = GetEquippedWeapon()->GetInstanceItemData();

				if (equippedWeaponInstanceItemData.slot == slot)
				{
					UE_LOG(Inventory, Log, TEXT("Weapon Removed %s"), *inOldItemData.name);
					PerformWeaponSwap(slot);
				}
			}
		}
	}
}

void UInventory::PerformWeaponSwap(int32 slot)
{
	if (!equipmentSwapTask)
	{
		equipmentSwapTask = NewObject<UEquipmentSwapTask>();
	}

	equipmentSwapTask->SetSlot(slot);

	if (characterOwner->IsAlive()) {
		characterOwner->GetTaskManager()->PerformTask(equipmentSwapTask, false);
	}
}

/**
 * ItemUpdated This method is run when the characters inventory changes state, typically when the player moves items around. WE need to use this, to check if the weapons or armour has changed, so we can equip / unequip them correctly.
 * 
 * @param newItem The item that is being placed into the slot of the Old Item
 * @param oldItem The item that is being replaced, either with a new item or a blank item. I.e. you've removed your weapon or armour, opposed to replacing it
 */
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
		CheckWeaponItems(oldItem, newItemData, oldItemData);
	}
}