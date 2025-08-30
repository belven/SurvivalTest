#include "SwapEquipmentAction.h"

USwapEquipmentAction* USwapEquipmentAction::CreateSwapEquipmentAction(ABaseCharacter* character)
{
	return nullptr;
}

void USwapEquipmentAction::StartAction()
{
	//FInstanceItemData iid = GetBaseCharacter()->GetInventory()->GetInstanceItemAtSlot(slot);

	//UWeapon* equippedWeapon = mCurrentWeapon();

	//if (iid.isValid() && (!equippedWeapon || equippedWeapon->GetInstanceWeaponData().instanceItemID != iid.ID))
	//{
	//	UWeapon* weapon = UWeaponCreator::CreateWeapon(iid.itemID, GetWorld(), iid.ID);
	//	GetBaseCharacter()->GetInventory()->SetEquippedWeapon(weapon);
	//}
}
