#include "SwapEquipmentAction.h"

#include "SurvivalTest/HelperFunctions.h"
#include "SurvivalTest/Items/Weapon.h"
#include "SurvivalTest/Items/WeaponCreator.h"

USwapEquipmentAction* USwapEquipmentAction::CreateSwapEquipmentAction(ABaseCharacter* character, int32 slot)
{
	USwapEquipmentAction* sea = NewObject<USwapEquipmentAction>();
	sea->character = character;
	sea->slot = slot;
	sea->canBeInterrupted = false;
	//TODO Disable player inventory actions here
	return sea;
}

void USwapEquipmentAction::StartAction()
{
	FInstanceItemData iid = character->GetInventory()->GetInstanceItemAtSlot(slot);

	UWeapon* equippedWeapon = character->GetEquippedWeapon();

	bool idIsValid = iid.isValid();
	bool ItemsAreDifferent = !equippedWeapon || equippedWeapon->GetInstanceWeaponData().instanceItemID != iid.ID;

	if (idIsValid && ItemsAreDifferent)
	{
		FString name = equippedWeapon ? equippedWeapon->GetItemData().name : "Empty";
		UBaseGameInstance* gameInstance = Cast<UBaseGameInstance>(character->GetGameInstance());
		UWeapon* weapon = UWeaponCreator::CreateWeapon(gameInstance, iid, gameInstance->GetItemData(iid.itemID));

		UE_LOG(LogTemp, Log, TEXT("Swapping from %s to %s"), *name, *weapon->GetItemData().name);
		character->GetInventory()->SetEquippedWeapon(weapon);
		ActionComplete(FStatusData(true));
	}
	else 
	{
		UE_LOG(LogTemp, Log, TEXT("Swapping failed"));
		ActionComplete(FStatusData(false));
	}
}
