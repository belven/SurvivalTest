#include "ItemContainer.h"
#include "../BaseGameInstance.h"
#include "SurvivalTest/Tables/ItemDataTable.h"

UItemContainer::UItemContainer() : Super()
{
}

UItemContainer* UItemContainer::CreateItemContainer(FContainerData inContainerData, FInstanceContainerData inInstanceContainerData, UBaseGameInstance* inGame)
{
	UItemContainer* ic = NewObject<UItemContainer>();
	ic->SetContainerData(inContainerData);
	ic->SetInstanceContainerData(inInstanceContainerData);
	ic->game = inGame;
	return ic;
}

FString UItemContainer::GetItemName(int32 itemID)
{
	return GetGame()->GetItemData(itemID).name;
}

int32 UItemContainer::GetNextItemID()
{
	return GetGame()->GetNextInstanceItemDataID();
}

int32 UItemContainer::GetItemStackSize(int32 itemID)
{
	return GetGame()->GetItemData(itemID).maxStack;
}

bool UItemContainer::HasSpace(FInstanceItemData item)
{
	return item.GetRemainingSpace(GetGame()->GetItemData(item.itemID).maxStack) > 0;
}


/**
 *
 *
 */
FInstanceItemData UItemContainer::GetItemAtSlot(int32 slot)
{
	FInstanceItemData id;

	for (FInstanceItemData item : GetItems())
	{
		if (item.slot == slot)
			return item;
	}

	return id;
}

FInstanceItemData UItemContainer::GetExistingItemWithSpace(FInstanceItemData inItem)
{
	for (FInstanceItemData item : GetItems())
	{
		// Finds the first item with space available and a matching name
		if (item.itemID == inItem.itemID && HasSpace(item))
		{
			return item;
		}
	}
	return FInstanceItemData();
}

void UItemContainer::TransferItem(UItemContainer* other, FInstanceItemData data, int32 droppedSlot)
{
	// Can't move items if we don't have space
	// TODO need to change this to check for adding parts of items, such as resources
	if (HasSpace())
	{
		// Get the item, if any, at the slot we've dropped the item onto
		FInstanceItemData existingItem = GetItemAtSlot(droppedSlot);

		// If the ID isn't -1, then we have an item in the slot
		if (existingItem.ID != UItemStructs::InvalidInt)
		{
			data.slot = GetNextEmptySlotForItem(data.itemID);
		}
		// If the ID is -1, then we don't have an item in the slot
		else
		{
			data.slot = droppedSlot;
		}

		// If we found a valid slot then transfer the item
		if (data.slot != UItemStructs::InvalidInt)
		{
			// Set the containerInstanceID to this container, this will move the item to belong to us
			data.containerInstanceID = GetInstanceContainerData().ID;

			// Update the data in the database
			GetGame()->AddUpdateData(data);

			// Tell our listeners that we've made changes, so things like UI can be updated
			other->OnItemRemoved.Broadcast(data);
			OnItemAdded.Broadcast(data);
			UpdateDebugItemsList();
		}
	}
}

/**
 * Gets the next slot for an item, taking into account the possibility of it having a EGearType,
 * and therefore possibly being invalid for our inventory. E.g. a weapon might not going into a rucksack, or bullets in a medical case
 *
 *@param itemID the ID of the FItemData we want to check
 *
 *@return The next valid empty slot found, if any. 
 */
int32 UItemContainer::GetNextEmptySlotForItem(int32 itemID)
{
	// Get the slot that the gear should go into, gear type is things like Legs, chest, primary weapon and bag
	EGearType gearType = GetGame()->GetGearTypeForItem(itemID);

	// If the gear type isn't End, then we found a slot for the item type, therefore we need to see if we have valid slots for the item
	// Otherwise, we found no gear type and can simply add the item to the next empty slot
	return gearType != EGearType::End ? FindNextEmptyValidSlot(gearType) : GetNextEmptySlot();
}

