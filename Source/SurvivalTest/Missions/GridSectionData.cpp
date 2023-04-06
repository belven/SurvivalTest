#include "GridSectionData.h"

AGridSection::AGridSection()
{
	gridSectionComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Grid Mesh Comp"));
	gridSectionComp->SetupAttachment(RootComponent);
}

AGridSection* AGridSection::CreateGridSection(UWorld* world, FGridSectionData data)
{
	FActorSpawnParameters params;
	AGridSection* gs = world->SpawnActor<AGridSection>(data.CornerLocation(), FRotator(0), params);
	gs->sectionData = data;
	return gs;
}

void AGridSection::BeginPlay()
{
	Super::BeginPlay();
}
