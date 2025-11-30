#include "WeaponCreator.h"
#include "MeleeWeapon.h"
#include "ProjectileWeapon.h"
#include "RangedWeapon.h"
#include "Weapon.h"
#include "Kismet/GameplayStatics.h"
#include "../BaseGameInstance.h"

UWeapon* UWeaponCreator::CreateWeapon(UBaseGameInstance* gameIn, const FInstanceItemData& inInstanceItemData, const FItemData& inItemData)
{
	UWeapon* weaponOut = NULL;

	if (inItemData.type == EItemType::Weapon) {
		const FWeaponData w = gameIn->GetWeaponData(inItemData.ID);

		if (w.type == EWeaponType::Projectile)
		{
			UProjectileWeapon* pw = mNewObject(UProjectileWeapon);
			FRangedWeaponData rwd = gameIn->GetRangedWeaponData(w.ID);
			FProjectileWeaponData pwd = gameIn->GetProjectileWeaponData(rwd.ID);
			FInstanceWeaponData iwd = gameIn->GetInstanceWeaponDataByInstanceItemID(inInstanceItemData.ID);
						
			if (iwd.ID == UItemStructs::InvalidInt) {
				iwd = gameIn->CreateNewInstanceWeaponData(inInstanceItemData.ID, pwd);
			}

			pw->SetItemData(inItemData);
			pw->SetInstanceItemData(inInstanceItemData);
			pw->SetWeaponData(w);
			pw->SetRangedWeaponData(rwd);
			pw->SetProjectileWeaponData(pwd);
			pw->SetInstanceWeaponData(iwd);
			weaponOut = pw;
		}
		else if (w.type == EWeaponType::Melee) 
		{
			UMeleeWeapon* mw = mNewObject(UMeleeWeapon);
			mw->SetItemData(inItemData);
			mw->SetInstanceItemData(inInstanceItemData);
			mw->SetWeaponData(w);
			mw->SetMeleeWeaponData(gameIn->GetMeleeWeaponData(w.ID));
			weaponOut = mw;
		}
	}
	return weaponOut;
}