/** Adds an item to the inventory, if it finds an item with less than StackSize it adds the amount
* else it will create a new item with the remaining amount and set the one found to StackSize
*
*@param ids the list of new InstanceItemData ids created in the database
*@param itemToAdd the items to add to the inventory, typically only item creation will run this method, for instance, when first creating items from a loot box
*
* @return the input item with the amount set to the remainder if any, i.e. if it's not 0 then the inventory was full
*/
FInstanceItemData UItemContainer::AddItem(FInstanceItemData itemToAdd, TArray<int32>& ids)
{
	bool itemAdded = false;

	// Are we adding a whole item, i.e. an item that is at it's max stack size? If so, just add it
	if (HasSpace())
	{
		int32 emptySlot = GetNextEmptySlotForItem(itemToAdd.itemID);

		const int32 stackSize = GetItemStackSize(itemToAdd.itemID);
		if (itemToAdd.amount == stackSize)
		{
			if (emptySlot != UItemStructs::InvalidInt)
			{
				FInstanceItemData newItem = itemToAdd.CopyItem(emptySlot, GetNextItemID(), instanceContainerData.ID);
				newItem.amount = itemToAdd.amount;
				GetGame()->AddUpdateData(newItem);
				ids.Add(newItem.ID);
				itemToAdd.amount = 0;
				itemAdded = true;
			}
		}
		else
		{
			FInstanceItemData existingItem = GetExistingItemWithSpace(itemToAdd);

			// Check all existing matching items to see if they have space
			while (itemToAdd.amount > 0 && existingItem.isValid())
			{
				existingItem.TakeFrom(itemToAdd, stackSize);

				// Try to find another item to add to
				existingItem = GetExistingItemWithSpace(itemToAdd);
				GetGame()->GetInstancedItems().Add(existingItem.ID, existingItem);
				itemAdded = true;
			}

			// Keep adding new items until we're either full or added all items
			while (itemToAdd.amount > 0 && HasSpace())
			{
				// Get the next slot, taking into account invalid slot locations
				emptySlot = GetNextEmptySlotForItem(itemToAdd.itemID);

				if (emptySlot != UItemStructs::InvalidInt)
				{
					// Make a new item
					FInstanceItemData newItem = itemToAdd.CopyItem(emptySlot, GetNextItemID(), instanceContainerData.ID);
					newItem.amount = 0;
					newItem.TakeFrom(itemToAdd, stackSize);

					// Add the new item
					ids.Add(newItem.ID);
					GetGame()->AddUpdateData(newItem);
					itemAdded = true;
				}
				// We found no more valid slots for the item
				else
				{
					break;
				}
			}
		}
	}

	UpdateDebugItemsList();

	if (itemAdded)
	{
		OnItemAdded.Broadcast(itemToAdd);
	}
	return itemToAdd;
}

TArray<int32> UItemContainer::GetEmptySlots()
{
	TArray<int32> slotsLeft;

	for (int32 i = 0; i < GetMaxItemCount(); i++)
	{
		slotsLeft.Add(i);
	}

	RemoveFilledSlots(slotsLeft);
	return slotsLeft;
}

bool UItemContainer::HasSpace()
{
	return GetGame()->GetInstancedItemsForContainer(instanceContainerData.ID).Num() < containerData.slots;
}

/**
 *Searches through all current items and checks for an available validSlots, if any
 *
 * @return The zero indexed slot found, with no items stored OR UItemStructs::InvalidInt, if none is found
 */
int32 UItemContainer::GetNextEmptySlot()
{
	TArray<int32> slotsLeft = GetEmptySlots();

	if (!slotsLeft.IsEmpty())
	{
		return slotsLeft[0];
	}

	return UItemStructs::InvalidInt;
}

UBaseGameInstance* UItemContainer::GetGame()
{
	return game;
}

/**
 *Helper method that loops through our items and removes slots from the list, for existing items in that slot.
 *
 *@param slots The slot positions you want to remove if they are full
 *
 */
