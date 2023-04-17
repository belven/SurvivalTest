#include "GridSectionData.h"

AGridSection::AGridSection()
{
	gridSectionComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Grid Mesh Comp"));
	RootComponent = gridSectionComp;
	SetFolderPath("Grid");
}

void AGridSection::BeginDestroy()
{
	//if(gridSectionComp && this)
	//gridSectionComp->DestroyComponent(true);
	Super::BeginDestroy();
}

AGridSection* AGridSection::CreateGridSection(UWorld* world, FGridSectionData data)
{
	FActorSpawnParameters params;
	AGridSection* gs = world->SpawnActor<AGridSection>(data.CornerLocation(), FRotator(0), params);
	gs->sectionData = data;
	return gs;
}

void AGridSection::HighlightSection(float time)
{
	DrawDebugBox(GetWorld(), sectionData.centerLocation, FVector(sectionData.sectionSize / 2), FColor::Blue, false, time);
}

void AGridSection::BeginPlay()
{
	Super::BeginPlay();
}
