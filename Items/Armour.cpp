#include "Armour.h"
#include "ArmourCreator.h"
#include "ItemContainer.h"
#include "SurvivalTest/BaseGameInstance.h"
#include "SurvivalTest/Tables/ContainerTableData.h"

UArmour* UArmour::CreateArmour(int32 itemID, UBaseGameInstance* game, int32 instanceItemDataID)
{
	UArmour* armour = NewObject<UArmour>();
	UArmourCreator::CreateArmourData(itemID, game, armour, instanceItemDataID);
	FContainerData cd = game->GetContainerData()->GetData().FindOrAdd(armour->GetData().containerID);
	armour->SetContainer(UItemContainer::CreateItemContainer(cd, armour->GetInstanceContainerData(), game));

	FInstanceItemData testData;
	testData.itemID = 1;
	testData.amount = 3;

	TArray<int32> ids;
	armour->GetContainer()->AddItem(testData, ids);
	return armour;
}

UArmour* UArmour::LoadArmour(int32 armourInstanceID, UBaseGameInstance* game)
{
	UArmour* armour = NewObject<UArmour>();
	FInstanceArmourData acd = game->GetInstancedArmour().FindChecked(armourInstanceID);

	FArmourData armourData = game->GetArmourData(acd.armourID);
	armour->SetData(armourData);
	armour->SetItemData(game->GetItemData(armourData.itemID));
	armour->SetInstanceArmourData(acd);

	FContainerData cd = game->GetContainerData()->GetData().FindOrAdd(armourData.containerID);
	armour->SetContainer(UItemContainer::CreateItemContainer(cd, armour->GetInstanceContainerData(), game));
	return armour;
}
