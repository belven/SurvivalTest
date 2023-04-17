#include "Armour.h"
#include "ArmourCreator.h"
#include "ItemContainer.h"
#include "SurvivalTest/BaseGameInstance.h"
#include "SurvivalTest/Tables/ContainerTableData.h"
#include "SurvivalTest/Tables/TableManager.h"

/**
 * Create a new instance of equipped armour. Requires an instance of FInstanceItemData to exist, so we can create a container linked to this piece of armour
 * If there is no existing FInstanceArmour, it will create one, otherwise find one
 *
 * @param itemID the ID of the item data that represents the armour
 * @param game The game instance, as we're a static method and can't do GetWorld()
 * @param instanceItemDataID The existing ID of FInstanceItemData, store in the game instance
 *
 * @return A new instance of UArmour, with all the data setup for it
 */
UArmour* UArmour::CreateArmour(int32 itemID, UBaseGameInstance* game, int32 instanceItemDataID)
{
	UArmour* armour = NewObject<UArmour>();
	UArmourCreator::CreateArmourData(itemID, game, armour, instanceItemDataID);
	FContainerData cd = game->GetTableManager()->GetContainerData()->GetData().FindOrAdd(armour->GetData().containerID);
	
	//if (cd.slots > 0) {
	//	bool spawnedArmour = false;

	//	for (int i = 0; i < 4; i++) // TODO remove test data!
	//	{
	//		TArray<int32> ids;
	//		FInstanceItemData testData;
	//		FItemData id = UItemStructs::GetRandomItemData(game);
	//		testData.itemID = id.ID;
	//		testData.amount = FMath::RandRange(1, id.maxStack);

	//		if (id.type != EItemType::Armour)
	//		{
	//			armour->GetContainer()->AddItem(testData, ids);
	//		}
	//		else
	//		{
	//			i--;
	//		}
	//	}
	//	//TArray<int32> ids;
	//	//FInstanceItemData testData;
	//	//testData.itemID = 1;
	//	//testData.amount = 10;
	//	//testData = armour->GetContainer()->AddItem(testData, ids);
	//}
	return armour;
}