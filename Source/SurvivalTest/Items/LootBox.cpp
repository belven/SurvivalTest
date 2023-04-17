#include "LootBox.h"

#include "Armour.h"
#include "SurvivalTest/BaseGameInstance.h"
#include "ItemContainer.h"
#include "SurvivalTest/Tables/Items/ItemDataTable.h"
#include "SurvivalTest/Tables/TableManager.h"

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
	if (boxMesh) {
		boxMeshComp->SetStaticMesh(boxMesh);
		//boxMeshComp->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	}
}

void ALootBox::BeginPlay()
{
	Super::BeginPlay();
	CreateLootboxData();
	SetUpBox();
}

void ALootBox::Interact(ABasePlayerController* instigator)
{
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

TMap<EItemType, TArray<FItemData>> ALootBox::GetItemList()
{
	TMap<EItemType, TArray<FItemData>> items;
	TArray<EItemType> types;
	itemTypes.GenerateKeyArray(types);

	for (auto item : GetGame()->GetTableManager()->GetItemDataTable()->GetData())
	{
		if (types.Contains(item.type)) {
			items.FindOrAdd(item.type).Add(item);
		}
	}

	return items;
}

void ALootBox::SpawnLoot()
{
	// runnable = new FSpawnLootRunnable();
	//runnable->lootBox = this;
	//FRunnableThread* thread = FRunnableThread::Create(runnable, TEXT(""));
	TMap<EItemType, TArray<FItemData>> itemList = GetItemList();

	for (auto& lootItem : itemTypes)
	{
		TArray<FItemData> items = itemList.FindOrAdd(lootItem.Key);

		for (int i = 0; i < lootItem.Value; ++i)
		{
			FItemData id = items[FMath::RandRange(0, items.Num() - 1)];
			FInstanceItemData iid = CreateLoot(id);

			TArray<int32> ids;
			FInstanceItemData added = container->AddItem(iid, ids);

			if (added.amount == 0 && id.type == EItemType::Armour)
			{
				UArmour::CreateArmour(id.ID, GetGame(), ids[0]);
			}
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
	icd.containerID = containerID;
	icd.type = EContainerType::Box;
	icd.name = "Loot Box " + FString::FromInt(ibd.boxID);
	GetGame()->GetInstancedContainers().Add(icd.ID, icd);

	container = UItemContainer::CreateItemContainer(GetGame()->GetContainerDataByID(containerID), icd, gameIn);
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
	SetUpBox();
}

FSpawnLootRunnable::FSpawnLootRunnable() : lootBox(nullptr)
{

}

bool FSpawnLootRunnable::Init()
{
	return true;
}

uint32 FSpawnLootRunnable::Run()
{
	TMap<EItemType, TArray<FItemData>> itemList = lootBox->GetItemList();

	for (auto& lootItem : lootBox->itemTypes)
	{
		TArray<FItemData> items = itemList.FindOrAdd(lootItem.Key);

		for (int i = 0; i < lootItem.Value; ++i)
		{
			FItemData id = items[FMath::RandRange(0, items.Num() - 1)];
			FInstanceItemData iid = CreateLoot(id);

			TArray<int32> ids;
			lootBox->container->AddItem(iid, ids);

			if (id.type == EItemType::Armour)
			{
				UArmour::CreateArmour(id.ID, lootBox->GetGame(), ids[0]);
			}
		}
	}

	return 0;
}

FInstanceItemData FSpawnLootRunnable::CreateLoot(FItemData id)
{
	FInstanceItemData iid;
	if (id.ID != UItemStructs::InvalidInt)
	{
		iid.ID = lootBox->GetGame()->GetNextInstanceItemDataID();
		iid.amount = FMath::RandRange(1, id.maxStack);
		iid.containerInstanceID = lootBox->icd.ID;
		iid.itemID = id.ID;
		iid.slot = lootBox->container->GetNextEmptySlot();
	}
	return iid;
}
