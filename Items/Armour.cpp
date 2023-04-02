#include "Armour.h"
#include "ItemContainer.h"
#include "SurvivalTest/SurvivalGameInstance.h"

UArmour* UArmour::CreateArmour(int32 itemID, USurvivalGameInstance* game)
{
	FArmourData armourData = game->GetArmourData(itemID);

	int32 instanceContainerDataID = 0;

	if (game->instancedContainers.Num() > 0)
	{
		instanceContainerDataID = game->instancedContainers.Num() - 1;
	}

	FInstanceContainerData icd;
	icd.ID = instanceContainerDataID;
	icd.containerID = armourData.containerID;
	icd.type = EContainerType::Armour;
	game->instancedContainers.Add(icd.ID, icd);

	int32 armourContainerDataID = 0;

	if (game->armourInstances.Num() > 0)
	{
		armourContainerDataID = game->armourInstances.Num() - 1;
	}

	FArmourInstanceData acd;
	acd.ID = armourContainerDataID;
	acd.armourID = armourData.ID;
	acd.containerInstanceID = icd.ID;
	game->armourInstances.Add(acd.ID, acd);

	UArmour* armour = NewObject<UArmour>();
	armour->SetData(armourData);
	armour->SetItemData(game->GetItemData(itemID));
	armour->SetContainerData(acd);

	FContainerData cd = game->containers.FindChecked(icd.containerID);

	UItemContainer* ic = UItemContainer::CreateItemContainer(cd, icd, game->GetInventoryItems(icd.ID));
	return armour;
}
