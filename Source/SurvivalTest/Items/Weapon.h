#pragma once
#include "CoreMinimal.h"
#include "Item.h"
#include "ItemStructs.h"
#include "Engine/EngineTypes.h"
#include "SurvivalTest/HelperFunctions.h"
#include "Weapon.generated.h"

#define mSpawnProjectile(projectileClass) GetCharacterOwner()->GetWorld()->SpawnActor<ABaseProjectile>(projectileClass, gunLocation, FireRotation)


DECLARE_DYNAMIC_MULTICAST_DELEGATE(FWeaponReady);

class ABaseCharacter;
class ABaseProjectile;

UCLASS()
class SURVIVALTEST_API UWeapon : public UItem
{
	GENERATED_BODY()

public:
	UWeapon();
	UStaticMesh* GetItemMesh();

	FWeaponReady OnWeaponReady;

	FInstanceWeaponData& GetInstanceWeaponData() { return instanceWeaponData; }
	void SetInstanceWeaponData(FInstanceWeaponData inInstanceWeaponData) { instanceWeaponData = inInstanceWeaponData; }

	FWeaponData& GetWeaponData() { return weaponData; }
	void SetWeaponData(FWeaponData data) { weaponData = data; }
	virtual void UseWeapon(const FRotator& LookAtRotation);

	ABaseCharacter* GetCharacterOwner() const { return characterOwner; }
	void SetOwner(ABaseCharacter* val);

	void AttackComplete();

protected:
	FWeaponData weaponData;
	FInstanceWeaponData instanceWeaponData;
	FVector GunOffset;
	bool canAttack;
	FTimerHandle TimerHandle_ShotTimerExpired;

	UPROPERTY()
	ABaseCharacter* characterOwner;

	UPROPERTY()
	UStaticMeshComponent* weaponMeshComp;

	ABaseProjectile* SpawnProjectile(FVector gunLocation, FRotator FireRotation, UClass* projectileClass);
};
