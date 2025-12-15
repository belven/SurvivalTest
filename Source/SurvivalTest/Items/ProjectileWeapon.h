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
	void SetProjectileWeaponData(const FProjectileWeaponData& data) { projectileWeaponData = data;  }
	virtual void UseWeapon(const FRotator& LookAtRotation) override;
	bool HasAmmo();
	void SetCanFireTimer();
	void ConsumeAmmo();
	void SpawnProjectile(const FRotator& FireRotation);
	void Reload();

	virtual FString GetWeaponHUDText() override;

	bool NeedsReloading() { return GetCurrentAmmo() < GetProjectileWeaponData().magazineSize;  }

	virtual ABaseProjectile* SpawnProjectile(FVector gunLocation, FRotator FireRotation, UClass* projectileClass) override;

	int32 GetCurrentAmmo() const
	{
		return instanceWeaponData.ammo;
	}

	FOutOfAmmo OnOutOfAmmo;

	FTimerHandle TimerHandle_ReloadExpired;
	FTimerHandle TimerHandle_RecoilReset;
	void ReloadExpired();
	void RecoilReset();
protected:
	FProjectileWeaponData projectileWeaponData;
	bool firstShot;
	float maxRecoil = 1.5;
	float defaultRecoil = 0.2;
	float currentRecoil = 0.5;
};