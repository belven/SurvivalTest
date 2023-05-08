#pragma once
#include "CoreMinimal.h"
#include "RangedWeapon.h"
#include "ProjectileWeapon.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOutOfAmmo);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FReloadComplete);

UCLASS()
class SURVIVALTEST_API UProjectileWeapon : public URangedWeapon
{
	GENERATED_BODY()
public:
	FProjectileWeaponData GetProjectileWeaponData() const { return projectileWeaponData; }
	void SetProjectileWeaponData(FProjectileWeaponData data) { projectileWeaponData = data; currentAmmo = data.magazineSize;  }
	virtual void UseWeapon(const FRotator& LookAtRotation) override;
	bool HasAmmo();
	void SetCanFireTimer();
	void ConsumeAmmo();
	void SpawnProjectile(const FRotator& FireRotation);
	void Reload();

	FOutOfAmmo OnOutOfAmmo;
	FReloadComplete OnReloadComplete;

	FTimerHandle TimerHandle_ReloadExpired;
	FTimerHandle TimerHandle_RecoilReset;
	void ReloadExpired();
	void RecoilReset();
protected:
	FProjectileWeaponData projectileWeaponData;
	int32 currentAmmo;
	bool firstShot;
};