#include "Weapon.h"
#include "SurvivalTest/BaseCharacter.h"

UWeapon::UWeapon(): characterOwner(nullptr), weaponMeshComp(nullptr)
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

void UWeapon::UseWeapon(const FRotator& LookAtRotation)
{

}