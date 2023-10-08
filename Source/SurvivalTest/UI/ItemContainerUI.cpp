#include "ItemContainerUI.h"

#include "ItemUI.h"
#include "SurvivalTest/Tables/ContainerTableData.h"

int32 UItemContainerUI::GetColumn()
{
	return index % 5;
}

int32 UItemContainerUI::GetNextRowIndex()
{
	int32 nextRow = GetRow() + 1;
	return (nextRow * 5);
}

void UItemContainerUI::GetGridData(int32& row, int32& column)
{
	row = GetRow();
	column = GetColumn();
}

int32 UItemContainerUI::GetRow()
{
	int32 itemsPerRow = 5;
	int32 rowMod = index % itemsPerRow;
	int32 nearestRow = index - rowMod;
	return nearestRow / itemsPerRow;
}

void UItemContainerUI::SetItemContainer(UItemContainer* inContainer)
{
	container = inContainer;

	if (container != NULL) {
		// Set up add and remove listeners for our new container, so we can update our UI as things are added and removed
		container->OnItemUpdated.AddUniqueDynamic(this, &UItemContainerUI::ItemUpdated);
	}
}

FString UItemContainerUI::GetContainerName()
{
	return GetBaseGameInstance()->GetContainerInstanceName(container->GetInstanceContainerData().ID);
}

UItemContainer* UItemContainerUI::GetItemContainerForArmour(FInstanceItemData data)
{
	FInstanceArmourData iadFound;
	FArmourData ad = GetBaseGameInstance()->GetArmourDataByItemID(data.itemID);
	FContainerData cd = GetBaseGameInstance()->GetContainerDataByID(ad.containerID);

	if (cd.slots > 0)
	{
		TArray<FInstanceArmourData> armour;
		gameInstance->GetInstancedArmour().GenerateValueArray(armour);

		for (FInstanceArmourData iad : armour)
		{
			if (iad.instancedItemDataID == data.ID)
			{
				iadFound = iad;
			}
		}

		FInstanceContainerData icd = gameInstance->GetInstancedContainers().FindChecked(iadFound.containerInstanceID);

		UItemContainer* ic = UItemContainer::CreateItemContainer(cd, icd, gameInstance);
		return ic;
	}
	return nullptr;
}

bool UItemContainerUI::IsArmour(FItemData id)
{
	return id.type == EItemType::Armour;
}

UItemUI* UItemContainerUI::GetItemAtSlot(int32 itemSlot, TArray<UWidget*> widgets)
{
	for(UWidget* widget : widgets)
	{
		if(widget->IsA(UItemUI::StaticClass()))
		{
			UItemUI* itemUI = Cast<UItemUI>(widget);

			if (itemUI->GetInstanceItemData().slot == itemSlot)
				return itemUI;
		}
	}

	return nullptr;
}

//void UItemContainerUI::ItemAdded(FInstanceItemData inItem)
//{
//	AddItem(inItem, GetBaseGameInstance()->GetItemData(inItem.itemID));
//}
//
//void UItemContainerUI::ItemRemoved(FInstanceItemData inItem)
//{
//	RemoveItem(inItem);
//}

void UItemContainerUI::ItemUpdated(FInstanceItemData inItem, FInstanceItemData oldItem)
{
	if(!GetItemContainer()->GetInstanceItemAtSlot(oldItem.slot).isValid())
	{
		RemoveItem(oldItem);
	}

	UpdateItem(inItem, GetBaseGameInstance()->GetItemData(inItem.itemID));
}