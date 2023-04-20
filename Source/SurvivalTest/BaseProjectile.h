// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "CharacterStructs.h"
#include "GameFramework/Actor.h"
#include "BaseProjectile.generated.h"

class USphereComponent;
class UProjectileMovementComponent;
class UWeapon;

UCLASS(config=Game)
class ABaseProjectile : public AActor
{
	GENERATED_BODY()

public:
	ABaseProjectile();
	float CalculateDamageFalloff();
	virtual void Tick(float DeltaSeconds) override;
	
	static const float Default_Initial_Speed;
	static const float Default_Initial_Lifespan;

	void SetHealthChange(FHealthChange inHealthChange);

	UWeapon* GetWeaponUsed() const	{		return weaponUsed;	}
	void SetWeaponUsed(UWeapon* inWeaponUsed)	{		weaponUsed = inWeaponUsed;	}
	UProjectileMovementComponent* GetProjectileMovement() { return ProjectileMovement; }
protected:
	FHealthChange healthChange;
	FVector startLoc = FVector::ZeroVector;
	
protected:
	UPROPERTY()
	UWeapon* weaponUsed;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Projectile, meta = (AllowPrivateAccess = "true"))
	UStaticMeshComponent* ProjectileMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Movement, meta = (AllowPrivateAccess = "true"))
	UProjectileMovementComponent* ProjectileMovement;

	UPROPERTY()
	USphereComponent* CollisionComp;
};
