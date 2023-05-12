#include "Weapon.h"
#include "../BaseProjectile.h"
#include "SurvivalTest/BaseCharacter.h"
#include "SurvivalTest/CharacterStructs.h"

UWeapon::UWeapon()
{
	GunOffset = FVector(0.f, 0.f, 0.f);
	canAttack = true;
}

UStaticMesh* UWeapon::GetItemMesh()
{
	if (GetItemData().mesh.Equals(""))
		return nullptr;

	return LoadObject<UStaticMesh>(this, *GetItemData().mesh);
}

void UWeapon::SetOwner(ABaseCharacter* val)
{
	characterOwner = val;
	characterOwner->GetWeaponMeshComp()->SetStaticMesh(GetItemMesh());
}

void UWeapon::AttackComplete()
{
	canAttack = true;
	OnWeaponReady.Broadcast();
}

ABaseProjectile* UWeapon::SpawnProjectile(FVector gunLocation, FRotator FireRotation, UClass* projectileClass) {
	ABaseProjectile* projectile = mSpawnProjectile(projectileClass);
	FHealthChange hc;
	hc.changeAmount = weaponData.healthChange;
	hc.source = GetCharacterOwner();
	hc.heals = weaponData.heals;
	projectile->SetHealthChange(hc);
	projectile->SetWeaponUsed(this);
	return projectile;
}

void UWeapon::UseWeapon(const FRotator& LookAtRotation)
{

}