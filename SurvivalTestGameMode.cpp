// Copyright Epic Games, Inc. All Rights Reserved.

#include "SurvivalTestGameMode.h"
#include "SurvivalTestCharacter.h"
#include "UObject/ConstructorHelpers.h"

ASurvivalTestGameMode::ASurvivalTestGameMode()
	: Super()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnClassFinder(TEXT("/Game/FirstPerson/Blueprints/BP_FirstPersonCharacter"));
	DefaultPawnClass = PlayerPawnClassFinder.Class;

}
