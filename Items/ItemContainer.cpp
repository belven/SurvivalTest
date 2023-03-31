#include "ItemContainer.h"

UItemContainer::UItemContainer() : Super() {
	SetOwnerID("0");
}

void UItemContainer::DataTableChanged() {

}

FString UItemContainer::GetItemName(int32 itemID)
{
	return "";
}

int32 UItemContainer::GetNextInventoryID()
{
	return 1;
}

int32 UItemContainer::GetItemStackSize(int32 itemID)
{
	return 1;
}

bool UItemContainer::HasSpace(FInventoryItemData item)
{
	return item.GetRemainingSpace(10) > 1;
}

FInventoryItemData UItemContainer::GetExistingItemWithSpace(FInventoryItemData inItem) {
	for (FInventoryItemData item : items) {
		// Finds the first item with space available and a matching name
		if (GetItemName(item.itemID).Equals(GetItemName(inItem.itemID)) && HasSpace(item)) {
			return item;
		}
	}
	return FInventoryItemData();
}

/* Adds an item to the inventory, if it finds an item with less than StackSize it adds the amount
* else it will create a new item with the remaining amount and set the one found to StackSize
*
* @return the input item with the amount set to the remainder if any, i.e. if it's not 0 then the inventory was full
*/
FInventoryItemData UItemContainer::AddItem(FInventoryItemData itemToAdd) {
	bool itemAdded = false;

	// Are we adding a whole item, i.e. an item that is at it's max stack size? If so, just add it
	if (HasSpace() && itemToAdd.GetRemainingSpace(GetItemStackSize(itemToAdd.itemID)) == 0) {
		FInventoryItemData tempItem = itemToAdd.CopyItem(GetNextEmptySpace(), GetNextInventoryID());
		items.Add(tempItem);
		itemToAdd.amount = 0;
		itemAdded = true;
	}
	else
	{
		FInventoryItemData existingItem = GetExistingItemWithSpace(itemToAdd);

		// Check all existing matching items to see if they have space
		while (itemToAdd.amount > 0 && existingItem.isValid()) {
			existingItem.TakeFrom(itemToAdd);

			// Try to find another item to add to
			existingItem = GetExistingItemWithSpace(itemToAdd);
			itemAdded = true;
		}

		// Keep adding new items until we're either full or added all items
		while (itemToAdd.amount > 0 && HasSpace()) {
			// Make a new item
			FInventoryItemData newItem = itemToAdd.CopyItem(GetNextEmptySpace(), GetNextInventoryID());// TODO get data from database
			newItem.amount = 0;
			newItem.TakeFrom(itemToAdd);

			// Add the new item
			items.Add(newItem);
			itemAdded = true;
		}
	}

	if (itemAdded)
		OnItemAdded.Broadcast(itemToAdd);
	return itemToAdd;
}

/* Searches through all current items and checks for an available slot, if any*/
int32 UItemContainer::GetNextEmptySpace() {
	TArray<int32> slotsLeft;

	for (int32 i = 0; i < GetMaxItemCount(); i++) {
		slotsLeft.Add(i);
	}

	RemoveFilledSlots(slotsLeft);

	if (!slotsLeft.IsEmpty()) {
		return slotsLeft[0];
	}

	return UItemStructs::InvalidInt;
}

void UItemContainer::RemoveFilledSlots(TArray<int32>& slots)
{
	for(FInventoryItemData iid : items)
	{
		slots.Remove(iid.slot);
	}
}

/* This will reduce the an items amount by the given item if found */
bool UItemContainer::RemoveItem(FInventoryItemData itemToRemove) {
	TArray<FInventoryItemData> itemsToRemove;

	for (FInventoryItemData item : items) {
		int32 amountToTake = itemToRemove.amount;

		// Check if the IDs match
		if (item.itemID == itemToRemove.itemID) {

			// Simply remove it if the item has less than the required amount
			if (amountToTake >= item.amount) {
				itemsToRemove.Add(item);
				itemToRemove.amount = amountToTake - item.amount;
			}
			// Just change the amount
			else {
				itemToRemove.amount = 0;
				item.amount -= amountToTake;
			}
		}

		// Stop when we've run out of items
		if (amountToTake <= 0) {
			break;
		}
	}

	if (itemsToRemove.Num() > 0) {
		for (FInventoryItemData ii : itemsToRemove) {
			items.Remove(ii);
		}
	}

	OnItemRemoved.Broadcast(itemToRemove);

	if (itemToRemove.amount == 0) {
		return true;
	}
	else {
		return false;
	}
}

/* Returns the total amount of items for the given id */
int32 UItemContainer::GetItemAmount(int32 id) {
	int32 total = 0;
	for (FInventoryItemData item : items) {
		if (item.itemID == id) {
			total += item.amount;
		}
	}
	return total;
}