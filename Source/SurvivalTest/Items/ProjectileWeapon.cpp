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
			int32 ammoLeft = GetCharacterOwner()->GetInventory()->GetItemAmount(GetProjectileWeaponData().ammoID);

			if (ammoLeft > 0) {
				canAttack = false;
				mSetTimerWorld(owner->GetWorld(), TimerHandle_ShotTimerExpired, &UProjectileWeapon::ReloadExpired, projectileWeaponData.reloadSpeed);
			}
		}
	}
}

void UProjectileWeapon::ReloadExpired()
{
	int32 ammoLeft = GetCharacterOwner()->GetInventory()->GetItemAmount(GetProjectileWeaponData().ammoID);
	int32 ammoToTake = FMath::Min(ammoLeft, projectileWeaponData.magazineSize);

	if (ammoLeft > 0) {
		FInstanceItemData iid;
		iid.amount = ammoToTake;
		iid.itemID = GetProjectileWeaponData().ammoID;
		GetCharacterOwner()->GetInventory()->RemoveItem(iid);

		canAttack = true;
		instanceWeaponData.ammo = ammoToTake;
	}
}
