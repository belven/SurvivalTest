#include "ProjectileWeapon.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "SurvivalTest/BaseCharacter.h"
#include "SurvivalTest/BasePlayerController.h"
#include "SurvivalTest/BaseProjectile.h"
#include "SurvivalTest/HelperFunctions.h"

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

ABaseProjectile* UProjectileWeapon::SpawnProjectile(FVector gunLocation, FRotator FireRotation, UClass* projectileClass) {
	ABaseProjectile* projectile = mSpawnProjectile(projectileClass);
	FHealthChange hc;
	hc.changeAmount = weaponData.healthChange;
	hc.source = GetCharacterOwner();
	hc.heals = weaponData.heals;
	projectile->SetHealthChange(hc);
	projectile->SetWeaponUsed(this);
	projectile->GetProjectileMovement()->ProjectileGravityScale = GetProjectileWeaponData().gravity;
	return projectile;
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
	// TODO might need to make this a delayed update to the data but instant for the UI.
	// Perhaps queue data updates every second and have a local ammo count instead
	GetCharacterOwner()->GetGame()->AddUpdateData(instanceWeaponData);

	if (instanceWeaponData.ammo <= 0)
	{
		instanceWeaponData.ammo = 0;
		OnOutOfAmmo.Broadcast();
	}
}

void UProjectileWeapon::SpawnProjectile(const FRotator& FireRotation)
{
	FVector startLoc = GetCharacterOwner()->GetActorLocation() + (GetCharacterOwner()->GetActorForwardVector() * 30);
	const FVector gunLocation = startLoc + GunOffset;
	FRotator rot = FireRotation;

	if (!firstShot)
	{
		double angle = 360 * (1 - GetRangedWeaponData().accuracy);
		rot = UKismetMathLibrary::RandomUnitVectorInConeInDegrees(FireRotation.Vector(), angle).Rotation();
	}
	else
	{
		firstShot = false;
	}

	GetCharacterOwner()->AddControllerPitchInput(FMath::RandRange(-currentRecoil, 0.f));
	GetCharacterOwner()->AddControllerYawInput(FMath::RandRange(-currentRecoil, currentRecoil));

	currentRecoil = FMath::Clamp(currentRecoil + 0.1, defaultRecoil, maxRecoil);

	ABaseProjectile* proj = SpawnProjectile(gunLocation, rot, ABaseProjectile::StaticClass());
	FVector velocity = FVector(1.f, 0.f, 0.f).GetSafeNormal() * GetProjectileWeaponData().bulletVelocity;
	proj->GetProjectileMovement()->SetVelocityInLocalSpace(velocity);

	mSetRecoilResetTimer();
}

void UProjectileWeapon::Reload()
{
	canAttack = false;
	RecoilReset();
}

void UProjectileWeapon::ReloadExpired()
{
	canAttack = true;
	OnWeaponReady.Broadcast();
	OnReloadComplete.Broadcast();
}

void UProjectileWeapon::RecoilReset()
{
	firstShot = true;
	currentRecoil = defaultRecoil;
}
