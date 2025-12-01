#include "PlayerBuildingComponent.h"

#include "Kismet/GameplayStatics.h"
#include "SurvivalTest/BaseCharacter.h"
#include "SurvivalTest/BaseGameInstance.h"
#include "SurvivalTest/BaseBuilding/BuildingPart.h"

UPlayerBuildingComponent::UPlayerBuildingComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}


void UPlayerBuildingComponent::BeginPlay()
{
	Super::BeginPlay();
	
}


void UPlayerBuildingComponent::Build()
{
	TArray<AActor*> actors;
	GetBaseCharacter()->GetOverlappingActors(actors, ABuildingPart::StaticClass());

	if (!actors.IsEmpty()) {
		for (AActor* actor : actors)
		{
			ABuildingPart* bp = Cast<ABuildingPart>(actor);
			CreateBuildingPart(bp, EOneDirection::North);
			CreateBuildingPart(bp, EOneDirection::NorthEast);
			CreateBuildingPart(bp, EOneDirection::NorthWest);
			CreateBuildingPart(bp, EOneDirection::South);
			CreateBuildingPart(bp, EOneDirection::SouthEast);
			CreateBuildingPart(bp, EOneDirection::SouthWest);
			CreateBuildingPart(bp, EOneDirection::East);
			CreateBuildingPart(bp, EOneDirection::West);
		}
	}
	else
	{

	}

}

void UPlayerBuildingComponent::CreateBuildingPart(ABuildingPart* bp, EOneDirection direction)
{
	FRotator rotation;
	FVector location;

	if (bp != NULL) {
		location = bp->GetActorLocation();
		rotation = bp->GetActorRotation();
	}

	location = MoveVectorByDirection(location, rotation, 120, direction);

	FString location_string = FString::SanitizeFloat(location.X) + "X" + FString::SanitizeFloat(location.Y) + "Y" + FString::SanitizeFloat(location.Z) + "Z";

	TMap<FString, ABuildingPart*> building_parts = GetBaseGameInstance()->GetBuildingParts();

	if (!building_parts.Contains(location_string))
	{
		FActorSpawnParameters params;
		params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::DontSpawnIfColliding;
		ABuildingPart* newBP = GetBaseCharacter()->GetWorld()->SpawnActor<ABuildingPart>(ABuildingPart::StaticClass(), location, rotation, params);

		if (newBP) {
			building_parts.Add(location_string, newBP);

			FString name;

			switch (direction)
			{
			case EOneDirection::North:
				name = "North";
				break;
			case EOneDirection::East:
				name = "East";
				break;
			case EOneDirection::West:
				name = "West";
				break;
			case EOneDirection::South:
				name = "South";
				break;
			case EOneDirection::End:
				name = "End";
				break;
			case EOneDirection::NorthEast:
				name = "NorthEast";
				break;
			case EOneDirection::NorthWest:
				name = "NorthWest";
				break;
			case EOneDirection::SouthEast:
				name = "SouthEast";
				break;
			case EOneDirection::SouthWest:
				name = "SouthWest";
				break;
			}

			if (newBP) {
				newBP->SetActorLabel(name);
			}
		}
	}
	else
	{
		UE_LOG(LogTemp, Log, TEXT("building_parts building already exists"));
	}
}


FVector UPlayerBuildingComponent::MoveVectorByDirection(const FVector& centerLocation, const FRotator& currentRotation, float distance, EOneDirection direction)
{
	const FRotator ninety = FRotator(0, currentRotation.Yaw + 90, 0);
	FVector north = FVector(distance, 0, 0);
	FVector east = FVector(0, distance, 0);
	FVector out = centerLocation;

	out = currentRotation.RotateVector(out);
	north = currentRotation.RotateVector(north);
	east = currentRotation.RotateVector(east);

	switch (direction)
	{
	case EOneDirection::North:
		out += north;
		break;
	case EOneDirection::NorthWest:
		out += north;
		out -= east;
		break;
	case EOneDirection::NorthEast:
		out += east;
		out += north;
		break;
	case EOneDirection::East:
		north = ninety.RotateVector(north);
		out += north;
		break;
	case EOneDirection::West:
		north = ninety.RotateVector(north);
		out -= north;
		break;
	case EOneDirection::South:
		out -= north;
		break;
	case EOneDirection::SouthEast:
		out -= north;
		out += east;
		break;
	case EOneDirection::SouthWest:
		out -= north;
		out -= east;
		break;
	case EOneDirection::End:
		break;
	}

	return out;
}

UBaseGameInstance* UPlayerBuildingComponent::GetBaseGameInstance()
{
	if (baseGameInstance == NULL)
	{
		baseGameInstance = GameInstance(GetWorld());
	}

	return baseGameInstance;
}

void UPlayerBuildingComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

}