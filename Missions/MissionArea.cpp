#include "MissionArea.h"

AMissionArea::AMissionArea()
{
	PrimaryActorTick.bCanEverTick = true;
	box = CreateDefaultSubobject<UBoxComponent>(TEXT("Mission Area Box"));
	box->SetupAttachment(RootComponent);
}

void AMissionArea::BeginPlay()
{
	Super::BeginPlay();

}

void AMissionArea::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}