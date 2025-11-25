#include "Mission.h"
#include "DrawDebugHelpers.h"
#include "MissionArea.h"
#include "SurvivalTest/BaseCharacter.h"
#include <NavigationSystem.h>
#include "MissionManager.h"
#include "Kismet/GameplayStatics.h"
#include "SurvivalTest/BaseAIController.h"
#include "SurvivalTest/Items/LootBox.h"
#include "SurvivalTest/BaseGameInstance.h"
#include "SurvivalTest/HelperFunctions.h"
#include "SurvivalTest/Tables/ContainerTableData.h"
#include "SurvivalTest/Tables/LoadoutTableData.h"
#include "SurvivalTest/Tables/Items/ItemDataTable.h"
#include "SurvivalTest/Tables/Mission/MissionLoadoutTable.h"

AMission::AMission()
{
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnClassFinder(TEXT("Blueprint'/Game/FirstPerson/Blueprints/AI.AI_C'"));
	AIClass = PlayerPawnClassFinder.Class;

	navInvoker = CreateDefaultSubobject<UNavigationInvokerComponent>(TEXT("navInvoker"));
}

FContainerData AMission::GetRandomContainerData()
{
	FContainerData foundData = {};

	// TODO clean up data once mission is over

	if (cds.IsEmpty())
	{
		for (auto& cdFound : game->GetTableManager()->GetMissionContainers(missionType))
		{
			FContainerData containerData = game->GetTableManager()->GetContainerDataByID(cdFound.containerID);

			if (containerData.ID != UItemStructs::InvalidInt)
			{
				cds.Add(containerData);
			}
			else
			{
				UE_LOG(LogTemp, Log, TEXT("Container data invalid for mission type %s"), *UMissionStructs::GetMissionTypeString(missionType));				
			}
		}
	}

	if (!cds.IsEmpty())
	{
		foundData = UHelperFunctions::GetRandom(cds, FContainerData());
	}
	else
	{
		UE_LOG(LogTemp, Log, TEXT("No containers set for mission type %s"), *UMissionStructs::GetMissionTypeString(missionType));
	}

	return foundData;
}

void AMission::SetUpLootBoxes()
{
	TArray<AActor*> actors;
	missionArea[0]->GetBox()->GetOverlappingActors(actors, ALootBox::StaticClass());
	cds.Empty();

	for (AActor* actor : actors)
	{
		ALootBox* loot = Cast<ALootBox>(actor);
		loot->SetActorHiddenInGame(false);
		loot->ClearData();

		FContainerData cd = GetRandomContainerData();

		if (cd.ID != UItemStructs::InvalidInt)
		{
			TArray<int32>	itemTypes = game->GetTableManager()->GetContainerItems(cd.ID);
			loot->SetContainerData(cd);
			loot->SetItemTypes(itemTypes);
			loot->SpawnLoot();
		}
		else
		{
			UE_LOG(LogTemp, Log, TEXT("No containers set for mission type %s, for loot box %s"), *UMissionStructs::GetMissionTypeString(missionType), *loot->GetActorNameOrLabel());
		}
	}
}

void AMission::SpawnDefault()
{
	EMissionType mt = mGetRandomEnum<EMissionType>(EMissionType::End);
	SetMissionType(mt);
	SetUpLootBoxes();
	SpawnAI();
}

void AMission::BeginPlay()
{
	Super::BeginPlay();
	navInvoker->SetGenerationRadii(boxSize, boxSize);
	navInvoker->Activate();
	navInvoker->RegisterWithNavigationSystem(*UNavigationSystemV1::GetCurrent(GetWorld()));

	game = mGameInstance();
	game->GetMissionManager()->AddMission(this);
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

void AMission::SpawnBox(const FVector& location)
{
	FActorSpawnParameters params;
	params.Owner = this;

	AMissionArea* area = GetWorld()->SpawnActor<AMissionArea>(location, GetActorRotation(), params);
	FVector extent = FVector(boxSize / 2, boxSize / 2, boxHeight);
	area->GetBox()->SetBoxExtent(extent);
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

void AMission::MissionComplete()
{
	missionComplete = true;
	SetUpLootBoxes();
}

void AMission::SpawnAI()
{
	FNavLocation location;
	UMissionLoadoutTable* mlt = game->GetTableManager()->GetMissionLoadoutTable();

	FActorSpawnParameters params;
	params.Owner = this;
	params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

	UNavigationSystemV1* nav = UNavigationSystemV1::GetCurrent(GetWorld());

	for (auto& mld : mlt->GetData())
	{
		if (mld.type == GetMissionType())
		{
			FLoadoutData ld = game->GetTableManager()->GetLoadoutTableData()->GetLoadoutDataByID(mld.loadoutID);

			nav->GetRandomPointInNavigableRadius(GetActorLocation(), boxSize / 2, location);

			ABaseCharacter* character = GetWorld()->SpawnActor<ABaseCharacter>(AIClass, location, GetActorRotation(), params);

			if (character)
			{
				character->SetupLoadout(ld.name);
				aiSpawned.Add(character);
				character->OnCharacterDied.AddUniqueDynamic(this, &AMission::CharacterDied);
			}
		}
	}

	aiAlive = aiSpawned;

	for (ABaseCharacter* character : aiSpawned)
	{
		if (character) 
		{
			ABaseAIController* con = Cast<ABaseAIController>(character->GetController());

			if (con) 
			{
				con->alliesSeen = aiSpawned;
				con->alliesSeen.Remove(character);
			}
		}
	}
}

void AMission::SpawnMission_Internal()
{
	if (!HasPlayers() && spawnMission)
	{
		spawnMission = false;
		missionSpawned = true;

		SpawnAI();

		SetUpLootBoxes();
	}
}

void AMission::NavDone(ANavigationData* inNavData)
{
	SpawnMission_Internal();
}

void AMission::SpawnMission()
{
	UNavigationSystemV1* navigationSystemV1 = UNavigationSystemV1::GetCurrent(GetWorld());

	if (navigationSystemV1->IsNavigationBuildInProgress()) {
		navigationSystemV1->OnNavigationGenerationFinishedDelegate.AddUniqueDynamic(this, &AMission::NavDone);
	}
	else
	{
		SpawnMission_Internal();
	}
}

bool AMission::IsPlayer(AActor* inActor, UPrimitiveComponent* inOtherComp)
{
	if (inActor->IsA(ABaseCharacter::StaticClass()))
	{
		ABaseCharacter* character = Cast<ABaseCharacter>(inActor);

		return character->IsPlayer();
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

void AMission::CharacterDied(ABaseCharacter* character)
{
	aiAlive.Remove(character);

	if (aiAlive.IsEmpty()) {
		MissionComplete();
	}
}
