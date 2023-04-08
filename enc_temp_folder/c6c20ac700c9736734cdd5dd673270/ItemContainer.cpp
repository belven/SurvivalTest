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

void UItemContainer::TransferItem(UItemContainer* other, FInstanceItemData data)
{
	//FItemData id = game->GetItemData(data.itemID);

	//if(id.type == EItemType::Armour)
	//{
	//	FInstanceArmourData iad = game->GetInstancedArmourByContainerID(data.containerInstanceID);
	//	//iad.containerInstanceID = GetInstanceContainerData().ID;
	//	game->AddUpdateData(iad);
	//}

	data.containerInstanceID = GetInstanceContainerData().ID;
	game->AddUpdateData(data);

	other->OnItemRemoved.Broadcast(data);
	OnItemAdded.Broadcast(data);
	UpdateDebugItemsList();
}

int32 UItemContainer::GetNextSlotForItem(int32 itemID)
{
	EGearType gearType = GetGame()->GetGearTypeForItem(itemID);
	return  gearType != EGearType::End ? FindNextEmptyValidSlot(gearType) : GetNextEmptySlot();;
}

/** Adds an item to the inventory, if it finds an item with less than StackSize it adds the amount
* else it will create a new item with the remaining amount and set the one found to StackSize
*
*@param ids the list of new InstanceItemData ids created in the database
*
* @return the input item with the amount set to the remainder if any, i.e. if it's not 0 then the inventory was full
*/
FInstanceItemData UItemContainer::AddItem(FInstanceItemData itemToAdd, TArray<int32>& ids)
{
	bool itemAdded = false;

	// Are we adding a whole item, i.e. an item that is at it's max stack size? If so, just add it
	if (HasSpace())
	{
		int32 emptySlot = GetNextSlotForItem(itemToAdd.itemID);

		const int32 stackSize = GetItemStackSize(itemToAdd.itemID);
		if (itemToAdd.amount == stackSize)
		{
			if (emptySlot != UItemStructs::InvalidInt) {
				FInstanceItemData newItem = itemToAdd.CopyItem(emptySlot, GetNextItemID(), instanceContainerData.ID);
				newItem.amount = itemToAdd.amount;
				game->AddUpdateData(newItem);
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
				game->GetInstancedItems().Add(existingItem.ID, existingItem);
				itemAdded = true;
			}

			// Keep adding new items until we're either full or added all items
			while (itemToAdd.amount > 0 && HasSpace())
			{
				// Get the next slot, taking into account invalid slot locations
				emptySlot = GetNextSlotForItem(itemToAdd.itemID);

				if (emptySlot != UItemStructs::InvalidInt) {
					// Make a new item
					FInstanceItemData newItem = itemToAdd.CopyItem(emptySlot, GetNextItemID(), instanceContainerData.ID);
					newItem.amount = 0;
					newItem.TakeFrom(itemToAdd, stackSize);

					// Add the new item
					ids.Add(newItem.ID);
					game->AddUpdateData(newItem);
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

/* Searches through all current items and checks for an available validSlots, if any*/
int32 UItemContainer::GetNextEmptySlot()
{
	TArray<int32> slotsLeft = GetEmptySlots();

	if (!slotsLeft.IsEmpty())
	{
		return slotsLeft[0];
	}

	return UItemStructs::InvalidInt;
}

void UItemContainer::RemoveFilledSlots(TArray<int32>& slots)
{
	for (FInstanceItemData iid : GetItems())
	{
		slots.Remove(iid.slot);
	}
}

bool UItemContainer::IsValidForSlot(int32 slot, EGearType inType)
{
	if (validSlots.Num() > 0)
	{
		FValidSlots* slots = validSlots.Find(inType);
		if (slots && slots->validSlots.Contains(slot))
		{
			return true;
		}
		return false;
	}
	return true;
}

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

		if (emptySlots.Num() > 0)
		{
			return emptySlots[0];
		}

		return UItemStructs::InvalidInt;
	}
	return GetNextEmptySlot();
}

void UItemContainer::UpdateDebugItemsList()
{
	 TArray<FInstanceItemData> data = GetGame()->GetInventoryItems(instanceContainerData.ID);
	 lastUpdatedItems.Empty();

	 for (FInstanceItemData iid : data) {
		 lastUpdatedItems.Add(FItemDataPair(iid, GetGame()->GetItemData(iid.itemID)));
	 }
}

/* This will reduce the an items amount by the given item if found */
bool UItemContainer::RemoveItem(FInstanceItemData itemToRemove)
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

	if (itemsToRemove.Num() > 0)
	{
		for (FInstanceItemData ii : itemsToRemove)
		{
			game->GetInstancedItems().Remove(ii.ID);
		}
		OnItemRemoved.Broadcast(itemToRemove);
	}

	UpdateDebugItemsList();

	if (itemToRemove.amount == 0)
	{
		return true;
	}
	return false;
}

/* Returns the total amount of items for the given id */
int32 UItemContainer::GetItemAmount(int32 id)
{
	int32 total = 0;
	for (FInstanceItemData item : GetItems())
	{
		if (item.itemID == id)
		{
			total += item.amount;
		}
	}
	return total;
}
