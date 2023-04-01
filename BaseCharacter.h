#pragma once
#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "BaseCharacter.generated.h"

class UInputComponent;
class USkeletalMeshComponent;
class USceneComponent;
class UCameraComponent;
class UAnimMontage;
class USoundBase;

USTRUCT(BlueprintType)
struct FCharacterStats
{
	GENERATED_USTRUCT_BODY()
public:
	void CopyStats(FCharacterStats other)
	{
		health = other.health;
		water = other.water;
		hunger = other.hunger;
		rest = other.rest;
		waterLossRate = other.waterLossRate;
		hungerLossRate = other.hungerLossRate;
		restLossRate = other.restLossRate;
	}

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Stats, meta = (AllowPrivateAccess = "true"))
		float health;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Stats, meta = (AllowPrivateAccess = "true"))
		float water;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Stats, meta = (AllowPrivateAccess = "true"))
		float hunger;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Stats, meta = (AllowPrivateAccess = "true"))
		float rest;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Stats, meta = (AllowPrivateAccess = "true"))
		float waterLossRate;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Stats, meta = (AllowPrivateAccess = "true"))
		float hungerLossRate;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Stats, meta = (AllowPrivateAccess = "true"))
		float restLossRate;
};

UCLASS(config = Game)
class ABaseCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	ABaseCharacter();

	USkeletalMeshComponent* GetMesh1P() const { return Mesh1P; }
	UCameraComponent* GetFirstPersonCameraComponent() const { return FirstPersonCameraComponent; }
	
	FCharacterStats GetCurrentStats() const
	{
		return currentStats;
	}

	FCharacterStats GetMaxStats() const
	{
		return maxStats;
	}

	virtual void Tick(float DeltaSeconds) override;

protected:
	virtual void BeginPlay() override;
	void DrainStat(float& stat, float drainRate, float healthDamage, float deltaSeconds);
		
	UPROPERTY(VisibleDefaultsOnly, Category = Mesh)
		USkeletalMeshComponent* Mesh1P;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
		UCameraComponent* FirstPersonCameraComponent;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Stats, meta = (AllowPrivateAccess = "true"))
		FCharacterStats currentStats;

	UPROPERTY()
		FCharacterStats maxStats;
};