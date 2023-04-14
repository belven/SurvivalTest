#include "Weapon.h"
#include "../BaseProjectile.h"
#include "SurvivalTest/BaseCharacter.h"
#include "SurvivalTest/CharacterStructs.h"

UWeapon::UWeapon()
{
	GunOffset = FVector(100.f, 0.f, 0.f);
	canAttack = true;
}

void UWeapon::AttackComplete()
{
	canAttack = true;
}

ABaseProjectile* UWeapon::SpawnProjectile(FVector gunLocation, FRotator FireRotation, UClass* projectileClass) {
	ABaseProjectile* projectile = mSpawnProjectile(projectileClass);
	FHealthChange hc;
	hc.changeAmount = weaponData.healthChange;
	hc.source = GetOwner();
	hc.heals = weaponData.heals;
	projectile->SetHealthChange(hc);
	return projectile;
}

void UWeapon::UseWeapon(const FVector& LookAtRotation)
{

}