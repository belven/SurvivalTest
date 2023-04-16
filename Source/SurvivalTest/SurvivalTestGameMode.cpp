#include "SurvivalTestGameMode.h"
#include "BasePlayerController.h"
#include "Missions/MissionManager.h"
#include "UI/BaseHUD.h"
#include "UObject/ConstructorHelpers.h"
#include "SurvivalTest/BaseGameInstance.h"

void ASurvivalTestGameMode::StartPlay()
{
	Super::StartPlay();
	mGameInstance()->GetMissionManager()->StartPlay();
}

ASurvivalTestGameMode::ASurvivalTestGameMode()
	: Super()
{
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnClassFinder(TEXT("Blueprint'/Game/FirstPerson/Blueprints/MainCharacter_BP.MainCharacter_BP_C'"));
	DefaultPawnClass = PlayerPawnClassFinder.Class;

	HUDClass = ABaseHUD::StaticClass();
	PlayerControllerClass = ABasePlayerController::StaticClass();
}
