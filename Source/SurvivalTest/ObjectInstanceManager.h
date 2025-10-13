#pragma once
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ObjectInstanceManager.generated.h"

class UStaticMesh;
class UInstancedStaticMeshComponent;

UCLASS()
class SURVIVALTEST_API AObjectInstanceManager : public AActor
{
	GENERATED_BODY()

public:
	AObjectInstanceManager();
	int32 CreateStaticMesh(UStaticMesh* mesh, const FTransform& Transform);


	UPROPERTY()
	TArray<FVector> existing_points;

	UPROPERTY()
	TMap<UStaticMesh*, UInstancedStaticMeshComponent*> meshComps;
};