#include "ArmourCreator.h"
#include "Armour.h"
#include "ItemContainer.h"
#include "Kismet/GameplayStatics.h"
#include "SurvivalTest/BaseGameInstance.h"
#include "SurvivalTest/Tables/ContainerTableData.h"

UArmour* UArmourCreator::CreateArmour(int32 itemID, UWorld* world, int32 instanceItemDataID)
{
	UArmour* a = NULL;
	UBaseGameInstance* gameIn = GameInstance(world);
	const FItemData id = gameIn->GetItemData(itemID);
	
	a  = UArmour::CreateArmour(itemID, gameIn, instanceItemDataID);
	a->SetItemData(id);
	return a;
}

void UArmourCreator::CreateArmourData(int32 itemID, UBaseGameInstance* game, UArmour* armour, int32 instanceItemDataID)
{
	// Get Armour Data by ItemID
	FArmourData armourData = game->GetArmourDataByItemID(itemID);

	// Create a new instance of container
	// Get the next ID from the table
	int32 instanceContainerDataID = game->GetNextInstanceContainerDataID();

	FInstanceContainerData icd;
	icd.ID = instanceContainerDataID;
	icd.containerID = armourData.containerID;
	icd.type = EContainerType::Armour;
	game->GetInstancedContainers().Add(icd.ID, icd);

	// Create a new Armour instance based on the container instance
	int32 armourContainerDataID = game->GetNextInstanceArmourDataID();

	FInstanceArmourData acd;
	acd.ID = armourContainerDataID;
	acd.armourID = armourData.ID;
	acd.containerInstanceID = icd.ID;
	acd.instancedItemDataID = instanceItemDataID;
	game->GetInstancedArmour().Add(acd.ID, acd);

	armour->SetData(armourData);
	armour->SetItemData(game->GetItemData(itemID));
	armour->SetInstanceItemData(icd);
	armour->SetInstanceArmourData(acd);
}