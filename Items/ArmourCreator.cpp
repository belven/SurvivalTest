#include "ArmourCreator.h"
#include "Armour.h"
#include "Kismet/GameplayStatics.h"
#include "SurvivalTest/SurvivalGameInstance.h"

UArmour* UArmourCreator::CreateArmour(int32 itemID, UWorld* world)
{
	UArmour* a = NULL;
	USurvivalGameInstance* gameIn = GameInstance(world);
	const FItemData id = gameIn->GetItemData(itemID);

	const FArmourData ad = gameIn->GetArmourData(itemID);
	a  = UArmour::CreateArmour(ad);
	a->SetItemData(id);
	return a;
}
