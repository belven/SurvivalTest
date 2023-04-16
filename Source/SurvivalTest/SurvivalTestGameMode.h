#pragma once
#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "SurvivalTestGameMode.generated.h"

UCLASS(minimalapi)
class ASurvivalTestGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	virtual void StartPlay() override;
	ASurvivalTestGameMode();
};