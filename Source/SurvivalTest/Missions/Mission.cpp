#include "Mission.h"

#include "Components/ShapeComponent.h"
#include "DrawDebugHelpers.h"
#include "MissionArea.h"
#include "SurvivalTest/BaseCharacter.h"
#include <NavigationSystem.h>

#include "SurvivalTest/BasePlayerController.h"
#include "SurvivalTest/Items/LootBox.h"

AMission::AMission()
{
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnClassFinder(TEXT("Blueprint'/Game/FirstPerson/Blueprints/AI.AI_C'"));
	AIClass = PlayerPawnClassFinder.Class;
}

void AMission::SetUpLootBoxes()
{
	TArray<AActor*> actors;
	missionArea[0]->GetBox()->GetOverlappingActors(actors, ALootBox::StaticClass());

	for(AActor* actor : actors)
	{
		ALootBox* loot = Cast<ALootBox>(actor);

		loot->SetItemTypes(itemTypes);
		loot->SpawnLoot();
	}
}

void AMission::BeginPlay()
{
	Super::BeginPlay();

	SpawnBox(GetActorLocation());
}

bool AMission::HasPlayers()
{
	for (auto& box : players)
	{
		if (box.Value > 0)
		{
			return true;
		}
	}
	return false;
}

void AMission::SpawnBox(FVector location)
{
	FActorSpawnParameters params;
	params.Owner = this;

	AMissionArea* area = GetWorld()->SpawnActor<AMissionArea>(location, GetActorRotation(), params);
	FVector extent = FVector(boxSize / 2, boxSize / 2, boxHeight);
	area->GetBox()->SetBoxExtent(extent);
	//area->GetBox()->SetCollisionResponseToChannel(ECollisionChannel::ECC_GameTraceChannel2, ECR_Overlap);
	missionArea.Add(area);
	area->GetBox()->OnComponentBeginOverlap.AddUniqueDynamic(this, &AMission::BeginOverlap);
	area->GetBox()->OnComponentEndOverlap.AddUniqueDynamic(this, &AMission::EndOverlap);
	DrawDebugBox(GetWorld(), location, extent, FColor::Blue, true);
}

void AMission::EndOverlap(UPrimitiveComponent* overlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 otherBodyIndex)
{
	if (IsPlayer(OtherActor, OtherComp))
	{
		players.FindOrAdd(Cast<AMissionArea>(overlappedComponent->GetOwner()))--;

		if (!HasPlayers() && spawnMission)
		{
			GetWorld()->GetTimerManager().SetTimer(TimerHandle_CheckNoPlayers, this, &AMission::SpawnMission, 3.0f);
		}
	}
}

void AMission::SpawnMission()
{
	if (!HasPlayers() && spawnMission)
	{
		FActorSpawnParameters params;
		params.Owner = this;
		FNavLocation location;

		for (int i = 0; i < enemyAmount; ++i)
		{
			UNavigationSystemV1* nav = UNavigationSystemV1::GetCurrent(GetWorld());
			nav->GetRandomPointInNavigableRadius(GetActorLocation(), size * (boxSize / 2), location);

			GetWorld()->SpawnActor<ABaseCharacter>(AIClass, location, GetActorRotation(), params);
		}

		SetUpLootBoxes();
	}
}

bool AMission::IsPlayer(AActor* inActor, UPrimitiveComponent* inOtherComp)
{
	// TODO find better way to check for player
	if (inActor->IsA(ABaseCharacter::StaticClass()) && inOtherComp->GetName().Equals("CollisionCylinder"))
	{
		ABaseCharacter* character = Cast<ABaseCharacter>(inActor);

		return character->GetController() && character->GetController()->IsA(ABasePlayerController::StaticClass());
	}
	return false;
}

void AMission::BeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (IsPlayer(OtherActor, OtherComp))
	{
		GetWorld()->GetTimerManager().ClearTimer(TimerHandle_CheckNoPlayers);
		players.FindOrAdd(Cast<AMissionArea>(OverlappedComponent->GetOwner()))++;
	}
}
