#include "Mission.h"

#include "Components/ShapeComponent.h"
#include "DrawDebugHelpers.h"
#include "MissionArea.h"

void AMission::BeginPlay()
{
	const FVector actorLocation = GetActorLocation();
	boxSize = 1000;
	boxHeight = 1000;

	//if (size == 1)
	//{
	//	SpawnBox(actorLocation);
	//}
	//else if (size == 2)
	//{
	//	for(int x = actorLocation.X - boxSize; x != actorLocation.X + boxSize *2; x += boxSize)
	//	{
	//		for (int y = actorLocation.Y - boxSize; y != actorLocation.Y + boxSize * 2; y += boxSize)
	//		{
	//			SpawnBox(FVector(x, y, actorLocation.Z + (boxHeight)));
	//		}
	//	}
	//}

	boxSize = 1000;
	boxHeight = 1000;

	int locationOffset = 0;

	if(size % 2 == 0)
	{
		locationOffset = (boxSize / 2);
	}
	else
	{
		locationOffset = boxSize;
	}

	int x = actorLocation.X - (boxSize * (size / 2)) - locationOffset;

	for (int indexX = 0; indexX < size; indexX++)
	{
		int y = actorLocation.Y - (boxSize * (size /2)) - locationOffset;
		x += boxSize;

		for (int indexY = 0; indexY < size; indexY++)
		{
			y += boxSize;
			SpawnBox(FVector(x, y, actorLocation.Z + (boxHeight)));
		}
	}
}

bool AMission::HasPlayers()
{
	for(auto& box : players)
	{
		if (box.Value > 0)
			return true;
	}
	return false;
}

void AMission::SpawnBox(FVector location)
{
	FActorSpawnParameters params;
	params.Owner = this;

	AMissionArea* area = GetWorld()->SpawnActor<AMissionArea>(location, GetActorRotation(), params);
	FVector extent = FVector(boxSize / 2, boxSize / 2, boxHeight);
	area->GetBox()->InitBoxExtent(extent);
	missionArea.Add(area);
	area->GetBox()->OnComponentBeginOverlap.AddUniqueDynamic(this, &AMission::BeginOverlap);
	area->GetBox()->OnComponentEndOverlap.AddUniqueDynamic(this, &AMission::EndOverlap);
	DrawDebugBox(GetWorld(), location, extent, FColor::Blue, true);
}

void AMission::EndOverlap(UPrimitiveComponent* overlappedComponent, AActor* otherActor, UPrimitiveComponent* otherComp, int32 otherBodyIndex)
{
	players.FindOrAdd(Cast<AMissionArea>(overlappedComponent->GetOwner()))--;
}

void AMission::BeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	players.FindOrAdd(Cast<AMissionArea>(OverlappedComponent->GetOwner()))++;
}