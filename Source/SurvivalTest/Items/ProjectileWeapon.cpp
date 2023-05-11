#include "ProjectileWeapon.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "SurvivalTest/BaseCharacter.h"
#include "SurvivalTest/BasePlayerController.h"
#include "SurvivalTest/BaseProjectile.h"

#define mSetReloadTimer() mSetTimerWorld(GetCharacterOwner()->GetWorld(), TimerHandle_ShotTimerExpired, &UProjectileWeapon::ReloadExpired, GetProjectileWeaponData().reloadSpeed)

#define mSetFireTimer() mSetTimerWorld(GetCharacterOwner()->GetWorld(), TimerHandle_ShotTimerExpired, &UWeapon::AttackComplete, GetWeaponData().useRate)

#define mSetRecoilResetTimer() mSetTimerWorld(GetCharacterOwner()->GetWorld(), TimerHandle_RecoilReset, &UProjectileWeapon::RecoilReset, 0.33)

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
	instanceWeaponData.ammo--;
	GetCharacterOwner()->GetGame()->AddUpdateData(instanceWeaponData);

	if (instanceWeaponData.ammo == 0)
	{
		OnOutOfAmmo.Broadcast();
	}
}

void UProjectileWeapon::SpawnProjectile(const FRotator& FireRotation)
{
	FVector startLoc = GetCharacterOwner()->GetActorLocation() + (GetCharacterOwner()->GetActorForwardVector() * 30);
	const FVector gunLocation = startLoc + GunOffset;
	FRotator rot = FireRotation;

	if (!firstShot) {
		double angle = 360 * (1 - GetRangedWeaponData().accuracy);
		rot = UKismetMathLibrary::RandomUnitVectorInConeInDegrees(FireRotation.Vector(), angle).Rotation();
	}
	else
	{
		firstShot = false;
	}

	GetCharacterOwner()->AddControllerPitchInput(rot.Pitch);
	GetCharacterOwner()->AddControllerYawInput(rot.Yaw);
	
	ABaseProjectile* proj = UWeapon::SpawnProjectile(gunLocation, rot, ABaseProjectile::StaticClass());
	FVector velocity = FVector(1.f, 0.f, 0.f).GetSafeNormal() * GetProjectileWeaponData().bulletVelocity;
	proj->GetProjectileMovement()->SetVelocityInLocalSpace(velocity);

	mSetRecoilResetTimer();
}

void UProjectileWeapon::Reload()
{
	canAttack = false;
	RecoilReset();
	mSetReloadTimer();
}

void UProjectileWeapon::ReloadExpired()
{
	int32 ammoLeft = GetCharacterOwner()->GetInventory()->GetItemAmount(GetProjectileWeaponData().ammoID);
	int32 ammoMissing = GetProjectileWeaponData().magazineSize - GetInstanceWeaponData().ammo;
	int32 ammoToTake = FMath::Min(ammoLeft, ammoMissing);

	if (ammoLeft > 0)
	{
		FInstanceItemData iid(GetProjectileWeaponData().ammoID, ammoToTake);
		GetCharacterOwner()->GetInventory()->RemoveItem(iid);

		GetInstanceWeaponData().ammo += ammoToTake;
		GetCharacterOwner()->GetGame()->AddUpdateData(GetInstanceWeaponData());
		canAttack = true;
		OnReloadComplete.Broadcast();
	}
}

void UProjectileWeapon::RecoilReset()
{
	firstShot = true;
}
