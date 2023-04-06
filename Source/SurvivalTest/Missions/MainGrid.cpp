#include "MainGrid.h"

#include "GridSectionData.h"

void AMainGrid::ClearGrid()
{
	for(AGridSection* gs : gridSections)
	{
		if(gs)
			gs->Destroy();
	}

	gridSections.Empty(columnsAndRows * columnsAndRows);
}

void AMainGrid::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);
	const FVector startLocation = FVector(0, 0, 0);
	int32 halfBoxSize = boxSize / 2;
	float startingDistance = boxSize * (columnsAndRows / 2);
	
	ClearGrid();

	int locationOffset = boxSize;;

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
			double inZ = startLocation.Z + heightAboveGround;
			FVector location = FVector(x, y, inZ);

			// Create a grid section at the correct location
			AGridSection* gs = AGridSection::CreateGridSection(GetWorld(), FGridSectionData(location, boxSize));
			gs->GetGridSectionComp()->SetStaticMesh(mesh);
			gs->GetGridSectionComp()->SetWorldScale3D(FVector(boxSize / 100, boxSize / 100, 0.5));
			gridSections.Add(gs);

			DrawDebugBox(GetWorld(), location, extent, FColor::Blue, false, debugDuration);
		}
	}
}

AMainGrid::AMainGrid()
{
	static ConstructorHelpers::FObjectFinder<UStaticMesh> cube(TEXT("StaticMesh'/Game/LevelPrototyping/Meshes/SM_Cube.SM_Cube'"));
	mesh = cube.Object;
}
