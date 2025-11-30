#include "Weapon.h"
#include "SurvivalTest/BaseCharacter.h"

UWeapon::UWeapon() : characterOwner(nullptr), weaponMeshComp(nullptr)
{
	GunOffset = FVector(0.f, 0.f, 0.f);
	canAttack = true;
}

UStaticMesh* UWeapon::GetItemMesh()
{
	UStaticMesh* meshFound = NULL;

	if (GetItemData().mesh.Equals(""))
	{
		UE_LOG(LogTemp, Warning, TEXT("No mesh string set in item for %s"), *GetItemData().name);
	}
	else 
	{
		meshFound = LoadObject<UStaticMesh>(this, *GetItemData().mesh);
	}

	if (!meshFound) 
	{
		UE_LOG(LogTemp, Warning, TEXT("No static mesh object found for %s"), *GetItemData().name);
	}

	return meshFound;
}

void UWeapon::SetOwner(ABaseCharacter* val)
{
	characterOwner = val;
	characterOwner->GetWeaponMeshComp()->SetStaticMesh(GetItemMesh());
}


void UWeapon::Equip(ABaseCharacter* val)
{
	SetOwner(val);
	characterOwner->GetWeaponMeshComp()->SetStaticMesh(GetItemMesh());
}

void UWeapon::Unequip()
{
	characterOwner->GetWeaponMeshComp()->SetStaticMesh(NULL);
	// TODO delete self
}

void UWeapon::AttackComplete()
{
	canAttack = true;
	OnWeaponReady.Broadcast();
}

void UWeapon::UseWeapon(const FRotator& LookAtRotation)
{

}