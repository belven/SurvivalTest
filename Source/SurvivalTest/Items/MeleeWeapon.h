#pragma once
#include "CoreMinimal.h"
#include "Weapon.h"
#include "MeleeWeapon.generated.h"

UCLASS()
class SURVIVALTEST_API UMeleeWeapon : public UWeapon
{
	GENERATED_BODY()
public:
	virtual void UseWeapon(const FVector& LookAtRotation) override;
	void AttackComplete();
	FMeleeWeaponData GetMeleeWeaponData() const { return meleeWeaponData; }
	void SetMeleeWeaponData(FMeleeWeaponData data) { meleeWeaponData = data; }
	virtual UWorld* GetWorld() const override;

protected:
	FMeleeWeaponData meleeWeaponData;
	
};