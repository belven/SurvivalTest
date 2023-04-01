// Copyright Epic Games, Inc. All Rights Reserved.

#include "SurvivalTestGameMode.h"
#include "BaseHUD.h"
#include "BasePlayerController.h"
#include "UObject/ConstructorHelpers.h"

ASurvivalTestGameMode::ASurvivalTestGameMode()
	: Super()
{
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnClassFinder(TEXT("/Game/FirstPerson/Blueprints/BP_FirstPersonCharacter"));
	DefaultPawnClass = PlayerPawnClassFinder.Class;

	HUDClass = ABaseHUD::StaticClass();
	PlayerControllerClass = ABasePlayerController::StaticClass();
}
