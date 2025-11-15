#include "ObjectInstanceManager.h"
#include  "Components/InstancedStaticMeshComponent.h"

AObjectInstanceManager::AObjectInstanceManager()
{
	PrimaryActorTick.bCanEverTick = false;
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
}

int32 AObjectInstanceManager::CreateStaticMesh(UStaticMesh* mesh, const FTransform& Transform, bool applyCustomStencilDepth)
{
	int32 ID = -1;
	UInstancedStaticMeshComponent* comp = NULL;

	if (!existing_points.Contains(Transform.GetLocation()))
	{
		if (meshComps.Contains(mesh))
		{
			comp = *meshComps.Find(mesh);
		}
		else
		{
			comp = NewObject<UInstancedStaticMeshComponent>(this);

			if (applyCustomStencilDepth) {
				comp->SetCustomDepthStencilValue(2);
			}

			comp->SetStaticMesh(mesh);
			comp->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
			comp->SetupAttachment(RootComponent);
			comp->RegisterComponent();
			meshComps.Add(mesh, comp);
		}

		FTransform LocalTransform = Transform.GetRelativeTransform(comp->GetComponentTransform());
		ID = comp->AddInstance(LocalTransform);
		existing_points.Add(Transform.GetLocation());
	}
	return ID;
}