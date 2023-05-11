#include "Mission.h"
#include "Components/ShapeComponent.h"
#include "DrawDebugHelpers.h"
#include "MissionArea.h"
#include "SurvivalTest/BaseCharacter.h"
#include <NavigationSystem.h>
#include "MissionManager.h"
#include "SurvivalTest/BasePlayerController.h"
#include "SurvivalTest/Items/LootBox.h"
#include "SurvivalTest/BaseGameInstance.h"
#include "SurvivalTest/Events/BaseEvent.h"
#include "SurvivalTest/Events/RPGEventManager.h"
#include "SurvivalTest/Tables/ContainerTableData.h"
#include "SurvivalTest/Tables/LoadoutTableData.h"
#include "SurvivalTest/Tables/Mission/MissionItemTable.h"
#include "SurvivalTest/Tables/Mission/MissionLoadoutTable.h"

AMission::AMission()
{
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnClassFinder(TEXT("Blueprint'/Game/FirstPerson/Blueprints/AI.AI_C'"));
	AIClass = PlayerPawnClassFinder.Class;

	navInvoker = CreateDefaultSubobject<UNavigationInvokerComponent>(TEXT("navInvoker"));
	navInvoker->SetGenerationRadii(boxSize, boxSize * 1.2);
}

FContainerData AMission::GetRandomContainerData()
{
	TArray<FContainerData> cds;

	for (auto& cdFound : game->GetTableManager()->GetContainerData()->GetData())
	{
		if (cdFound.Value.type == missionType)
		{
			cds.Add(cdFound.Value);
		}
	}

	if (cds.Num() > 0)
	{
		return cds[FMath::RandRange(0, cds.Num() - 1)];
	}
	return {};
}

void AMission::SetUpLootBoxes()
{
	TArray<AActor*> actors;
	missionArea[0]->GetBox()->GetOverlappingActors(actors, ALootBox::StaticClass());

	for (AActor* actor : actors)
	{
		ALootBox* loot = Cast<ALootBox>(actor);
		FContainerData cd = GetRandomContainerData();
		TArray<int32> itemTypes = game->GetTableManager()->GetItemsForMissionType(missionType);	

		if (cd.ID != UItemStructs::InvalidInt)
		{
			loot->SetContainerData(cd);
			loot->SetItemTypes(itemTypes);
			loot->SetActorHiddenInGame(false);
			loot->ClearData();
			loot->SpawnLoot();
		}
	}
}

void AMission::BeginPlay()
{
	Super::BeginPlay();
	navInvoker->SetGenerationRadii(boxSize, boxSize * 1.2);
	navInvoker->RegisterWithNavigationSystem(*UNavigationSystemV1::GetCurrent(GetWorld()));

	game = mGameInstance();
	game->GetMissionManager()->AddMission(this);
	game->GetEventManager()->OnEventTriggered.AddUniqueDynamic(this, &AMission::EventTriggered);
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
	missionArea.Add(area);

	area->GetBox()->OnComponentBeginOverlap.AddUniqueDynamic(this, &AMission::BeginOverlap);
	area->GetBox()->OnComponentEndOverlap.AddUniqueDynamic(this, &AMission::EndOverlap);
	//DrawDebugBox(GetWorld(), location, extent, FColor::Blue, true);
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

void AMission::EventTriggered(UBaseEvent* inEvent)
{
	if (!missionComplete && inEvent->GetEventType() == EEventType::PostHealthChange && aiSpawned.Contains(inEvent->GetEventOwner()))
	{
		bool aiAlive = false;
		for (ABaseCharacter* character : aiSpawned)
		{
			if (character->IsAlive())
			{
				aiAlive = true;
				break;
			}
		}

		if (!aiAlive)
			MissionComplete();
	}
}

void AMission::MissionComplete()
{
	missionComplete = true;
	SetUpLootBoxes();
}

void AMission::SpawnMission()
{
	if (!HasPlayers() && spawnMission)
	{
		spawnMission = false;
		missionSpawned = true;
		FNavLocation location;
		UMissionLoadoutTable* mlt = game->GetTableManager()->GetMissionLoadoutTable();

		FActorSpawnParameters params;
		params.Owner = this;
		params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
		
		for (auto& mld : mlt->GetData())
		{
			FLoadoutData ld = game->GetTableManager()->GetLoadoutTableData()->GetLoadoutDataByID(mld.loadoutID);

			UNavigationSystemV1* nav = UNavigationSystemV1::GetCurrent(GetWorld());
			nav->GetRandomPointInNavigableRadius(GetActorLocation(), boxSize / 2, location);

			ABaseCharacter* character = GetWorld()->SpawnActor<ABaseCharacter>(AIClass, location, GetActorRotation(), params);

			if (character)
			{
				character->SetupLoadout(ld.name);
				aiSpawned.Add(character);
			}
		}
	}
	else
	{
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