void UItemContainer::RemoveFilledSlots(TArray<int32>& slots)
{
	for (FInstanceItemData iid : GetItems())
	{
		slots.Remove(iid.slot);
	}
}

/**
 *Checks through validSlots, to see if the slot we're checking, is a valid location to store an item of a specific gear type
 *Don't want to store guns in a medical case etc.
 *
 *@param slot The slot we're checking
 *@param inType the gear type 
 *
 *@return True if the slot chosen can support the type of gear
 */
bool UItemContainer::IsValidForSlot(int32 slot, EGearType inType)
{
	// Do we have any valid slots set?
	if (validSlots.Num() > 0)
	{
		// Get the array of valid slots based on the gear type
		FValidSlots* slots = validSlots.Find(inType);

		// Check the valid slots found, if any, and return true if the slot we're checking is valid for that type of gear
		if (slots && slots->validSlots.Contains(slot))
		{
			return true;
		}

		// Either there were no slots defined for that gear type or no valid slots were found for the gear type at all
		return false;
	}

	// If we have no validSlots, then all slots are considered valid. It's easier to assume all good, as most containers will allow everything 
	return true;
}

/**
 *
 */
int32 UItemContainer::FindNextEmptyValidSlot(EGearType inType)
{
	// Do we have any valid slots defined?
	if (validSlots.Num() > 0)
	{
		// Get all the currently empty slots
		TArray<int32> emptySlots = GetEmptySlots();

		// Remove all the non empty slots from the valid ones
		for (int32 emptySlot : emptySlots)
		{
			// If the slot isn't valid for the item type, then remove it
			if (!IsValidForSlot(emptySlot, inType))
			{
				emptySlots.Remove(emptySlot);
			}
		}

		// Return the first empty slot
		if (emptySlots.Num() > 0)
		{
			return emptySlots[0];
		}

		return UItemStructs::InvalidInt;
	}

	// If there are no valid slots defined just get the next empty slot
	return GetNextEmptySlot();
}

void UItemContainer::UpdateDebugItemsList()
{
	TArray<FInstanceItemData> data = GetGame()->GetInventoryItems(instanceContainerData.ID);
	lastUpdatedItems.Empty();

	for (FInstanceItemData iid : data)
	{
		lastUpdatedItems.Add(FItemDataPair(iid, GetGame()->GetItemData(iid.itemID)));
	}
}

/**
 *This will reduce the an items amount by the given item if found
 *
 *@param itemToRemove The item type and amount to remove.
 *The amount could be more than the stack size, if we're removing a lot of items at once
 *
 *@return The itemToRemove with the new amount, in case any are left over
 */
FInstanceItemData UItemContainer::RemoveItem(FInstanceItemData itemToRemove)
{
	TArray<FInstanceItemData> itemsToRemove;

	for (FInstanceItemData item : GetItems())
	{
		int32 amountToTake = itemToRemove.amount;

		// Check if the IDs match
		if (item.itemID == itemToRemove.itemID)
		{
			// Simply remove it if the item has less than the required amount
			if (amountToTake >= item.amount)
			{
				itemsToRemove.Add(item);
				itemToRemove.amount = amountToTake - item.amount;
			}
			// Just change the amount
			else
			{
				itemToRemove.amount = 0;
				item.amount -= amountToTake;
			}
		}

		// Stop when we've run out of items
		if (amountToTake <= 0)
		{
			break;
		}
	}

	// Do we have items to remove?
	if (itemsToRemove.Num() > 0)
	{
		// Remove item data from the database
		for (FInstanceItemData ii : itemsToRemove)
		{
			GetGame()->GetInstancedItems().Remove(ii.ID);
		}
		OnItemRemoved.Broadcast(itemToRemove);
	}

	UpdateDebugItemsList();
	
	return itemToRemove;
}

int32 UItemContainer::GetItemAmount(int32 itemID)
{
	int32 total = 0;
	for (FInstanceItemData item : GetItems())
	{
		if (item.itemID == itemID)
		{
			total += item.amount;
		}
	}
	return total;
}
