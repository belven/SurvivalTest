#include "ItemContainer.h"

#include <iostream>

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
	bool result = false;
	FItemData id = GetGame()->GetItemData(itemToTransfer.itemID);

	// If the item type is armour, check we're not adding to ourselves
	if (id.type == EItemType::Armour)
	{
		// Find the armour instance, containerInstanceID for the armour item
		FInstanceArmourData iad = GetGame()->GetInstanceArmourDataByInstanceItemID(itemToTransfer.ID);

		// TODO This only works for 1 layer of container, would be worth making a recursive method for this
		// Also, this is solved by simply not allowing armour in armour etc.

		// If the containerInstanceID is the same as this container, then we're dragging the armour item, into its own container
		if (iad.containerInstanceID != GetContainerInstanceID())
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
						break;
					}
				}
			}

			if (!selfFound) {
				result = true;
			}
		}
	}
	// Otherwise continue
	else
	{
		result = true;
	}

	return result;
}

void UItemContainer::SwapItems(UItemContainer* other, FInstanceItemData& itemToTransfer, const int32 droppedSlot, const FInstanceItemData& originalItemData, int32 maxStackSize, FInstanceItemData& existingItem)
{
	// Are we dropping something that can stack more than once and the item we're dropping onto is the same
	if (maxStackSize > 1 && itemToTransfer.itemID == existingItem.itemID)
	{
		UE_LOG(LogTemp, Log, TEXT("SwapItems same Item ID"));
		FInstanceItemData oldExistingItem = existingItem;

		// Are either of the items at max stack?
		// If so, we're trying to add an item to another
		if (!existingItem.HasSpace(maxStackSize) || !itemToTransfer.HasSpace(maxStackSize))
		{
			UE_LOG(LogTemp, Log, TEXT("SwapItems same Item ID, Swap amounts"));

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
			UE_LOG(LogTemp, Log, TEXT("SwapItems same Item ID, Add amounts together"));
			existingItem.TakeFrom(itemToTransfer, maxStackSize);
		}

		UpdateItemData(this, existingItem, oldExistingItem);

		if (itemToTransfer.amount == 0)
		{
			UE_LOG(LogTemp, Log, TEXT("SwapItems Remove old Item"));
			RemoveInstanceItem(other, itemToTransfer);
		}
		else
		{
			UE_LOG(LogTemp, Log, TEXT("SwapItems Update old Item"));
			UpdateItemData(other, itemToTransfer, originalItemData);
		}
	}
	// We have an item that only stacks once and the items are in the same container, so swap the items
	else
	{
		UE_LOG(LogTemp, Log, TEXT("SwapItems Single Stack Item"));

		FInstanceItemData otherItem = itemToTransfer;
		FInstanceItemData ourItem = existingItem;

		int otherSlot = otherItem.slot;

		otherItem.containerInstanceID = GetContainerInstanceID();
		otherItem.slot = ourItem.slot;

		UpdateItemData(this, otherItem, ourItem);

		ourItem.containerInstanceID = other->GetContainerInstanceID();
		ourItem.slot = otherSlot;

		UpdateItemData(other, ourItem, itemToTransfer);
	}
}

void UItemContainer::MoveItemToSlot(UItemContainer* container, FInstanceItemData& itemToTransfer, const int32 slot, const FInstanceItemData& originalItemData)
{
	if (slot != UItemStructs::InvalidInt)
	{
		// Set the containerInstanceID to this container, this will move the item to belong to us
		itemToTransfer.containerInstanceID = container->GetContainerInstanceID();
		itemToTransfer.slot = slot;

		UpdateItemData(container, itemToTransfer, originalItemData);
	}
}

//void UItemContainer::MoveItemToEmptySlot(UItemContainer* otherContainer, FInstanceItemData& itemToTransfer)
//{
//	FInstanceItemData originalItemData = itemToTransfer;
//
//	FInstanceItemData blankData = FInstanceItemData(originalItemData.slot);
//	int32 emptySlot = GetNextEmptySlotForItem(itemToTransfer.itemID);
//
//	MoveItemToSlot(this, itemToTransfer, emptySlot, originalItemData);
//
//	MoveItemToSlot(otherContainer, blankData, originalItemData.slot, itemToTransfer);
//}

