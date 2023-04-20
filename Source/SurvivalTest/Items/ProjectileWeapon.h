#pragma once
#include "CoreMinimal.h"
#include "RangedWeapon.h"
#include "ProjectileWeapon.generated.h"

UCLASS()
class SURVIVALTEST_API UProjectileWeapon : public URangedWeapon
{
	GENERATED_BODY()
public:
	FProjectileWeaponData GetProjectileWeaponData() const { return projectileWeaponData; }
	void SetProjectileWeaponData(FProjectileWeaponData data) { projectileWeaponData = data; currentAmmo = data.magazineSize;  }
	virtual void UseWeapon(const FRotator& LookAtRotation) override;

	FTimerHandle TimerHandle_ReloadExpired;
	void ReloadExpired();
protected:
	FProjectileWeaponData projectileWeaponData;
	int32 currentAmmo;
	
};