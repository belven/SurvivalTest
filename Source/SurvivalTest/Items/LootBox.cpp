#include "LootBox.h"
#include "Armour.h"
#include "SurvivalTest/BaseGameInstance.h"
#include "ItemContainer.h"
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

	if (!defaultContainer.Equals(""))
	{
		for (auto cd : GetGame()->GetTableManager()->GetContainerData()->GetData())
		{
			if (cd.Value.name.Equals(defaultContainer))
			{
				SetContainerData(cd.Value);
				itemTypes = GetGame()->GetTableManager()->GetItemsForMissionType(cd.Value.type);
			}
		}

		if (GetContainerData().ID != UItemStructs::InvalidInt)
			SpawnLoot();
	}
}

void ALootBox::Interact(ABasePlayerController* instigator)
{
	instigator->OpenInventory();
}

void ALootBox::Highlight(bool activate)
{
	if (!isHighlighted && mIsTimerActive(TimerHandle_LootboxClear))
	{
		mSetTimer(TimerHandle_LootboxClear, &ALootBox::RemoveLootBox, minTime);
	}

	isHighlighted = activate;
	boxMeshComp->SetRenderCustomDepth(activate);
}

UBaseGameInstance* ALootBox::GetGame()
{
	if (!gameIn)
		gameIn = GameInstance(GetWorld());
	return gameIn;
}

void ALootBox::ClearData()
{
	itemTypes.Empty();

	for (FInstanceItemData iid : GetGame()->GetInstancedItemsForContainer(container->GetInstanceContainerData().ID))
	{
		GetGame()->GetInstancedItems().Remove(iid.ID);
	}
}

void ALootBox::SpawnLoot()
{
	SetUpBox();

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

void ALootBox::CreateLootboxData()
{
	ibd.ID = GetGame()->GetNextInstanceBoxDataID();
	ibd.containerInstanceID = icd.ID;
	ibd.boxID = GetGame()->GetNextBoxID();
	GetGame()->GetInstancedBoxes().Add(ibd.ID, ibd);

	icd.ID = GetGame()->GetNextInstanceContainerDataID();
	icd.containerID = containerData.ID;
	icd.type = EContainerType::Box;
	icd.name = "Loot Box " + FString::FromInt(ibd.boxID);
	GetGame()->GetInstancedContainers().Add(icd.ID, icd);

	container = UItemContainer::CreateItemContainer(GetGame()->GetContainerDataByID(containerData.ID), icd, gameIn);
	container->OnItemRemoved.AddUniqueDynamic(this, &ALootBox::ItemRemoved);
	container->OnItemAdded.AddUniqueDynamic(this, &ALootBox::ItemAdded);
}

FInstanceItemData ALootBox::CreateLoot(FItemData id)
{
	FInstanceItemData iid;
	if (id.ID != UItemStructs::InvalidInt)
	{
		iid.ID = GetGame()->GetNextInstanceItemDataID();
		iid.amount = FMath::RandRange(1, id.maxStack);
		iid.containerInstanceID = icd.ID;
		iid.itemID = id.ID;
		iid.slot = container->GetNextEmptySlot();
	}
	return iid;
}

void ALootBox::ItemAdded(FInstanceItemData inItem)
{
}

void ALootBox::RemoveLootBox()
{
	// TODO sort out loot boxes despawning etc.
	//Destroy();
}

void ALootBox::ItemRemoved(FInstanceItemData inItem)
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
