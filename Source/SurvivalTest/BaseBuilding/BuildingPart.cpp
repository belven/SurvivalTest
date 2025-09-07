#include "BuildingPart.h"

ABuildingPart::ABuildingPart()
{
	static ConstructorHelpers::FObjectFinder<UStaticMesh> baseMesh(TEXT("StaticMesh'/Game/FirstPerson/LootBoxes/Crate.Crate'"));
	PrimaryActorTick.bCanEverTick = false;
	buildingPartComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Building Part Mesh"));
	buildingPartComp->SetStaticMesh(baseMesh.Object);
	//RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));
	arrow = CreateDefaultSubobject<UArrowComponent>(TEXT("Building Arrow"));

	arrow->SetupAttachment(buildingPartComp);

	arrow->ArrowSize = 2.0f;
	arrow->bIsScreenSizeScaled = true;
}

void ABuildingPart::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);

	if (!meshRef.IsEmpty())
	{
		UStaticMesh* mesh = LoadObject<UStaticMesh>(this, *meshRef);
		buildingPartComp->SetStaticMesh(mesh);
	}
}

void ABuildingPart::BeginPlay()
{
	Super::BeginPlay();
	
	if (!meshRef.IsEmpty())
	{
		UStaticMesh* mesh = LoadObject<UStaticMesh>(this, *meshRef);
		buildingPartComp->SetStaticMesh(mesh);
	}
}