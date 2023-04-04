#include "ItemContainerUI.h"

#include "SurvivalTest/Tables/ContainerTableData.h"

int32 UItemContainerUI::GetColumn()
{
	return ((index + 1) % 5) + 1;
}

int32 UItemContainerUI::GetNextRowIndex()
{
	return GetRow() * 5;
}

void UItemContainerUI::GetGridData(int32& row, int32& column)
{
	row = GetRow();
	column = GetColumn();
}

int32 UItemContainerUI::GetRow()
{
	float row = 0;
	int32 nonZeroIndex = index + 1;
	float rowMod = nonZeroIndex % 5;
	float indexRemaining = (nonZeroIndex - rowMod) / 5.0f;

	if (rowMod == 0)
	{
		return FMath::RoundToZero(indexRemaining);
	}

	return FMath::RoundToZero(indexRemaining) + 1;
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
