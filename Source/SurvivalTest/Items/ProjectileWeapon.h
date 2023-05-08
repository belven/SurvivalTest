#pragma once
#include "CoreMinimal.h"
#include "RangedWeapon.h"
#include "ProjectileWeapon.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOutOfAmmo);

UCLASS()
class SURVIVALTEST_API UProjectileWeapon : public URangedWeapon
{
	GENERATED_BODY()
public:
	FProjectileWeaponData GetProjectileWeaponData() const { return projectileWeaponData; }
	void SetProjectileWeaponData(FProjectileWeaponData data) { projectileWeaponData = data; currentAmmo = data.magazineSize;  }
	virtual void UseWeapon(const FRotator& LookAtRotation) override;
	void Reload();

	FOutOfAmmo OnOutOfAmmo;

	FTimerHandle TimerHandle_ReloadExpired;
	void ReloadExpired();
protected:
	FProjectileWeaponData projectileWeaponData;
	int32 currentAmmo;
	
};