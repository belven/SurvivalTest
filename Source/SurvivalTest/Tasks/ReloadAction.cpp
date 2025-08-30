#include "ReloadAction.h"

#include "SurvivalTest/Items/ProjectileWeapon.h"

void UReloadAction::StartAction()
{
	UE_LOG(LogTemp, Log, TEXT("Reload Action Started"));
	Super::StartAction();

	UWeapon* equippedWeapon = character->GetEquippedWeapon();
	UProjectileWeapon* weapon = Cast<UProjectileWeapon>(equippedWeapon);

	int32 ammoToTake = weapon->GetProjectileWeaponData().magazineSize - weapon->GetCurrentAmmo();
	//int32 ammoToTake = FMath::Min(weapon->GetCurrentAmmo(), ammoMissing);

	FInstanceItemData iid(weapon->GetProjectileWeaponData().ammoID, ammoToTake);
	character->GetInventory()->RemoveItem(iid);

	weapon->GetInstanceWeaponData().ammo += ammoToTake;
	character->GetGame()->AddUpdateData(weapon->GetInstanceWeaponData());

	weapon->ReloadExpired();

	ActionComplete(FStatusData(true));
}

UReloadAction* UReloadAction::CreateReloadAction(ABaseCharacter* character)
{
	UReloadAction* reloadAction = NewObject<UReloadAction>();
	reloadAction->character = character;
	reloadAction->canBeInterrupted = false;
	return reloadAction;
}
