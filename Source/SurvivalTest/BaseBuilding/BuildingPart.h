#pragma once
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/ArrowComponent.h"
#include "Components/BoxComponent.h"
#include "BuildingPart.generated.h"

USTRUCT(BlueprintType)
struct FBuildingPartData
{
	GENERATED_USTRUCT_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Building Part")
	int32 ID;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Building Part")
	FString meshRef;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Building Part")
	FVector location;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Building Part")
	FRotator rotation;
};

class UStaticMeshComponent;

UCLASS()
class SURVIVALTEST_API ABuildingPart : public AActor
{
	GENERATED_BODY()

public:
	ABuildingPart();

	/*UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Building Part", meta = (AllowPrivateAccess = "true"))
	UStaticMeshComponent* buildingPartComp;*/


protected:
	virtual void BeginPlay() override;

	int32 meshID;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Building Part", meta = (AllowPrivateAccess = "true"))
	UStaticMesh* mesh;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Building Part", meta = (AllowPrivateAccess = "true"))
	FString meshRef;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Building Part", meta = (AllowPrivateAccess = "true"))
	FBuildingPartData buildingData;

	UPROPERTY()
	UArrowComponent* arrow;

	UPROPERTY()
	UBoxComponent* collision;
};