void UItemContainer::MoveItemToEmptySlot(UItemContainer* sourceContainer, UItemContainer* destContainer, FInstanceItemData& itemToTransfer)
{
	// Find destination empty slot
	int32 emptySlot = destContainer->GetNextEmptySlotForItem(itemToTransfer.itemID);
	if (!UItemStructs::IsValidID(emptySlot))
	{
		UE_LOG(LogTemp, Warning, TEXT("MoveItemToEmptySlot: No valid empty slot in destination container %d"), destContainer->GetContainerInstanceID());
		return;
	}

	// Store the original copy for UI update purposes
	FInstanceItemData originalItemData = itemToTransfer;

	// Blank data for clearing the old slot in the source
	FInstanceItemData blankData(originalItemData.slot);

	// Move the item into destination
	destContainer->MoveItemToSlot(destContainer, itemToTransfer, emptySlot, originalItemData);

	// Clear the original slot in the source
	sourceContainer->MoveItemToSlot(sourceContainer, blankData, originalItemData.slot, itemToTransfer);
}

void UItemContainer::FillExistingItems(FInstanceItemData& itemToTransfer, int32 maxStackSize)
{
	TArray<FInstanceItemData> itemsFound = GetExistingItemsWithSpace(itemToTransfer.itemID);

	for (FInstanceItemData iid : itemsFound)
	{
		// Check for self, as we can be in the same container
		// Check if we have space
		if (iid.ID != itemToTransfer.ID && iid.HasSpace(maxStackSize))
		{
			FInstanceItemData originalItemData = iid;
			iid.TakeFrom(itemToTransfer, maxStackSize);

			// Update each amount increase
			UpdateItemData(this, iid, originalItemData);

			if (itemToTransfer.amount == 0)
			{
				break;
			}
		}
	}

}

void UItemContainer::DropOnExistingItem(UItemContainer* other, FInstanceItemData& itemToTransfer, const int32 droppedSlot, const FInstanceItemData& originalItemData, int32 maxStack, FInstanceItemData& existingItem, EGearType type)
{
	EGearType existingType = GetGame()->GetGearTypeForItem(existingItem.itemID);
	bool canSwitchPlaces = IsValidForSlot(droppedSlot, type) && other->IsValidForSlot(itemToTransfer.slot, existingType);

	// Check if our current item is valid for the dropped slot and if the existing item is valid for the other slot
	if (canSwitchPlaces)
	{
		UE_LOG(LogTemp, Log, TEXT("DropOnExistingItem canSwitchPlaces"));
		SwapItems(other, itemToTransfer, droppedSlot, originalItemData, maxStack, existingItem);
	}
	// If the item has no space, then it's a whole stack and should go into the next valid slot, if any
	// No point adding it to others if it's a whole stack, this also helps dealing with single item stacks of armour and weapons
	else if (!HasSpace(itemToTransfer))
	{
		UE_LOG(LogTemp, Log, TEXT("DropOnExistingItem MoveItemToEmptySlot"));
		MoveItemToEmptySlot(other, this, itemToTransfer);
	}
	// We're not full so try and find existing items OR add it to the inventory as is
	else
	{
		UE_LOG(LogTemp, Log, TEXT("DropOnExistingItem FillExistingItemsWithDroppedItem"));
		FillExistingItemsWithDroppedItem(other, itemToTransfer, originalItemData, maxStack);
	}
}

void UItemContainer::AddUpdateItemData(FInstanceItemData& existingItem)
{
	GetGame()->AddUpdateData(existingItem);
}

void UItemContainer::UpdateItemData(UItemContainer* container, FInstanceItemData& newItem, const FInstanceItemData& OldItem)
{
	AddUpdateItemData(newItem);
	container->OnItemUpdated.Broadcast(newItem, OldItem);
}

void UItemContainer::RemoveInstanceItem(UItemContainer* other, FInstanceItemData& itemToDelete)
{
	GetGame()->GetInstancedItems().Remove(itemToDelete.ID);
	other->OnItemUpdated.Broadcast(itemToDelete, itemToDelete);
}

