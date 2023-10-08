#include "Inventory.h"

#include "Armour.h"
#include "ArmourCreator.h"
#include "Weapon.h"
#include "WeaponCreator.h"
#include "SurvivalTest/BaseCharacter.h"

UInventory* UInventory::CreateInventory(FContainerData inContainerData, FInstanceContainerData inInstanceContainerData, UBaseGameInstance* inGame, ABaseCharacter* inCharacterOwner)
{
	UInventory* ic = NewObject<UInventory>();
	ic->SetContainerData(inContainerData);
	ic->SetInstanceContainerData(inInstanceContainerData);
	ic->game = inGame;
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
		FItemData id = game->GetItemData(itemID);
		TArray<int32> ids;
		FInstanceItemData iid(itemID, 1);

		if (AddItem(iid, ids).amount == 0)
		{
			int32 weaponInstanceItemID = ids.IsEmpty() ? UItemStructs::InvalidInt : ids[0];

			if (id.type == EItemType::Armour)
			{
				EquipArmour(UArmourCreator::CreateArmour(itemID, characterOwner->GetWorld(), ids[0]));
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
						AddItem(iid, ids);
					}
				}

				SetEquippedWeapon(UWeaponCreator::CreateWeapon(itemID, characterOwner->GetWorld(), weaponInstanceItemID));
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
	case EGearType::Head: validSlotsForGear.Add(0);
		break;
	case EGearType::Chest: validSlotsForGear.Add(1);
		break;
	case EGearType::Vest: validSlotsForGear.Add(2);
		break;
	case EGearType::Legs: validSlotsForGear.Add(3);
		break;
	case EGearType::Weapon:
		validSlotsForGear.Add(4);
		validSlotsForGear.Add(5);
		break;
	case EGearType::Sidearm: validSlotsForGear.Add(6);
		break;
	case EGearType::Bag: validSlotsForGear.Add(7);
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
	equippedArmour.FindOrAdd(armour->GetData().slot, armour);
}


void UInventory::SetupLoadout(FLoadoutData ld)
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

void UInventory::ItemUpdated(FInstanceItemData inItem, FInstanceItemData oldItem)
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
			else if (isWeaponSlot)
			{
				SetEquippedWeapon(UWeaponCreator::CreateWeapon(inItem.itemID, characterOwner->GetWorld(), inItem.ID));
			}
		}
		else if (isWeaponSlot)
		{
			SetEquippedWeapon(UWeaponCreator::CreateWeapon(inItem.itemID, characterOwner->GetWorld(), inItem.ID));
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
//
///**
// * An event listener for the players inventory that responds to items being added
// *
// *@param inItem The item added to the inventory
// *
// */
//void UInventory::ItemAdded(FInstanceItemData inItem)
//{
//	FItemData id = game->GetItemData(inItem.itemID);
//
//	// If we added armour, then update the Equipped armour
//	if (id.type == EItemType::Armour)
//	{
//		FInstanceArmourData iad = game->GetInstanceArmourDataByInstanceItemID(inItem.ID);
//		EquipArmour(UArmour::CreateArmour(inItem.itemID, game, inItem.ID));
//	}
//	// If we added a weapon and don't have one equipped, then equip it
//	else if (id.type == EItemType::Weapon)
//	{
//		FWeaponData wd = game->GetWeaponData(id.ID);
//		if (!GetEquippedWeapon() && GetSlotForGear(wd.gearType).Contains(inItem.slot))
//		{
//			SetEquippedWeapon(UWeaponCreator::CreateWeapon(inItem.itemID, characterOwner->GetWorld(), inItem.ID));
//		}
//	}
//	OnContainersUpdated.Broadcast();
//}
//
///**
// * An event listener for the players inventory that responds to items being removed
// *
// *@param inItem The item removed from the inventory
// *
// */
//void UInventory::ItemRemoved(FInstanceItemData inItem)
//{
//	FItemData id = game->GetItemData(inItem.itemID);
//
//	if (id.type == EItemType::Weapon)
//	{
//		if (GetEquippedWeapon() && GetEquippedWeapon()->GetInstanceWeaponData().instanceItemID == inItem.ID)
//		{
//			SetEquippedWeapon(nullptr);
//		}
//	}
//	else if (id.type == EItemType::Armour)
//	{
//		UArmour* armourFound = nullptr;
//
//		for (auto& a : equippedArmour)
//		{
//			if (a.Value->GetInstanceArmourData().instancedItemDataID == inItem.ID)
//			{
//				armourFound = a.Value;
//				break;
//			}
//		}
//
//		if (armourFound)
//		{
//			equippedArmour.Remove(armourFound->GetData().slot);
//		}
//	}
//	OnContainersUpdated.Broadcast();
//}