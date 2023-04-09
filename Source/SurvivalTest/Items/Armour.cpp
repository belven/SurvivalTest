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

	if (cd.slots > 0) {

		armour->SetContainer(UItemContainer::CreateItemContainer(cd, armour->GetInstanceContainerData(), game));

		bool spawnedArmour = false;

		for (int i = 0; i < 4; i++) // TODO remove test data!
		{
			TArray<int32> ids;
			FInstanceItemData testData;
			FItemData id = UItemStructs::GetRandomItemData(game);
			testData.itemID = id.ID;
			testData.amount = FMath::RandRange(1, id.maxStack);

			if (id.type != EItemType::Armour)
			{
				armour->GetContainer()->AddItem(testData, ids);
			}
			else
			{
				i--;
			}
		}
		//TArray<int32> ids;
		//FInstanceItemData testData;
		//testData.itemID = 1;
		//testData.amount = 10;
		//testData = armour->GetContainer()->AddItem(testData, ids);
	}
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
