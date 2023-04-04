#include "ItemContainerUI.h"

#include "SurvivalTest/Tables/ContainerTableData.h"

FString UItemContainerUI::GetContainerName()
{
	return GetBaseGameInstance()->GetInstancedContainers().FindChecked(container->GetInstanceContainerData().ID).name;
}

UItemContainer* UItemContainerUI::GetItemContainerForArmour(FInstanceItemData data)
{
	FInstanceArmourData iadFound;
	TArray<FInstanceArmourData> armour;
	gameInstance->GetInstancedArmour().GenerateValueArray(armour);

	for(FInstanceArmourData iad : armour)
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
