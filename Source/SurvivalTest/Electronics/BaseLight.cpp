#include "BaseLight.h"

ABaseLight::ABaseLight()
{
	PrimaryActorTick.bCanEverTick = true;

}

void ABaseLight::BeginPlay()
{
	Super::BeginPlay();
	
}

void ABaseLight::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}