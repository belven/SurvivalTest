#include "LootBox.h"
#include "Armour.h"
#include "SurvivalTest/BaseGameInstance.h"
#include "ItemContainer.h"
#include "Kismet/GameplayStatics.h"
#include "SurvivalTest/BasePlayerController.h"
#include "SurvivalTest/Tables/ContainerTableData.h"
#include "SurvivalTest/HelperFunctions.h"

ALootBox::ALootBox()
{
	static ConstructorHelpers::FObjectFinder<UStaticMesh> baseMesh(TEXT("StaticMesh'/Game/FirstPerson/LootBoxes/Crate.Crate'"));
	PrimaryActorTick.bCanEverTick = false;
	boxMeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Loot Box Mesh"));
	boxMeshComp->SetCustomDepthStencilValue(2);
	boxMeshComp->SetStaticMesh(baseMesh.Object);
	boxMeshComp->SetCollisionProfileName("Interaction");
}

ALootBox::~ALootBox()
{
	//delete runnable;
}

void ALootBox::SetUpBox()
{
	if (!containerData.mesh.Equals(""))
	{
		UStaticMesh* mesh = LoadObject<UStaticMesh>(this, *containerData.mesh);
		boxMeshComp->SetStaticMesh(mesh);
		//boxMeshComp->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	}
	CreateLootboxData();
}

void ALootBox::BeginPlay()
{
	Super::BeginPlay();
	SetActorHiddenInGame(false);

	if (!defaultContainer.IsEmpty())
	{
		for (auto cd : GetGame()->GetTableManager()->GetContainerData()->GetData())
		{
			if (cd.Value.name.Equals(defaultContainer))
			{
				SetContainerData(cd.Value);
				SpawnLoot();
			}
		}
	}
}

void ALootBox::Interact(ABasePlayerController* instigator)
{
	instigator->OpenInventory();
}

void ALootBox::Highlight(bool activate)
{
	if (isHighlighted != activate) {
		if (!isHighlighted && mIsTimerActive(TimerHandle_LootboxClear))
		{
			mSetTimer(TimerHandle_LootboxClear, &ALootBox::RemoveLootBox, minTime);
		}

		isHighlighted = activate;
		boxMeshComp->SetRenderCustomDepth(activate);
	}
}

UBaseGameInstance* ALootBox::GetGame()
{
	if (!gameIn) {
		gameIn = GameInstance(GetWorld());
	}
	return gameIn;
}

void ALootBox::ClearData()
{
	itemTypes.Empty();

	if (container != NULL) {
		for (FInstanceItemData iid : GetGame()->GetInstancedItemsForContainer(container->GetInstanceContainerData().ID))
		{
			GetGame()->GetInstancedItems().Remove(iid.ID);
		}
	}
}

void ALootBox::SpawnLoot()
{
	SetUpBox();

	if (itemTypes.IsEmpty())
	{
		itemTypes = GetGame()->GetTableManager()->GetContainerItems(GetContainerData().ID);
	}

	if (!itemTypes.IsEmpty())
	{
		for (int i = 0; i < FMath::RandRange(minItems, GetContainerData().slots); ++i)
		{
			int32 lootItem = mGetRandom<int32>(itemTypes);
			FItemData id = GetGame()->GetItemData(lootItem);
			FInstanceItemData iid = CreateLoot(id);

			TArray<int32> ids;
			FInstanceItemData added = container->AddItem(iid, ids);

			if (added.amount == 0 && id.type == EItemType::Armour)
			{
				UArmour::CreateArmour(id.ID, GetGame(), ids[0]);
			}
		}
	}
	else
	{
		UE_LOG(LogTemp, Log, TEXT("Failed to get item types for container %s"), *GetContainerData().name);
	}
}

void ALootBox::CreateLootboxData()
{
	icd.ID = GetGame()->GetNextInstanceContainerDataID();
	icd.containerID = containerData.ID;
	icd.type = EContainerType::Box;
	GetGame()->GetInstancedContainers().Add(icd.ID, icd);

	ibd.ID = GetGame()->GetNextInstanceBoxDataID();
	ibd.containerInstanceID = icd.ID;
	GetGame()->GetInstancedBoxes().Add(ibd.ID, ibd);

	icd.name = "Loot Box " + FString::FromInt(ibd.ID);

	container = UItemContainer::CreateItemContainer(GetGame()->GetContainerDataByID(containerData.ID), icd, gameIn);

	if (container != NULL)
	{
		container->OnItemUpdated.AddUniqueDynamic(this, &ALootBox::ItemUpdated);
	}
	else 
	{
		UE_LOG(LogTemp, Log, TEXT("Failed to create container instance for %s"), *GetContainerData().name);
	}
}

FInstanceItemData ALootBox::CreateLoot(const FItemData& id)
{
	FInstanceItemData iid;
	if (id.ID != UItemStructs::InvalidInt)
	{
		int32 randomAmount = FMath::RandRange(1, id.maxStack);
		return FInstanceItemData(id.ID, randomAmount);
	}
	return {};
}


void ALootBox::RemoveLootBox()
{
	// TODO sort out loot boxes despawning etc.
	//Destroy();
}

void ALootBox::ItemUpdated(const FInstanceItemData& inItem, const FInstanceItemData& oldItem)
{
	if (!mIsTimerActive(TimerHandle_LootboxClear))
		mSetTimer(TimerHandle_LootboxClear, &ALootBox::RemoveLootBox, 60.0f);

	if (mTimeRemaining(TimerHandle_LootboxClear) < minTime)
	{
		mSetTimer(TimerHandle_LootboxClear, &ALootBox::RemoveLootBox, minTime);
	}
}

void ALootBox::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);
}
