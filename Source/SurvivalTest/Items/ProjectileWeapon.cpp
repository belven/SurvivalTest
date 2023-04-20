#include "ProjectileWeapon.h"

#include "GameFramework/ProjectileMovementComponent.h"
#include "SurvivalTest/BaseCharacter.h"
#include "SurvivalTest/BaseProjectile.h"

void UProjectileWeapon::UseWeapon(const FRotator& LookAtRotation)
{
	if (canAttack)
	{
		if (instanceWeaponData.ammo > 0)
		{
			if (LookAtRotation.Vector().SizeSquared() > 0.0f)
			{
				const FRotator FireRotation = LookAtRotation;
				const FVector statLoc = owner->GetActorLocation() + (owner->GetActorForwardVector() * 30);
				const FVector gunLocation = statLoc + FireRotation.RotateVector(GunOffset);

				ABaseProjectile* proj = SpawnProjectile(gunLocation, FireRotation, ABaseProjectile::StaticClass());
				proj->GetProjectileMovement()->SetVelocityInLocalSpace(FVector(1.f, 0.f, 0.f) * GetProjectileWeaponData().bulletVelocity);

				mSetTimerWorld(owner->GetWorld(), TimerHandle_ShotTimerExpired, &UWeapon::AttackComplete, GetWeaponData().useRate);
				instanceWeaponData.ammo--;
				owner->GetGame()->AddUpdateData(instanceWeaponData);
				canAttack = false;
			}
		}

		if (instanceWeaponData.ammo == 0)
		{
			canAttack = false;
			mSetTimerWorld(owner->GetWorld(), TimerHandle_ShotTimerExpired, &UProjectileWeapon::ReloadExpired, projectileWeaponData.reloadSpeed);
		}
	}
}

void UProjectileWeapon::ReloadExpired()
{
	canAttack = true;
	instanceWeaponData.ammo = projectileWeaponData.magazineSize;
}
