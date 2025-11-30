#include "ArmourCreator.h"
#include "Armour.h"
#include "ItemContainer.h"
#include "Kismet/GameplayStatics.h"
#include "SurvivalTest/BaseGameInstance.h"


/**
 * Create a new instance of equipped armour. Requires an instance of FInstanceItemData to exist, so we can create a container linked to this piece of armour
 * If there is no existing FInstanceArmour, it will create one, otherwise find one
 *
 * @param game The game instance, as we're a static method and can't do GetWorld()
 * @param instanceItemDataID The existing ID of FInstanceItemData, store in the game instance
 *
 * @return A new instance of UArmour, with all the data setup for it
 */
UArmour* UArmourCreator::GetOrCreateArmour(UBaseGameInstance* game, const FInstanceItemData& instanceItemData)
{
	UArmour* armour = NewObject<UArmour>();
	FItemData itemData = game->GetItemData(instanceItemData.itemID);
	FArmourData armourData;
	FInstanceContainerData instanceContainerData;

	FInstanceArmourData instanceArmourData = game->GetTableManager()->GetInstanceArmourDataByInstanceItemID(instanceItemData.ID);

	if (instanceArmourData.ID == UItemStructs::InvalidInt)
	{
		CreateArmourData(game, instanceItemData, itemData, armourData, instanceContainerData, instanceArmourData);
	}
	else
	{
		instanceContainerData = game->GetTableManager()->GetInstanceContainerData(instanceArmourData.containerInstanceID);
		armourData = game->GetTableManager()->GetArmourData(instanceArmourData.armourID);
	}

	FContainerData cd = game->GetContainerDataByID(armourData.containerID);

	armour->SetItemData(itemData);
	armour->SetData(armourData);
	armour->SetInstanceContainerData(instanceContainerData);
	armour->SetInstanceArmourData(instanceArmourData);
	armour->SetContainer(UItemContainer::CreateItemContainer(cd, instanceContainerData, game));
	return armour;
}

void UArmourCreator::CreateArmourData(UBaseGameInstance* game, const FInstanceItemData& instanceItemData, const FItemData& itemData)
{
	FArmourData armourData;
	FInstanceContainerData instanceContainerData;
	FInstanceArmourData instanceArmourData;
	return CreateArmourData(game, instanceItemData, itemData, armourData, instanceContainerData, instanceArmourData);
}

void UArmourCreator::CreateArmourData(UBaseGameInstance* game, const FInstanceItemData& instanceItemData, const FItemData& itemData, FArmourData& armourData, FInstanceContainerData& inInstanceContainerData, FInstanceArmourData& inInstanceArmourData)
{
	armourData = game->GetArmourDataByItemID(instanceItemData.itemID);

	// Create a new instance of container
	inInstanceContainerData.ID = game->GetNextInstanceContainerDataID();
	inInstanceContainerData.containerID = armourData.containerID;
	inInstanceContainerData.type = EContainerType::Armour;
	inInstanceContainerData.name = itemData.name;
	game->GetInstancedContainers().Add(inInstanceContainerData.ID, inInstanceContainerData);

	// Create a new Armour instance based on the container instance

	inInstanceArmourData.ID = game->GetNextInstanceArmourDataID();
	inInstanceArmourData.armourID = armourData.ID;
	inInstanceArmourData.containerInstanceID = inInstanceContainerData.ID;
	inInstanceArmourData.instancedItemDataID = instanceItemData.ID;
	game->AddUpdateData(inInstanceArmourData);
}