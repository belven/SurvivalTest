#pragma once
#include "CoreMinimal.h"
#include "Item.h"
#include "ItemStructs.h"
#include "Engine/EngineTypes.h"
#include "Weapon.generated.h"

#define mSpawnProjectile(projectileClass) owner->GetWorld()->SpawnActor<ABaseProjectile>(projectileClass, gunLocation, FireRotation)
#define mSetTimerWorld(world, handle, method, delay) world->GetTimerManager().SetTimer(handle, this, method, delay)
#define mClearTimerWorld(world, handle, method, delay) world->GetTimerManager().SetTimer(handle, this, method, delay)

class ABaseCharacter;
class ABaseProjectile;

UCLASS()
class SURVIVALTEST_API UWeapon : public UItem
{
	GENERATED_BODY()

public:
	UWeapon();
	UStaticMesh* GetItemMesh();

	FInstanceWeaponData GetInstanceWeaponData() const { return instanceWeaponData; }
	void SetInstanceWeaponData(FInstanceWeaponData inInstanceWeaponData) { instanceWeaponData = inInstanceWeaponData; }

	FWeaponData GetWeaponData() const { return weaponData; }
	void SetWeaponData(FWeaponData data) { weaponData = data; }
	virtual void UseWeapon(const FVector& LookAtRotation);

	ABaseCharacter* GetCharacterOwner() const { return owner; }
	void SetOwner(ABaseCharacter* val);

	void AttackComplete();

protected:
	FWeaponData weaponData;
	FInstanceWeaponData instanceWeaponData;
	FVector GunOffset;
	bool canAttack;
	FTimerHandle TimerHandle_ShotTimerExpired;

	UPROPERTY()
	ABaseCharacter* owner;

	UPROPERTY()
	UStaticMeshComponent* weaponMeshComp;

	ABaseProjectile* SpawnProjectile(FVector gunLocation, FRotator FireRotation, UClass* projectileClass);
};
