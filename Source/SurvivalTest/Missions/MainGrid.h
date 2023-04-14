#pragma once
#include "CoreMinimal.h"
#include "Engine/TargetPoint.h"
#include "MainGrid.generated.h"

class AGridSection;

UCLASS(HideCategories=(Rendering, Replication, Collision, HLOD, World_Partition, Input, Replication, Actor, Cooking, Data_Layers, Physics, Lighting, Navigation, Tags, Activation))
class SURVIVALTEST_API AMainGrid : public ATargetPoint
{
	GENERATED_BODY()

public:
	void ClearGrid();
	virtual void OnConstruction(const FTransform& Transform) override;
	AGridSection* GetGridSection(FVector loc);
	int32 ModValue(double value);
	AMainGrid();

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

private:
	UPROPERTY(VisibleDefaultsOnly)
	TArray<AGridSection*> gridSections;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Grid", meta = (AllowPrivateAccess = "true"))
	int32 columnsAndRows = 19;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Grid", meta = (AllowPrivateAccess = "true"))
	int32 boxSize = 3500;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Grid", meta = (AllowPrivateAccess = "true"))
	int32 boxHeight = 0;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Grid", meta = (AllowPrivateAccess = "true"))
	int32 debugDuration = 10;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Grid", meta = (AllowPrivateAccess = "true"))
	int32 heightAboveGround = 300;

	UPROPERTY(VisibleDefaultsOnly)
	UStaticMesh* mesh;
	//UE::Math::TVector2<AGridSection*> grid;
	//AGridSection* grid[][];
};
