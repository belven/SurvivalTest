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
	return   nearestRow / itemsPerRow;
}

FString UItemContainerUI::GetContainerName()
{
	return GetBaseGameInstance()->GetInstancedContainers().FindChecked(container->GetInstanceContainerData().ID).name;
}

UItemContainer* UItemContainerUI::GetItemContainerForArmour(FInstanceItemData data)
{
	FInstanceArmourData iadFound;
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
	FContainerData cd = gameInstance->GetContainerData()->GetData().FindOrAdd(icd.containerID);

	UItemContainer* ic = UItemContainer::CreateItemContainer(cd, icd, gameInstance);
	return ic;
}