void UItemContainer::FillExistingItemsWithDroppedItem(UItemContainer* other, FInstanceItemData& itemToTransfer, const FInstanceItemData& originalItemData, int32 maxStackSize)
{
	// Does the item stack more than once?
	if (maxStackSize > 1)
	{
		FillExistingItems(itemToTransfer, maxStackSize);

		if (itemToTransfer.amount > 0)
		{
			// Update the itemToTransfer in the database
			UpdateItemData(other, itemToTransfer, originalItemData);

			MoveItemToEmptySlot(other, this, itemToTransfer);
		}
		else
		{
			RemoveInstanceItem(other, itemToTransfer);
		}
	}
	// We're dealing with a single stack item, so try and add it to the next empty slot
	else if (maxStackSize == 1)
	{
		MoveItemToEmptySlot(other, this, itemToTransfer);
	}
}

/**
 *
 */
FInstanceItemData& UItemContainer::TransferItem(UItemContainer* sourceInventory, FInstanceItemData itemToTransfer, const int32 droppedSlot)
{
	UE_LOG(LogTemp, Log, TEXT("TransferItem"));

	// Store a copy of the data so we can use it to update the UI
	FInstanceItemData originalItemData = itemToTransfer;
	FItemData id = GetGame()->GetItemData(itemToTransfer.itemID);

	// Check we're not trying to add a container to a container, i.e. dragging a bag inside itself
	if (CheckForArmourInventory(itemToTransfer))
	{
		// If dropped slot is invalid, then we're transferring items programatically, i.e. a take all method or shift clicking items
		// Otherwise we've manually moved the item onto a slot
		if (droppedSlot != UItemStructs::InvalidInt)
		{
			UE_LOG(LogTemp, Log, TEXT("TransferItem Manual Transfer"));

			// Get the item, if any, at the slot we've dropped the item onto
			FInstanceItemData existingItem = GetInstanceItemAtSlot(droppedSlot);
			EGearType type = GetGame()->GetGearTypeForItem(itemToTransfer.itemID);

			// If the ID isn't -1, then we have an existing item in the slot
			if (existingItem.isValid())
			{
				UE_LOG(LogTemp, Log, TEXT("TransferItem Manual Transfer Existing Item"));
				DropOnExistingItem(sourceInventory, itemToTransfer, droppedSlot, originalItemData, id.maxStack, existingItem, type);
			}
			// If the ID is -1, then we don't have an existing item in the slot
			else if (IsValidForSlot(droppedSlot, type))
			{
				UE_LOG(LogTemp, Log, TEXT("TransferItem Manual Transfer Empty Slot"));
				MoveItemToSlot(this, itemToTransfer, droppedSlot, existingItem);

				sourceInventory->OnItemUpdated.Broadcast(FInstanceItemData(originalItemData.slot), originalItemData);
			}
			// If we're here, there's no existing item but the slot we've dropped it onto isn't valid
			// I.e. you've dropped some food into a weapon / bag only slot
			// Try and add the item to either existing items or an empty slot in our inventory
			else
			{
				UE_LOG(LogTemp, Log, TEXT("TransferItem Manual Transfer Existing Item, Invalid Slot"));
				FillExistingItemsWithDroppedItem(sourceInventory, itemToTransfer, originalItemData, id.maxStack);
			}
		}
		// There was no dropped slot, so try and add the item to either existing items or an empty slot in our inventory 
		else
		{
			UE_LOG(LogTemp, Log, TEXT("TransferItem Manual Fill Existing Items"));
			FillExistingItemsWithDroppedItem(sourceInventory, itemToTransfer, originalItemData, id.maxStack);
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
	UE_LOG(LogTemp, Log, TEXT("Add Item"));

	// Are we adding a whole item, i.e. an item that is at it's max stack size? If so, just add it
	if (HasSpace())
	{
		int32 emptySlot = GetNextEmptySlotForItem(itemToAdd.itemID);

		const int32 stackSize = GetItemStackSize(itemToAdd.itemID);

		if (itemToAdd.amount == stackSize)
		{
			if (emptySlot != UItemStructs::InvalidInt)
			{
				UE_LOG(LogTemp, Log, TEXT("Add Item Max Stack"));

				FInstanceItemData newItem = itemToAdd.CopyItem(GetNextInstanceItemDataID(), GetContainerInstanceID(), emptySlot, itemToAdd.amount);
				UpdateItemData(this, newItem, FInstanceItemData(emptySlot));

				ids.Add(newItem.ID);
				itemToAdd.amount = 0;
			}
		}
		else
		{
			UE_LOG(LogTemp, Log, TEXT("Add Item FillExistingItems"));
			FillExistingItems(itemToAdd, stackSize);

			// Keep adding new items until we're either full or added all items
			while (itemToAdd.amount > 0 && HasSpace())
			{
				if (UItemStructs::IsValidID(emptySlot))
				{
					UE_LOG(LogTemp, Log, TEXT("Add Item New Item"));

					// Make a new item
					FInstanceItemData newItem = itemToAdd.CopyItem(GetNextInstanceItemDataID(), GetContainerInstanceID(), emptySlot);
					newItem.TakeFrom(itemToAdd, stackSize);

					// Add the new item
					ids.Add(newItem.ID);
					UpdateItemData(this, newItem, FInstanceItemData(emptySlot));
				}
				// We found no more valid slots for the item
				else
				{
					break;
				}

				// Get the next slot, taking into account invalid slot locations
				emptySlot = GetNextEmptySlotForItem(itemToAdd.itemID);
			}
		}
	}

	UpdateDebugItemsList();
	return itemToAdd;
}

TArray<int32> UItemContainer::GetEmptySlots(bool first)
{
	TArray<int32> emptySlots;
	TSet<int32> filledSlots;

	for (const FInstanceItemData& item : GetItems()) {
		filledSlots.Add(item.slot);
	}

	for (int32 i = 0; i < GetMaxItemCount(); ++i) {
		if (!filledSlots.Contains(i)) {
			emptySlots.Add(i);

			if (first)
			{
				break;
			}
		}
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
	TArray<int32> slotsLeft = GetEmptySlots(true);

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

bool UItemContainer::SplitItem(FInstanceItemData& itemToSplit)
{
	bool result = false;

	if (HasSpace() && itemToSplit.amount > 1 && GetItemStackSize(itemToSplit.itemID) > 1)
	{
		int32 emptySlot = GetNextEmptySlotForItem(itemToSplit.itemID);

		if (UItemStructs::IsValidID(emptySlot))
		{
			FString existingItemName = GetItemName(itemToSplit.itemID);
			UE_LOG(LogTemp, Log, TEXT("Splitting %d of %s"), itemToSplit.amount, *existingItemName);

			int32 total = itemToSplit.amount;
			int32 halfAmount = FMath::RoundToInt32(total / 2.0f);
			int32 remainingAmount = total - halfAmount;

			// Update original
			FInstanceItemData originalCopy = itemToSplit;
			itemToSplit.amount = halfAmount;
			UpdateItemData(this, itemToSplit, originalCopy);

			int32 newID = GetNextInstanceItemDataID();

			// Create new item
			FInstanceItemData newItem = itemToSplit.CopyItem(newID, GetContainerInstanceID(), emptySlot, remainingAmount);
			UE_LOG(LogTemp, Log, TEXT("SplitItem: New ID=%d, EmptySlot=%d, Existing? %s"),
				newItem.ID, emptySlot,
				GetGame()->GetInstancedItems().Contains(newItem.ID) ? TEXT("YES") : TEXT("NO"));

			UpdateItemData(this, newItem, FInstanceItemData(emptySlot));

			UpdateDebugItemsList();
			result = true;
		}
	}
	return result;
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

int32 UItemContainer::FindNextEmptyValidSlot(EGearType inType)
{
	int32 result = UItemStructs::InvalidInt;

	// Do we have any valid slots defined?
	if (!validSlots.IsEmpty())
	{
		// Get all the currently empty slots
		TArray<int32> emptySlots = GetEmptySlots();

		for (int32 emptySlot : emptySlots)
		{
			// If the slot isn't valid for the item type, then remove it
			if (IsValidForSlot(emptySlot, inType))
			{
				result = emptySlot;
				break;
			}
		}
	}
	// If there are no valid slots defined just get the next empty slot
	else
	{
		result = GetNextEmptySlot();
	}

	return result;
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
		FInstanceItemData originalItemData = item;

		// Check if the IDs match
		if (item.itemID == itemToRemove.itemID)
		{
			// Simply remove the item, if it has less than or equal to the required amount
			if (item.amount <= itemToRemove.amount)
			{
				itemToRemove.amount -= item.amount;
				RemoveInstanceItem(this, item);
			}
			// Otherwise item.amount is greater than the amount to remove
			// So just reduce the value by the amount to remove, and zero the amount to remove
			else
			{
				item.amount -= itemToRemove.amount;
				itemToRemove.amount = 0;

				UpdateItemData(this, item, originalItemData);
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
