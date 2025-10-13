#include "BuildingPart.h"

#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Kismet/GameplayStatics.h"
#include "SurvivalTest/BaseGameInstance.h"
#include "SurvivalTest/ObjectInstanceManager.h"

ABuildingPart::ABuildingPart() : meshID(0), mesh(nullptr), buildingData()
{
	static ConstructorHelpers::FObjectFinder<UStaticMesh> baseMesh(TEXT("StaticMesh'/Game/FirstPerson/LootBoxes/Crate.Crate'"));
	PrimaryActorTick.bCanEverTick = false;
	mesh = baseMesh.Object;
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	
	arrow = CreateDefaultSubobject<UArrowComponent>(TEXT("Building Arrow"));
	arrow->SetupAttachment(RootComponent);
	arrow->ArrowSize = 2.0f;
	arrow->bIsScreenSizeScaled = true;

	collision = CreateDefaultSubobject<UBoxComponent>(TEXT("Collision"));
	collision->SetupAttachment(RootComponent);
	//collision->SetBoxExtent(FVector(mesh->GetExtendedBounds().BoxExtent.GetUnsafeNormal()));
	collision->SetBoxExtent(FVector(50, 50, 50));
	collision->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	collision->SetCollisionResponseToAllChannels(ECR_Block);
}

void ABuildingPart::BeginPlay()
{
	Super::BeginPlay();

	if (mesh)
	{
		FTransform trans(GetActorRotation(), GetActorLocation(), FVector(1));
		meshID = GameInstance(GetWorld())->GetObjectInstanceManager()->CreateStaticMesh(mesh, trans);

		if (meshID == -1)
		{
			Destroy();
		}
		else
		{
			collision->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
			collision->SetCollisionResponseToAllChannels(ECR_Overlap);
		}
	}
}