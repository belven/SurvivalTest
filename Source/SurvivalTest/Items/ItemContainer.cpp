#include "ItemContainer.h"
#include "../BaseGameInstance.h"

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

int32 UItemContainer::GetNextInstanceItemDataID()
{
	return GetGame()->GetNextInstanceItemDataID();
}

int32 UItemContainer::GetItemStackSize(int32 itemID)
{
	return GetGame()->GetItemData(itemID).maxStack;
}

bool UItemContainer::HasSpace(FInstanceItemData item)
{
	return item.HasSpace(GetGame()->GetItemData(item.itemID).maxStack);
}

FInstanceItemData UItemContainer::GetInstanceItemAtSlot(int32 slot)
{
	FInstanceItemData id;

	for (FInstanceItemData item : GetItems())
	{
		if (item.slot == slot)
			return item;
	}

	return id;
}

TArray<FInstanceItemData> UItemContainer::GetExistingItemsWithSpace(int32 itemID)
{
	TArray<FInstanceItemData> itemsFound;

	for (FInstanceItemData iid : GetItems())
	{
		if (iid.itemID == itemID && HasSpace(iid))
		{
			itemsFound.Add(iid);
		}
	}
	return itemsFound;
}

/**
 * Checks to see if the item we're dragging, is a container itself, and we're not dragging a container, inside a container, the first one contains.
 *
 * I.e. if we have a chest piece (A), that has a chest piece (B) inside it, then we should not be able to drag A inside B, as they would then contain each other
 *
 * @return True if should continue with the exchange, False if not
 */
bool UItemContainer::CheckForArmourInventory(FInstanceItemData& itemToTransfer)
{
	FItemData id = GetGame()->GetItemData(itemToTransfer.itemID);

	// If the item type is armour, check we're not adding to ourselves
	if (id.type == EItemType::Armour)
	{
		// Find the armour instance, containerInstanceID for the armour item
		FInstanceArmourData iad = GetGame()->GetInstanceArmourDataByInstanceItemID(itemToTransfer.ID);

		// If the containerInstanceID is the same as this container, then we're dragging the armour item, into it's own container
		if (iad.containerInstanceID == GetContainerInstanceID())
		{
			return false;
		}
		else
		// TODO This only works for 1 layer of container, would be worth making a recursive method for this
		// This is also solved by simply not allowing armour in armour etc.
		{
			bool selfFound = false;
			// Other is the container for the item we've dragged
			// We need to find all the items contained in the armour we dragged
			for (FInstanceItemData iid : GetGame()->GetInstancedItemsForContainer(iad.containerInstanceID))
			{
				if (id.type == EItemType::Armour)
				{
					iad = GetGame()->GetInstanceArmourDataByInstanceItemID(iid.ID);

					if (iad.containerInstanceID == GetContainerInstanceID())
					{
						selfFound = true;
					}
				}
			}

			if (selfFound)
				return false;
		}
	}

	return true;
}

void UItemContainer::SwapItems(UItemContainer* other, FInstanceItemData& itemToTransfer, const int32 droppedSlot, const FInstanceItemData& oldData, int32 maxStackSize, FInstanceItemData& existingItem)
{
	// Are we dropping something that can stack more than once and the item we're dropping onto is the same
	if (maxStackSize > 1 && itemToTransfer.itemID == existingItem.itemID)
	{
		FInstanceItemData oldExistingItem = existingItem;

		// Are either of the items at max stack?
		// If so, we're trying to add an item to another
		if (!existingItem.HasSpace(maxStackSize) || !itemToTransfer.HasSpace(maxStackSize))
		{
			// if the existingItem.amount is less than the itemToTransfer.amount, then swap amounts
			if (existingItem.amount < itemToTransfer.amount)
			{
				existingItem.TakeFrom(itemToTransfer, maxStackSize);
			}
			// Otherwise we're just swapping the items, as existingItem.amount is at max.
			// This would be like swapping a half stack for a full stack
			else
			{
				itemToTransfer.TakeFrom(existingItem, maxStackSize);
			}
		}
		// Neither item is full, so simply add the dropped item amount onto the existing item
		else
		{
			existingItem.TakeFrom(itemToTransfer, maxStackSize);
		}

		UpdateItemData(this, existingItem, oldExistingItem);

		if (itemToTransfer.amount == 0)
		{
			RemoveInstanceItem(other, itemToTransfer, oldData);
		}
		else
		{
			UpdateItemData(other, itemToTransfer, oldData);
		}
	}
	// We have an item that only stacks once and the items are in the same container, so swap the items
	else
	{
		MoveItemToSlot(other, itemToTransfer, droppedSlot, existingItem);
		other->MoveItemToSlot(this, existingItem, oldData.slot, oldData);
	}
}

