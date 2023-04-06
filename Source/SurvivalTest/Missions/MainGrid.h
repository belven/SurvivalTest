#pragma once
#include "CoreMinimal.h"
#include "Engine/TargetPoint.h"
#include "MainGrid.generated.h"

class AGridSection;

UCLASS()
class SURVIVALTEST_API AMainGrid : public ATargetPoint
{
	GENERATED_BODY()

public:
	void ClearGrid();
	virtual void OnConstruction(const FTransform& Transform) override;
	AMainGrid();

private:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Grid", meta = (AllowPrivateAccess = "true"))
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

	UPROPERTY()
	UStaticMesh* mesh;
};
