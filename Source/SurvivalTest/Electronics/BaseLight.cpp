#include "BaseLight.h"

#include "SurvivalTest/BaseGameInstance.h"

ABaseLight::ABaseLight()
{
	PrimaryActorTick.bCanEverTick = true;
}

void ABaseLight::BeginPlay()
{
	Super::BeginPlay();
	
	 game = Cast<UBaseGameInstance>(GetGameInstance());
}

void ABaseLight::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	AMainLight* mainLight = game->GetMainLight();

	if (mainLight && !bound)
	{
		bound = true;
		mainLight->OnTimeTransition.AddDynamic(this, &ABaseLight::TimeChanged);
	}
}