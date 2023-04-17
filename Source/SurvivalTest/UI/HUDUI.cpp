#include "HUDUI.h"
#include "SurvivalTest/BaseCharacter.h"
#include "SurvivalTest/Items/ProjectileWeapon.h"
#include "SurvivalTest/Items/Weapon.h"

float UHUDUI::GetHealthProgress()
{
	return (GetPlayer()->GetCurrentStats().health / GetPlayer()->GetMaxStats().health) ;
}

float UHUDUI::GetWaterProgress()
{
	return (GetPlayer()->GetCurrentStats().water / GetPlayer()->GetMaxStats().water);
}

float UHUDUI::GetFoodProgress()
{
	return (GetPlayer()->GetCurrentStats().hunger / GetPlayer()->GetMaxStats().hunger);
}

int32 UHUDUI::GetWeaponMaxAmmo()
{
	int32 maxAmmo = -1;
	UWeapon* equippedWeapon = GetPlayer()->GetEquippedWeapon();
	if (equippedWeapon && equippedWeapon->GetWeaponData().type == EWeaponType::Projectile)
	{
		maxAmmo = Cast<UProjectileWeapon>(equippedWeapon)->GetProjectileWeaponData().magazineSize;
	}


	return maxAmmo;
}

int32 UHUDUI::GetWeaponCurrentAmmo()
{
	return GetPlayer()->GetEquippedWeapon() ? GetPlayer()->GetEquippedWeapon()->GetInstanceWeaponData().ammo : -1;
}

float UHUDUI::GetRestProgress()
{
	return (GetPlayer()->GetCurrentStats().rest / GetPlayer()->GetMaxStats().rest);
}
