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
	float changeAmount;
	bool heals;

	UPROPERTY()
		ABaseCharacter* source;
};

USTRUCT(BlueprintType)
struct FCombatStateChange
{
	GENERATED_USTRUCT_BODY()
public:
	bool oldState;
	bool newState;

	UPROPERTY()
		UObject* source;
};