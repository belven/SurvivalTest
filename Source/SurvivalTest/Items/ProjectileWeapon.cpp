#include "ProjectileWeapon.h"
#include "SurvivalTest/BaseCharacter.h"
#include "SurvivalTest/BaseProjectile.h"

void UProjectileWeapon::UseWeapon(const FVector& LookAtRotation)
{
	if (canFire)
	{
		if (currentAmmo == 0)
		{
			canFire = false;
			mSetTimerWorld(owner->GetWorld(), TimerHandle_ShotTimerExpired, &UProjectileWeapon::ReloadEExpired, projectileWeaponData.reloadSpeed);
		}
		else
		{
			if (LookAtRotation.SizeSquared() > 0.0f)
			{
				const FRotator FireRotation = LookAtRotation.Rotation();
				const FVector statLoc = owner->GetActorLocation() + FVector(0,0, 100);
				const FVector gunLocation = statLoc + FireRotation.RotateVector(GunOffset);

				ABaseProjectile* proj = SpawnProjectile(gunLocation, FireRotation, ABaseProjectile::StaticClass());

				mSetTimerWorld(owner->GetWorld(), TimerHandle_ShotTimerExpired, &UWeapon::ShotTimerExpired, GetWeaponData().useRate);
				currentAmmo--;
				canFire = false;
			}
		}
	}
}

void UProjectileWeapon::ReloadEExpired()
{
	canFire = true;
	currentAmmo = projectileWeaponData.magazineSize;
}