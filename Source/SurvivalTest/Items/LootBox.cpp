#include "LootBox.h"

#include "Armour.h"
#include "SurvivalTest/BaseGameInstance.h"
#include "ItemContainer.h"

ALootBox::ALootBox()
{
	PrimaryActorTick.bCanEverTick = true;
	boxMeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Loot Box Mesh"));
	boxMeshComp->SetCustomDepthStencilValue(2);	
}

void ALootBox::SetUpBox()
{
	if (boxMesh) {
		boxMeshComp->SetStaticMesh(boxMesh);
		boxMeshComp->SetCollisionProfileName("Interaction");
		boxMeshComp->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		//RootComponent->AttachToComponent(boxMeshComp, FAttachmentTransformRules::KeepRelativeTransform);
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
	isHighlighted = activate;
	boxMeshComp->SetRenderCustomDepth(activate);
}

UBaseGameInstance* ALootBox::GetGame()
{
	if (!gameIn)
		gameIn = GameInstance(GetWorld());
	return gameIn;
}

void ALootBox::CreateLootboxData()
{
	int32 instanceContainerDataID = GetGame()->GetNextInstanceContainerDataID();

	icd.ID = instanceContainerDataID;
	icd.containerID = containerID;
	icd.type = EContainerType::Box;
	icd.name = "Loot Box " + FString::FromInt(boxID);
	GetGame()->GetInstancedContainers().Add(icd.ID, icd);	

	int32 instanceBoxDataID = GetGame()->GetNextInstanceBoxDataID();
	
	ibd.ID = instanceBoxDataID;
	ibd.containerInstanceID = icd.ID;
	ibd.boxID = boxID;
	GetGame()->GetInstancedBoxes().Add(ibd.ID, ibd);
	container = UItemContainer::CreateItemContainer(GetGame()->GetContainerDataByID(containerID), icd, gameIn);
	//container->OnItemRemoved.AddUniqueDynamic(this, &ALootBox::ItemRemoved);
	//container->OnItemAdded.AddUniqueDynamic(this, &ALootBox::ItemAdded);


	for (int32 i = 0; i < itemQuantity - 1; i++)
	{
		FItemData id = UItemStructs::GetRandomItemData(GetGame());
		FInstanceItemData iid = CreateLoot(id);

		TArray<int32> ids;
		container->AddItem(iid, ids);

		if(id.type == EItemType::Armour)
		{
			UArmour::CreateArmour(id.ID, GetGame(), ids[0]);
		}
	}
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

void ALootBox::ItemRemoved(FInstanceItemData inItem)
{
}

void ALootBox::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ALootBox::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);
	SetUpBox();
}