void UItemContainer::MoveItemToSlot(UItemContainer* other, FInstanceItemData& itemToTransfer, const int32 slot, const FInstanceItemData& oldData)
{
	if (slot != UItemStructs::InvalidInt)
	{
		itemToTransfer.slot = slot;

		// Is the item to transfer not from this container?
		if (other->GetContainerInstanceID() != GetContainerInstanceID())
		{
			// Set the containerInstanceID to this container, this will move the item to belong to us
			itemToTransfer.containerInstanceID = GetContainerInstanceID();
			
			AddUpdateItemData(itemToTransfer);
			other->OnItemUpdated.Broadcast(itemToTransfer, oldData);
			OnItemUpdated.Broadcast(itemToTransfer, oldData);
		}
		else
		{
			// Update the itemToTransfer in the database
			UpdateItemData(other, itemToTransfer, oldData);
		}
	}
}

void UItemContainer::MoveItemToEmptySlot(UItemContainer* other, FInstanceItemData& itemToTransfer, const FInstanceItemData& oldData)
{
	MoveItemToSlot(other, itemToTransfer, GetNextEmptySlotForItem(itemToTransfer.itemID), oldData);
}

void UItemContainer::FillExistingItems(FInstanceItemData& itemToTransfer, int32 maxStackSize)
{
	TArray<FInstanceItemData> itemsFound = GetExistingItemsWithSpace(itemToTransfer.itemID);

	// If we found items, then try and add to them
	if (itemsFound.Num() > 0)
	{
		for (FInstanceItemData iid : itemsFound)
		{
			// Check for self, as we can be in the same container
			// Check if we have space
			if (iid.ID != itemToTransfer.ID && iid.HasSpace(maxStackSize))
			{
				FInstanceItemData oldData = iid;
				iid.TakeFrom(itemToTransfer, maxStackSize);

				// Update each amount increase
				UpdateItemData(this, iid, oldData);

				if (itemToTransfer.amount == 0)
				{
					break;
				}
			}
		}
	}
}

void UItemContainer::DropOnExistingItem(UItemContainer* other, FInstanceItemData& itemToTransfer, const int32 droppedSlot, const FInstanceItemData& oldData, int32 maxStack, FInstanceItemData& existingItem, EGearType type)
{
	EGearType existingType = GetGame()->GetGearTypeForItem(existingItem.itemID);
	bool canSwitchPlaces = IsValidForSlot(droppedSlot, type) && other->IsValidForSlot(itemToTransfer.slot, existingType);

	// Check if our current item is valid for the dropped slot and if the existing item is valid for the other slot
	if (canSwitchPlaces)
	{
		SwapItems(other, itemToTransfer, droppedSlot, oldData, maxStack, existingItem);
	}
	// If the item has no space, then it's a whole stack and should go into the next valid slot, if any
	// No point adding it to others if it's a whole stack, this also helps dealing with single item stacks of armour and weapons
	else if (!HasSpace(itemToTransfer))
	{
		MoveItemToEmptySlot(other, itemToTransfer, oldData);
	}
	// We're not full so try and find existing items OR add it to the inventory as is
	else
	{
		FillExistingItemsWithDroppedItem(other, itemToTransfer, oldData, maxStack);
	}
}

void UItemContainer::AddUpdateItemData(FInstanceItemData& existingItem)
{
	GetGame()->AddUpdateData(existingItem);
}

void UItemContainer::UpdateItemData(UItemContainer* container, FInstanceItemData& existingItem, const FInstanceItemData& oldData)
{
	AddUpdateItemData(existingItem);
	container->OnItemUpdated.Broadcast(existingItem, oldData);
}

void UItemContainer::RemoveInstanceItem(UItemContainer* other, FInstanceItemData& itemToTransfer, const FInstanceItemData& oldData)
{
	GetGame()->GetInstancedItems().Remove(itemToTransfer.ID);
	other->OnItemUpdated.Broadcast(FInstanceItemData(), oldData);
}

