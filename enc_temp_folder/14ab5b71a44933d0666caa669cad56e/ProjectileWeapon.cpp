#include "ProjectileWeapon.h"
#include "SurvivalTest/BaseCharacter.h"
#include "SurvivalTest/BaseProjectile.h"

void UProjectileWeapon::UseWeapon(const FVector& LookAtRotation)
{
	if (canAttack)
	{
		if (instanceWeaponData.ammo == 0)
		{
			canAttack = false;
			mSetTimerWorld(owner->GetWorld(), TimerHandle_ShotTimerExpired, &UProjectileWeapon::ReloadExpired, projectileWeaponData.reloadSpeed);
		}
		else
		{
			if (LookAtRotation.SizeSquared() > 0.0f)
			{
				const FRotator FireRotation = LookAtRotation.Rotation();
				const FVector statLoc = owner->GetActorLocation() + FVector(0,0, 70);
				const FVector gunLocation = statLoc + FireRotation.RotateVector(GunOffset);

				ABaseProjectile* proj = SpawnProjectile(gunLocation, FireRotation, ABaseProjectile::StaticClass());

				mSetTimerWorld(owner->GetWorld(), TimerHandle_ShotTimerExpired, &UWeapon::AttackComplete, GetWeaponData().useRate);
				instanceWeaponData.ammo--;
				owner->GetGame()->AddUpdateData(instanceWeaponData);
				canAttack = false;
			}
		}
	}
}

void UProjectileWeapon::ReloadExpired()
{
	canAttack = true;
	instanceWeaponData.ammo = projectileWeaponData.magazineSize;
}