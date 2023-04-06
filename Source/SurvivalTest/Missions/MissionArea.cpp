#include "MissionArea.h"

AMissionArea::AMissionArea()
{
	PrimaryActorTick.bCanEverTick = true;
	box = CreateDefaultSubobject<UBoxComponent>(TEXT("Mission Area Box"));
	RootComponent = box;
}

void AMissionArea::BeginPlay()
{
	Super::BeginPlay();

}

void AMissionArea::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}