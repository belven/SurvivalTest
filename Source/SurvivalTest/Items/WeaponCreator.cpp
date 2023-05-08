#include "WeaponCreator.h"
#include "MeleeWeapon.h"
#include "ProjectileWeapon.h"
#include "RangedWeapon.h"
#include "Weapon.h"
#include "Kismet/GameplayStatics.h"
#include "../BaseGameInstance.h"

UWeapon* UWeaponCreator::CreateWeapon(const int32 itemID, const UWorld* world, int32 instanceItemID)
{
	UWeapon* weaponOut = NULL;
	UBaseGameInstance* gameIn = GameInstance(world);
	const FItemData id = gameIn->GetItemData(itemID);

	if (id.type == EItemType::Weapon) {
		const FWeaponData w = gameIn->GetWeaponData(itemID);

		if (w.type == EWeaponType::Projectile)
		{
			UProjectileWeapon* pw = mNewObject(UProjectileWeapon);
			FRangedWeaponData rwd = gameIn->GetRangedWeaponData(w.ID);
			FInstanceWeaponData iwd = gameIn->GetInstanceWeaponDataByInstanceItemID(instanceItemID);
			FProjectileWeaponData pwd = gameIn->GetProjectileWeaponData(rwd.ID);
						
			if (iwd.ID == UItemStructs::InvalidInt) {
				iwd.ID = gameIn->GetNextInstanceWeaponDataID();
				iwd.instanceItemID = instanceItemID;
				iwd.mode = EFireMode::FullAuto;
				iwd.ammo = pwd.magazineSize;
				gameIn->AddUpdateData(iwd);
			}

			pw->SetItemData(id);
			pw->SetWeaponData(w);
			pw->SetRangedWeaponData(rwd);
			pw->SetProjectileWeaponData(pwd);
			pw->SetInstanceWeaponData(iwd);
			weaponOut = pw;
		}
		else if (w.type == EWeaponType::Melee) {
			UMeleeWeapon* mw = mNewObject(UMeleeWeapon);
			mw->SetItemData(id);
			mw->SetWeaponData(w);
			mw->SetMeleeWeaponData(gameIn->GetMeleeWeaponData(w.ID));
			weaponOut = mw;
		}
	}
	return weaponOut;
}