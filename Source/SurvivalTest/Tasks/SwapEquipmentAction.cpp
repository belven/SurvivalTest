#include "SwapEquipmentAction.h"
#include "SurvivalTest/Items/Weapon.h"
#include "SurvivalTest/Items/WeaponCreator.h"

USwapEquipmentAction* USwapEquipmentAction::CreateSwapEquipmentAction(ABaseCharacter* character, int32 slot)
{
	USwapEquipmentAction* sea = NewObject<USwapEquipmentAction>();
	sea->character = character;
	sea->slot = slot;
	sea->canBeInterrupted = false;
	sea->SetActionName("Swapping Weapon");
	//TODO Disable player inventory actions here
	return sea;
}

void USwapEquipmentAction::StartAction()
{
	FInstanceItemData iid = character->GetInventory()->GetInstanceItemAtSlot(slot);

	bool idIsValid = iid.isValid();

	if (idIsValid)
	{
		UBaseGameInstance* gameInstance = Cast<UBaseGameInstance>(character->GetGameInstance());
		UWeapon* weapon = UWeaponCreator::CreateWeapon(gameInstance, iid, gameInstance->GetItemData(iid.itemID));

		character->GetInventory()->SetEquippedWeapon(weapon);
		ActionComplete(FStatusData(true));
	}
	else 
	{
		character->GetInventory()->SetEquippedWeapon(NULL);
		ActionComplete(FStatusData(true));
	}
}
