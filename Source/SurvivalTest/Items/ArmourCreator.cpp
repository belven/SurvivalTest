#include "ArmourCreator.h"
#include "Armour.h"
#include "ItemContainer.h"
#include "Kismet/GameplayStatics.h"
#include "SurvivalTest/BaseGameInstance.h"

UArmour* UArmourCreator::CreateArmour(int32 itemID, UWorld* world, int32 instanceItemDataID)
{
	UArmour* a = UArmour::CreateArmour(itemID, GameInstance(world), instanceItemDataID);
	return a;
}

void UArmourCreator::CreateArmourData(int32 itemID, UBaseGameInstance* game, UArmour* armour, int32 instanceItemDataID)
{
	FItemData id = game->GetItemData(itemID);
	FArmourData armourData = game->GetArmourDataByItemID(itemID);
	armour->SetItemData(id);
	armour->SetData(armourData);

	FInstanceArmourData iad = game->GetInstanceArmourDataByInstanceItemID(instanceItemDataID);
	FContainerData cd = game->GetContainerDataByID(armourData.containerID);

	// Do we have existing data?
	if (iad.ID == UItemStructs::InvalidInt)
	{

		if (cd.slots > 0) {
			// Create a new instance of container
			// Get the next ID from the table
			int32 instanceContainerDataID = game->GetNextInstanceContainerDataID();

			FInstanceContainerData icd;
			icd.ID = instanceContainerDataID;
			icd.containerID = armourData.containerID;
			icd.type = EContainerType::Armour;
			icd.name = id.name;
			game->GetInstancedContainers().Add(icd.ID, icd);

			// Create a new Armour instance based on the container instance
			int32 armourContainerDataID = game->GetNextInstanceArmourDataID();

			FInstanceArmourData acd;
			acd.ID = armourContainerDataID;
			acd.armourID = armourData.ID;
			acd.containerInstanceID = icd.ID;
			acd.instancedItemDataID = instanceItemDataID;
			game->AddUpdateData(acd);

			armour->SetInstanceContainerData(icd);
			armour->SetInstanceArmourData(acd);
			armour->SetContainer(UItemContainer::CreateItemContainer(cd, armour->GetInstanceContainerData(), game));
		}
	}
	else
	{
		armour->SetInstanceContainerData(game->GetInstancedContainers().FindChecked(iad.containerInstanceID));
		armour->SetInstanceArmourData(iad);
		armour->SetContainer(UItemContainer::CreateItemContainer(cd, armour->GetInstanceContainerData(), game));
	}
}