void UItemContainer::FillExistingItemsWithDroppedItem(UItemContainer* other, FInstanceItemData& itemToTransfer, const FInstanceItemData& oldData, int32 maxStackSize)
{
	// Does the item stack more than once?
	if (maxStackSize > 1)
	{
		FillExistingItems(itemToTransfer, maxStackSize);

		if (itemToTransfer.amount > 0)
		{
			// Update the itemToTransfer in the database
			UpdateItemData(other, itemToTransfer, oldData);

			MoveItemToEmptySlot(other, itemToTransfer, oldData);
		}
		else
		{
			RemoveInstanceItem(other, itemToTransfer, oldData);
		}
	}
	// We're dealing with a single stack item, so try and add it to the next empty slot
	else if (maxStackSize == 1)
	{
		MoveItemToEmptySlot(other, itemToTransfer, oldData);
	}
}

/**
 *
 */
FInstanceItemData& UItemContainer::TransferItem(UItemContainer* other, FInstanceItemData itemToTransfer, const int32 droppedSlot)
{
	// Store a copy of the data so we can use it to update the UI
	FInstanceItemData oldData = itemToTransfer;
	FItemData id = GetGame()->GetItemData(itemToTransfer.itemID);

	// Check we're not trying to add a container to a container, i.e. dragging a bag inside itself
	if (CheckForArmourInventory(itemToTransfer))
	{
		// If dropped slot is invalid, then we're transferring items programatically, i.e. a take all method or shift clicking items
		// Otherwise we've manually moved the item onto a slot
		if (droppedSlot != UItemStructs::InvalidInt)
		{
			// Get the item, if any, at the slot we've dropped the item onto
			FInstanceItemData existingItem = GetInstanceItemAtSlot(droppedSlot);
			EGearType type = GetGame()->GetGearTypeForItem(itemToTransfer.itemID);

			// If the ID isn't -1, then we have an existing item in the slot
			if (existingItem.isValid())
			{
				DropOnExistingItem(other, itemToTransfer, droppedSlot, oldData, id.maxStack, existingItem, type);
			}
			// If the ID is -1, then we don't have an existing item in the slot
			else if (IsValidForSlot(droppedSlot, type))
			{
				MoveItemToSlot(other, itemToTransfer, droppedSlot, oldData);
			}
			// If we're here, there's no existing item but the slot we've dropped it onto isn't valid
			// I.e. you've dropped some food into a weapon / bag only slot
			// Try and add the item to either existing items or an empty slot in our inventory
			else
			{
				FillExistingItemsWithDroppedItem(other, itemToTransfer, oldData, id.maxStack);
			}
		}
		// There was no dropped slot, so try and add the item to either existing items or an empty slot in our inventory 
		else
		{
			FillExistingItemsWithDroppedItem(other, itemToTransfer, oldData, id.maxStack);
		}
	}

	UpdateDebugItemsList();
	return itemToTransfer;
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
	return FindNextEmptyValidSlot(gearType);
}

