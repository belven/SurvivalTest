#include "ArmourCreator.h"
#include "Armour.h"
#include "Kismet/GameplayStatics.h"
#include "SurvivalTest/SurvivalGameInstance.h"

UArmour* UArmourCreator::CreateArmour(int32 itemID, UWorld* world)
{
	UArmour* a = NULL;
	USurvivalGameInstance* gameIn = GameInstance(world);
	const FItemData id = gameIn->GetItemData(itemID);
	
	a  = UArmour::CreateArmour(itemID, gameIn);
	a->SetItemData(id);
	return a;
}
