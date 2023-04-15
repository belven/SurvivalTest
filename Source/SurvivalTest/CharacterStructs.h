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