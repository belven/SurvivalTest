#include "ProjectileWeapon.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "SurvivalTest/BaseCharacter.h"
#include "SurvivalTest/BaseProjectile.h"

#define mSetReloadTimer() mSetTimerWorld(GetCharacterOwner()->GetWorld(), TimerHandle_ShotTimerExpired, &UProjectileWeapon::ReloadExpired, GetProjectileWeaponData().reloadSpeed)

#define mSetFireTimer() mSetTimerWorld(GetCharacterOwner()->GetWorld(), TimerHandle_ShotTimerExpired, &UWeapon::AttackComplete, GetWeaponData().useRate)

void UProjectileWeapon::UseWeapon(const FRotator& LookAtRotation)
{
	if (canAttack && HasAmmo())
	{
		if (LookAtRotation.Vector().SizeSquared() > 0.0f)
		{
			SpawnProjectile(LookAtRotation);
			SetCanFireTimer();
			ConsumeAmmo();
		}
	}
}

bool UProjectileWeapon::HasAmmo()
{
	return GetInstanceWeaponData().ammo > 0;
}

void UProjectileWeapon::SetCanFireTimer()
{
	mSetFireTimer();
	canAttack = false;
}

void UProjectileWeapon::ConsumeAmmo()
{
	GetInstanceWeaponData().ammo--;
	GetCharacterOwner()->GetGame()->AddUpdateData(GetInstanceWeaponData());

	if (GetInstanceWeaponData().ammo == 0)
	{
		OnOutOfAmmo.Broadcast();
	}
}

void UProjectileWeapon::SpawnProjectile(const FRotator& FireRotation)
{
	const FVector startLoc = GetCharacterOwner()->GetActorLocation() + (GetCharacterOwner()->GetActorForwardVector() * 30);
	const FVector gunLocation = startLoc + FireRotation.RotateVector(GunOffset);

	ABaseProjectile* proj = UWeapon::SpawnProjectile(gunLocation, FireRotation, ABaseProjectile::StaticClass());
	FVector velocity = FVector(1.f, 0.f, 0.f).GetSafeNormal() * GetProjectileWeaponData().bulletVelocity;
	proj->GetProjectileMovement()->SetVelocityInLocalSpace(velocity);
}

void UProjectileWeapon::Reload()
{
	canAttack = false;
	mSetReloadTimer();
}

void UProjectileWeapon::ReloadExpired()
{
	int32 ammoLeft = GetCharacterOwner()->GetInventory()->GetItemAmount(GetProjectileWeaponData().ammoID);
	int32 ammoMissing = GetProjectileWeaponData().magazineSize - GetInstanceWeaponData().ammo;
	int32 ammoToTake = FMath::Min(ammoLeft, ammoMissing);

	if (ammoLeft > 0)
	{
		FInstanceItemData iid;
		iid.amount = ammoToTake;
		iid.itemID = GetProjectileWeaponData().ammoID;
		GetCharacterOwner()->GetInventory()->RemoveItem(iid);

		GetInstanceWeaponData().ammo += ammoToTake;
		GetCharacterOwner()->GetGame()->AddUpdateData(GetInstanceWeaponData());
		canAttack = true;
		OnReloadComplete.Broadcast();
	}
}