/**
* Adds an item to the inventory, if it finds an item with less than StackSize it adds the amount
* else it will create a new item with the remaining amount and set the one found to StackSize
*
*@param ids the list of new InstanceItemData ids created in the database
*@param itemToAdd the items to add to the inventory, typically only item creation will run this method, for instance, when first creating items from a loot box
*
* @return the input item with the amount set to the remainder if any, i.e. if it's not 0 then the inventory was full
*/
FInstanceItemData& UItemContainer::AddItem(FInstanceItemData& itemToAdd, TArray<int32>& ids)
{
	// Are we adding a whole item, i.e. an item that is at it's max stack size? If so, just add it
	if (HasSpace())
	{
		int32 emptySlot = GetNextEmptySlotForItem(itemToAdd.itemID);

		const int32 stackSize = GetItemStackSize(itemToAdd.itemID);
		if (itemToAdd.amount == stackSize)
		{
			if (emptySlot != UItemStructs::InvalidInt)
			{
				FInstanceItemData newItem = itemToAdd.CopyItem(GetNextInstanceItemDataID(), instanceContainerData.ID, emptySlot);
				newItem.amount = itemToAdd.amount;
				AddUpdateItemData(newItem);
				OnItemUpdated.Broadcast(newItem, FInstanceItemData());

				ids.Add(newItem.ID);
				itemToAdd.amount = 0;
			}
		}
		else
		{
			FillExistingItems(itemToAdd, stackSize);

			// Keep adding new items until we're either full or added all items
			while (itemToAdd.amount > 0 && HasSpace())
			{
				// Get the next slot, taking into account invalid slot locations
				emptySlot = GetNextEmptySlotForItem(itemToAdd.itemID);

				if (emptySlot != UItemStructs::InvalidInt)
				{
					// Make a new item
					FInstanceItemData newItem = itemToAdd.CopyItem(GetNextInstanceItemDataID(), instanceContainerData.ID, emptySlot);
					newItem.TakeFrom(itemToAdd, stackSize);

					// Add the new item
					ids.Add(newItem.ID);
					AddUpdateItemData(newItem);
					OnItemUpdated.Broadcast(newItem, FInstanceItemData());
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
	return itemToAdd;
}

TArray<int32> UItemContainer::GetEmptySlots()
{
	TArray<int32> slotsFilled;
	TArray<int32> emptySlots;

	for (FInstanceItemData iid : GetItems())
	{
		slotsFilled.Add(iid.slot);
	}

	for (int32 i = 0; i < GetMaxItemCount(); i++)
	{
		if (!slotsFilled.Contains(i))
			emptySlots.Add(i);
	}

	return emptySlots;
}

bool UItemContainer::HasSpace()
{
	int32 num = GetItems().Num();
	return num < containerData.slots;
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

int32 UItemContainer::GetContainerInstanceID()
{
	return GetInstanceContainerData().ID;
}

void UItemContainer::SplitItem(const FInstanceItemData itemToSplit)
{
	if (HasSpace() && itemToSplit.amount > 1 && GetItemStackSize(itemToSplit.itemID) > 1)
	{
		int32 emptySlot = GetNextEmptySlotForItem(itemToSplit.itemID);

		if (emptySlot != UItemStructs::InvalidInt)
		{
			const int32 nextID = GetNextInstanceItemDataID();
			FInstanceItemData iid = itemToSplit;

			const int32 total = iid.amount;
			float halfAmountFloat = total / 2.0f;
			int32 halfAmountInt = FMath::RoundToInt32(halfAmountFloat);
			iid.amount = halfAmountInt;

			UpdateItemData(this, iid, itemToSplit);

			int32 newAmount = total - halfAmountInt;

			// Create a new item
			FInstanceItemData newItem = iid.CopyItem(nextID, GetContainerInstanceID(), emptySlot, newAmount);

			AddUpdateItemData(newItem);
			OnItemUpdated.Broadcast(newItem, FInstanceItemData());

			UpdateDebugItemsList();
		}
	}
}

UBaseGameInstance* UItemContainer::GetGame()
{
	return game;
}

void UItemContainer::AddValidSlot(EGearType type, const int32 slot)
{
	validSlots.FindOrAdd(slot).validGear.Add(type);
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
bool UItemContainer::IsValidForSlot(const int32 slot, const EGearType inType)
{
	// Do we have any valid slots set?
	if (validSlots.Num() > 0)
	{
		// Get the array of valid slots based on the gear type
		FValidSlots* slots = validSlots.Find(slot);

		// Check the valid slots found, if any, and return true if the slot we're checking is valid for that type of gear
		if (!slots)
		{
			return true;
		}

		if (slots->validGear.Contains(inType))
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
	if (!validSlots.IsEmpty())
	{
		TArray<int32> slotsToRemove;

		// Get all the currently empty slots
		TArray<int32> emptySlots = GetEmptySlots();

		// Remove all the non empty slots from the valid ones
		for (int32 emptySlot : emptySlots)
		{
			// If the slot isn't valid for the item type, then remove it
			if (!IsValidForSlot(emptySlot, inType))
			{
				slotsToRemove.Add(emptySlot);
			}
			else
			{
				return emptySlot;
			}
		}

		return UItemStructs::InvalidInt;
	}

	// If there are no valid slots defined just get the next empty slot
	return GetNextEmptySlot();
}

void UItemContainer::UpdateDebugItemsList()
{
	TArray<FInstanceItemData> data = GetGame()->GetInstancedItemsForContainer(instanceContainerData.ID);
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
	for (FInstanceItemData item : GetItems())
	{
		FInstanceItemData oldData = item;

		// Check if the IDs match
		if (item.itemID == itemToRemove.itemID)
		{
			// Simply remove the item, if it has less than or equal to the required amount
			if (item.amount <= itemToRemove.amount)
			{
				itemToRemove.amount -= item.amount;
				RemoveInstanceItem(this, item, item);
			}
			// Otherwise item.amount is greater than the amount to remove
			// So just reduce the value by the amount to remove, and zero the amount to remove
			else
			{
				item.amount -= itemToRemove.amount;
				itemToRemove.amount = 0;

				UpdateItemData(this, item, oldData);
			}

			// Stop when we've run out of items to remove
			if (itemToRemove.amount <= 0)
			{
				break;
			}
		}
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
