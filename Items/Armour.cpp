#include "Armour.h"
#include "ItemContainer.h"
#include "SurvivalTest/SurvivalGameInstance.h"
#include "SurvivalTest/Tables/ContainerTableData.h"

UArmour* UArmour::CreateArmour(int32 itemID, USurvivalGameInstance* game)
{
	FArmourData armourData = game->GetArmourDataByItemID(itemID);

	int32 instanceContainerDataID = game->GetNextInstanceContainerDataID();

	FInstanceContainerData icd;
	icd.ID = instanceContainerDataID;
	icd.containerID = armourData.containerID;
	icd.type = EContainerType::Armour;
	game->GetInstancedContainers().Add(icd.ID, icd);

	int32 armourContainerDataID = 0;

	if (game->GetArmourInstances().Num() > 0)
	{
		armourContainerDataID = game->GetArmourInstances().Num() - 1;
	}

	FInstanceArmourData acd;
	acd.ID = armourContainerDataID;
	acd.armourID = armourData.ID;
	acd.containerInstanceID = icd.ID;
	game->GetArmourInstances().Add(acd.ID, acd);

	UArmour* armour = NewObject<UArmour>();
	armour->SetData(armourData);
	armour->SetItemData(game->GetItemData(itemID));
	armour->SetInstanceArmourData(acd);

	FContainerData cd = game->GetContainerData()->GetData().FindOrAdd(armourData.containerID);

	UItemContainer* ic = UItemContainer::CreateItemContainer(cd, icd);
	return armour;
}

UArmour* UArmour::LoadArmour(int32 armourInstanceID, USurvivalGameInstance* game)
{
	UArmour* armour = NewObject<UArmour>();
	FInstanceArmourData acd = game->GetArmourInstances().FindChecked(armourInstanceID);
	FArmourData armourData = game->GetArmourData(acd.armourID);
	armour->SetData(armourData);
	armour->SetItemData(game->GetItemData(armourData.itemID));
	armour->SetInstanceArmourData(acd);

	FContainerData cd = game->GetContainerData()->GetData().FindOrAdd(armourData.containerID);
	FInstanceContainerData icd = game->GetInstancedContainers().FindChecked(acd.containerInstanceID);
	UItemContainer* ic = UItemContainer::CreateItemContainer(cd, icd);
	return armour;
}
