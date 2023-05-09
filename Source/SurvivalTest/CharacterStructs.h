#pragma once
#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "CharacterStructs.generated.h"

class ABaseCharacter;

UCLASS()
class SURVIVALTEST_API UCharacterStructs : public UObject
{
	GENERATED_BODY()
};

USTRUCT(BlueprintType)
struct FHealthChange
{
	GENERATED_USTRUCT_BODY()
public:
	float changeAmount = 0;
	bool heals = false;

	UPROPERTY()
		ABaseCharacter* source = nullptr;
};

USTRUCT(BlueprintType)
struct FCombatStateChange
{
	GENERATED_USTRUCT_BODY()
public:
	bool oldState = false;
	bool newState = true;

	UPROPERTY()
		UObject* source = nullptr;
};

UENUM(BlueprintType)
enum class EMovementState : uint8
{
	Sprinting,
	Base,
	Steady,
	Slow,
	End
};

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
		stamina = other.stamina;
		waterLossRate = other.waterLossRate;
		hungerLossRate = other.hungerLossRate;
		restLossRate = other.restLossRate;
		staminaLossRate = other.staminaLossRate;
		staminaRecoverRate = other.staminaRecoverRate;
	}

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Stats, meta = (AllowPrivateAccess = "true"))
		float health = 0;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Stats, meta = (AllowPrivateAccess = "true"))
		float water = 0;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Stats, meta = (AllowPrivateAccess = "true"))
		float hunger = 0;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Stats, meta = (AllowPrivateAccess = "true"))
		float stamina = 0;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Stats, meta = (AllowPrivateAccess = "true"))
		float rest = 0;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Stats, meta = (AllowPrivateAccess = "true"))
		float waterLossRate = 0;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Stats, meta = (AllowPrivateAccess = "true"))
		float hungerLossRate = 0;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Stats, meta = (AllowPrivateAccess = "true"))
		float restLossRate = 0;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Stats, meta = (AllowPrivateAccess = "true"))
		float staminaLossRate = 0;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Stats, meta = (AllowPrivateAccess = "true"))
		float staminaRecoverRate = 0;
};