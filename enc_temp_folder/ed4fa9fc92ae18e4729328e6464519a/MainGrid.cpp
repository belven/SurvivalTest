#include "MainGrid.h"

#include "GridSectionData.h"
#include "SurvivalTest/BaseGameInstance.h"

void AMainGrid::ClearGrid()
{
	
	for(AGridSection* gs : gridSections)
	{
		if (gs) {
			gs->Destroy();
		}
	}

	gridSections.Empty(columnsAndRows * columnsAndRows);
}

void AMainGrid::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);

}

AGridSection* AMainGrid::GetGridSection(FVector loc)
{
	AGridSection* section = nullptr;
	int32 x = ModValue(loc.X);
	int32 y = ModValue(loc.Y);

	for(AGridSection* gs : gridSections)
	{
		if(gs->GetActorLocation().Y == y && gs->GetActorLocation().X == x)
		{
			section = gs;
			break;
		}
	}
	return section;
	//return grid[x][y];
}

int32 AMainGrid::ModValue(double value)
{
	int32 valueInt = FMath::RoundToInt32(value);
	return valueInt -= (valueInt % boxSize);
}

AMainGrid::AMainGrid()
{
	static ConstructorHelpers::FObjectFinder<UStaticMesh> cube(TEXT("StaticMesh'/Game/LevelPrototyping/Meshes/SM_Cube.SM_Cube'"));
	mesh = cube.Object;
}

AMainGrid::~AMainGrid()
{
	ClearGrid();
}

void AMainGrid::BeginPlay()
{
	Super::BeginPlay();

	mGameInstance()->grid = this;


	const FVector startLocation = FVector(0, 0, 0);
	int32 halfBoxSize = boxSize / 2;
	float startingDistance = boxSize * (columnsAndRows / 2);

	ClearGrid();

	int locationOffset = boxSize;

	// if this is 0, is means it's divisible by 2 / even, therefore we need to shift the offset by half the box size so the center is based on corner.
	// As you can't have a center in a 2x2 that's in the middle of a one section
	if (columnsAndRows % 2 == 0)
	{
		locationOffset = (boxSize / 2);
	}

	// Move the starting location by the distance from 000
	// This will move us, so we start from the right hand corner section
	int x = startLocation.X - startingDistance - locationOffset;

	// Loop over are grid size, this makes a size * size grid,
	// i,e. columnsAndRows = 2, so we have 2 rows and 2 columns
	for (int indexX = 0; indexX < columnsAndRows; indexX++)
	{
		// Every time we do another loop of X, reset Y so it goes back to the correct column
		int y = startLocation.Y - startingDistance - locationOffset;
		x += boxSize;

		for (int indexY = 0; indexY < columnsAndRows; indexY++)
		{
			y += boxSize;

			// Extends are done from a center position, so use half box size to create the box at the right location
			FVector extent = FVector(halfBoxSize, halfBoxSize, boxHeight);

			// Increase from 0 z by height given, this might be pointless later and likely overriden by landscape generation etc.
			// -1 for the 0.01 scale of 100 size of the cube, so it sits on 0 exactly
			double inZ = startLocation.Z + heightAboveGround - 1;
			FVector location = FVector(x, y, inZ);

			// Create a grid section at the correct location
			AGridSection* gs = AGridSection::CreateGridSection(GetWorld(), FGridSectionData(location, boxSize));
			gs->GetGridSectionComp()->SetStaticMesh(mesh);
			gs->GetGridSectionComp()->SetWorldScale3D(FVector(boxSize / 100, boxSize / 100, 0.01f));
			gridSections.Add(gs);

			//grid[ModValue(gs->GetActorLocation().X) / boxSize][ModValue(gs->GetActorLocation().Y) / boxSize] = gs;
			//grid.
			//DrawDebugBox(GetWorld(), location, extent, FColor::Blue, false, debugDuration);
		}
	}
}

void AMainGrid::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	ClearGrid();
}
