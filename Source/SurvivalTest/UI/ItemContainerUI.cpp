#include "ItemContainerUI.h"

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
		container->OnItemRemoved.AddUniqueDynamic(this, &UItemContainerUI::ItemRemoved);
		container->OnItemAdded.AddUniqueDynamic(this, &UItemContainerUI::ItemAdded);
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

void UItemContainerUI::ItemAdded(FInstanceItemData inItem)
{
	GenerateInventory();
}

void UItemContainerUI::ItemRemoved(FInstanceItemData inItem)
{
	GenerateInventory();
}

void UItemContainerUI::ItemUpdated(FInstanceItemData inItem)
{
	GenerateInventory();
}